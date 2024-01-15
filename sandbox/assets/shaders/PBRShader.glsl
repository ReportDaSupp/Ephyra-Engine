#region Vertex

#version 440 core
			
layout(location = 0) in vec3 a_vertexPosition;
layout(location = 1) in vec3 a_vertexNormal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in mat4 a_model;
layout(location = 7) in int a_texUnit; // MATERIAL m_texture
layout(location = 8) in vec4 a_tint; // MATERIAL m_tint
layout(location = 9) in vec3 a_albedo;
layout(location = 10) in float a_metallic;
layout(location = 11) in float a_roughness;
layout(location = 12) in float a_ao;

out vec3 fragmentPos;
out vec3 normal;
out vec2 texCoord;
out flat int texUnit;
out vec4 tint;

out vec3 albedo;
out float metallic;
out float roughness;
out float ao;

layout (std140) uniform b_camera
{
	mat4 u_projection;
	mat4 u_view;
};

void main()
{
	mat4 MVP = u_projection * u_view * a_model;
	fragmentPos = vec3(a_model * vec4(a_vertexPosition, 1.0));
	normal = mat3(transpose(inverse(a_model))) * a_vertexNormal;
	texCoord = vec2(a_texCoord.x, a_texCoord.y);
	texUnit = a_texUnit;
	tint = a_tint;
	albedo = a_albedo;
	metallic = a_metallic;
	roughness = a_roughness;
	ao = a_ao;
	gl_Position =  MVP * vec4(a_vertexPosition,1.0);
}

#region Fragment
#version 440 core
			
layout(location = 0) out vec4 FragColor;

in vec3 fragmentPos;
in vec3 normal;
in vec2 texCoord;
in flat int texUnit;
in vec4 tint;

in vec3 albedo;
in float metallic;
in float roughness;
in float ao;

layout (std140) uniform b_lights
{
	vec3 u_viewPos; 
};

const float PI = 3.14159265359;

uniform sampler2D[32] u_texData;

uniform vec3[4] u_lightColour;
uniform vec3[4] u_lightPos;

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

void main()
{
	vec3 N = normalize(normal);
    vec3 V = normalize(u_viewPos - fragmentPos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    
	for (int i = 0; i < 4; i++)
	{
    // calculate per-light radiance
    vec3 L = normalize(u_lightPos[i] - fragmentPos);
    vec3 H = normalize(V + L);
    float distance    = length(u_lightPos[i] - fragmentPos);
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
	
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  
   
    FragColor = vec4(color, 1.0)* texture(u_texData[texUnit], texCoord)  * tint;
}