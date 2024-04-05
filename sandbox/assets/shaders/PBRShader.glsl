#region Vertex

#version 440 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in mat4 a_model;
layout(location = 7) in vec4 a_tint; // MATERIAL m_tint
layout(location = 8) in int a_albedo;
layout(location = 9) in int a_metallic;
layout(location = 10) in int a_roughness;
layout(location = 11) in int a_ao;
layout(location = 12) in int a_normal;

out vec3 fragPos;
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

void main()
{
	mat4 MVP = u_projection * u_view * a_model;
	fragPos = vec3(a_model * vec4(a_vertexPosition, 1.0));
	norm = mat3(transpose(inverse(a_model))) * a_vertexNormal;
	texCoords = vec2(a_texCoord.x, a_texCoord.y);
	tints = a_tint;
	Albedo = a_albedo;
	Metallic = a_metallic;
	Roughness = a_roughness;
    
	Ao = a_ao;
    Normal = a_normal;
    model = a_model;
	gl_Position =  MVP * vec4(a_vertexPosition,1.0);
}

#region Geometry

#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 fragPos[];
in vec3 norm[];
in vec2 texCoords[];
in vec4 tints[];

in flat int Albedo[];
in flat int Metallic[];
in flat int Roughness[];
in flat int Ao[];
in flat int Normal[];

in mat4 model[];

out vec3 fragmentPos;
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
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    //vec3 edge0 = fragPos[1].xyz - fragPos[0].xyz;
    //vec3 edge1 = fragPos[2].xyz - fragPos[0].xyz;

    vec2 deltaUV0 = texCoords[1] - texCoords[0];
    vec2 deltaUV1 = texCoords[2] - texCoords[0];

    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));

    vec3 T = normalize(vec3(model[0] * vec4(tangent, 0.0f)));
    vec3 N = normalize(vec3(model[0] * vec4(norm[0], 0.0f)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    //vec3 size;
    //size.x = length(vec3(model[0]));
    //size.y = length(vec3(model[1]));
    //size.z = length(vec3(model[2]));

    for(int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        fragmentPos = fragPos[i];
        //texCoord = texCoords[i] * size.xy;
        texCoord = texCoords[i];
        normal = norm[i];
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

in vec3 fragmentPos;
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
    vec3 n = normalize(normal);

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

    vec3 worldNormal = normalize((Model * vec4(normal, 0.0)).xyz);
    newTexCoords = calculateTriplanarTexCoords(fragmentPos, texScale);
    }
    else
    {
        newTexCoords = texCoord;
        texScale = scale;
    }

    vec3 albedo = texture(u_texData[texAlbedo], newTexCoords).xyz;
    float metallic = texture(u_texData[texMetallic], newTexCoords).r;
    float roughness = texture(u_texData[texRoughness], newTexCoords).r;
    float ao = texture(u_texData[texAo], newTexCoords).r;
    vec3 Normal = texture(u_texData[texNormal], newTexCoords).rgb;
    Normal = Normal * 2.0 - 1.0;

	vec3 N = normalize(Normal);
    vec3 V = normalize(TBNMat * u_viewPos - TBNMat * fragmentPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    
	for (int i = 0; i < 64; i++)
	{
    // calculate per-light radiance
    vec3 L = normalize(TBNMat * u_lightPos[i] - TBNMat * fragmentPos);
    vec3 H = normalize(V + L);
    float distance    = length(TBNMat *u_lightPos[i] - TBNMat *fragmentPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance     = u_lightColour[i] * attenuation;        
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }
	
    vec3 ambient = vec3(0.2) * albedo * ao;
    vec3 color = ambient + Lo;
	
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2));  
   
    FragColor = vec4(color, 1.0) * tint;
}