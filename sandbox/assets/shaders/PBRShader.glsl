#region Vertex

#version 440 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec4 a_boneIndices;
layout(location = 4) in vec4 a_boneWeights;
layout(location = 5) in mat4 a_model;
layout(location = 9) in vec4 a_tint; // MATERIAL m_tint
layout(location = 10) in int a_albedo;
layout(location = 11) in int a_metallic;
layout(location = 12) in int a_roughness;
layout(location = 13) in int a_ao;
layout(location = 14) in int a_normal;

out vec3 worldPos;
out vec3 norm;
out vec2 texCoords;
out vec4 tints;

out flat int Albedo;
out flat int Metallic;
out flat int Roughness;
out flat int Ao;
out flat int Normal;

out mat4 model;

layout (std140) uniform b_camera
{
	mat4 u_projection;
	mat4 u_view;
};

uniform int ImmediateMode;

uniform int AlbedoTex;
uniform int MetallicTex;
uniform int RoughnessTex;
uniform int AOTex;
uniform int NormalTex;
uniform mat4 ModelMat;
uniform vec4 TintCol;

uniform mat4 boneMatrices[100];

void main()
{
    if (ImmediateMode == 1)
    {
	    Albedo = AlbedoTex;
	    Metallic = MetallicTex;
	    Roughness = RoughnessTex;
	    Ao = AOTex;
        Normal = NormalTex;
        model = ModelMat;
        tints = vec4(1.0f); //TintCol;
    }
    else
    {
	    Albedo = a_albedo;
	    Metallic = a_metallic;
	    Roughness = a_roughness;
	    Ao = a_ao;
        Normal = a_normal;
        model = a_model;
        tints = a_tint;
    }

    mat4 boneTransform = boneMatrices[int(a_boneIndices[0])] * a_boneWeights[0];
    boneTransform += boneMatrices[int(a_boneIndices[1])] * a_boneWeights[1];
    boneTransform += boneMatrices[int(a_boneIndices[2])] * a_boneWeights[2];
    boneTransform += boneMatrices[int(a_boneIndices[3])] * a_boneWeights[3];

    if (boneTransform == mat4(0.f))
    {
        mat4 MVP = u_projection * u_view * model;
	    worldPos = vec3(model * vec4(a_vertexPosition, 1.0));
	    norm = normalize(mat3(transpose(inverse(model))) * a_vertexNormal);
	    texCoords = vec2(a_texCoord.x, a_texCoord.y);

	    gl_Position = MVP * vec4(a_vertexPosition,1.0);
    }
    else
    {
	    worldPos = vec3(model * boneTransform * vec4(a_vertexPosition, 1.0));
	    norm = normalize(mat3(transpose(inverse(model))) * a_vertexNormal);
	    texCoords = vec2(a_texCoord.x, a_texCoord.y);

	    gl_Position = u_projection * u_view * vec4(worldPos,1.0);
    }
}

#region Geometry

#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldPos[];
in vec3 norm[];
in vec2 texCoords[];
in vec4 tints[];

in flat int Albedo[];
in flat int Metallic[];
in flat int Roughness[];
in flat int Ao[];
in flat int Normal[];

in mat4 model[];

out vec3 gworldPos;
out vec3 normal;
out vec2 texCoord;
out vec4 tint;

out flat int texAlbedo;
out flat int texMetallic;
out flat int texRoughness;
out flat int texAo;
out flat int texNormal;

out mat4 Model;

out mat3 TBNMat;

void main()
{
    for(int i = 0; i < 3; i++)
    {
        int v1ID = int(mod(i, 3));
        int v2ID = int(mod(i+1, 3));
        int v3ID = int(mod(i+2, 3));

        vec3 edge1 = gl_in[v2ID].gl_Position.xyz - gl_in[v1ID].gl_Position.xyz;
        vec3 edge2 = gl_in[v3ID].gl_Position.xyz - gl_in[v1ID].gl_Position.xyz;
        vec3 vNormal = normalize(cross(edge1, edge2));

        vec3 tangent, bitangent;

        vec3 helper = (abs(norm[i].x) > 0.99) ? vec3(0, 1, 0) : vec3(1, 0, 0);

        tangent = normalize(cross(norm[i], helper));
        bitangent = normalize(cross(norm[i], tangent));

        mat3 TBN = mat3(tangent, bitangent, norm[i]);

        gl_Position = gl_in[i].gl_Position;
        gworldPos = worldPos[i];
        texCoord = texCoords[i];
        normal = vNormal;
        tint = tints[i];
        texAlbedo = Albedo[i];
        texMetallic = Metallic[i];
        texRoughness = Roughness[i];
        texAo = Ao[i];
        texNormal = Normal[i];
        Model = model[i];
        TBNMat = TBN;
        EmitVertex();
    }
    
    EndPrimitive();
}

#region Fragment
#version 440 core
			
layout(location = 0) out vec4 FragColor;

in vec3 gworldPos;
in vec3 normal;
in vec2 texCoord;
in vec4 tint;

in flat int texAlbedo;
in flat int texMetallic;
in flat int texRoughness;
in flat int texAo;
in flat int texNormal;

in mat3 TBNMat;

in mat4 Model;

layout (std140) uniform b_lights
{
	vec3 u_viewPos; 
};

const float PI = 3.14159265359;

uniform sampler2D[32] u_texData;

uniform vec3[64] u_lightColour;
uniform vec3[64] u_lightPos;

float DistributionGGX (vec3 N, vec3 H, float roughness){
    float a2    = roughness * roughness * roughness * roughness;
    float NdotH = max (dot (N, H), 0.0);
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float geometrySchlickGGX (float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith (vec3 N, vec3 V, vec3 L, float roughness){
    return geometrySchlickGGX (max (dot (N, L), 0.0), roughness) * 
           geometrySchlickGGX (max (dot (N, V), 0.0), roughness);
}

vec3 fresnelSchlick (float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow (1.0 - cosTheta, 5.0);
}

uniform float scale = 0.0;

vec2 calculateTriplanarTexCoords(vec3 worldPos, float texScale) {
    vec3 n = normalize(TBNMat * normal);

    vec3 blendWeights = abs(n);
    blendWeights = blendWeights / dot(blendWeights, vec3(1.0));

    vec2 primaryTexCoords;

    if(blendWeights.x > blendWeights.y && blendWeights.x > blendWeights.z) {
        primaryTexCoords = worldPos.yz * texScale;
    } 
    else if(blendWeights.y > blendWeights.x && blendWeights.y > blendWeights.z) {
        primaryTexCoords = worldPos.xz * texScale;
    } 
    else {
        primaryTexCoords = worldPos.xy * texScale;
    }
    return primaryTexCoords;
}

void main()
{
    vec2 newTexCoords;
    float texScale;
    if (scale == 1.0)
    {
        float scaleX = length(vec3(Model[0].x, Model[1].x, Model[2].x));
        float scaleY = length(vec3(Model[0].y, Model[1].y, Model[2].y));
        float scaleZ = length(vec3(Model[0].z, Model[1].z, Model[2].z));
        texScale = 1/min(min(scaleX, scaleY), scaleZ);

        newTexCoords = calculateTriplanarTexCoords(gworldPos, texScale);
    }
    else
    {
        
        texScale = scale;
    }

    newTexCoords = texCoord;

    vec3 albedo = texture(u_texData[texAlbedo], newTexCoords).xyz;
    float metallic = texture(u_texData[texMetallic], newTexCoords).r;
    float roughness = texture(u_texData[texRoughness], newTexCoords).r;
    float ao = texture(u_texData[texAo], newTexCoords).r;
    vec3 Normal = texture(u_texData[texNormal], newTexCoords).rgb;
    if (!(Normal == vec3(1,1,1)))
        Normal = normalize(TBNMat * (Normal * 2.0 - 1.0));
    else
    {
        albedo = vec3(0.7, 0.3, 0.3);
        metallic = 0.5;
        roughness = 0.5;
        Normal = normalize(normal);
    }

	vec3 N = Normal;
    vec3 V = normalize(gworldPos - u_viewPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    
	for (int i = 0; i < 64; i++)
	{
    // calculate per-light radiance
    vec3 L = normalize(u_lightPos[i] - gworldPos);
    vec3 H = normalize(V + L);
    float distance    = length(u_lightPos[i] - gworldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = u_lightColour[i] * attenuation;        
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }
	
    vec3 ambient = 0.1 * albedo * ao;
    vec3 color =  ambient + Lo;
   
    FragColor = vec4(color, 1.0) * tint;
    //FragColor = vec4(Normal * 0.5 + 0.5, 1.0);
}