#type vertex
#version 450
			
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_textureCoord;
layout (location = 2) in vec4 a_color;
layout (location = 3) in vec3 a_normal;
layout (location = 4) in float a_texIndex;
layout (location = 5) in float a_tilingFactor;
layout (location = 6) in int a_entityID;

uniform mat4 u_viewProjection;

out vec4 v_color;
out vec2 v_textureCoord;
out flat float v_texIndex;
out float v_tilingFactor;
out flat int v_entityID;

out vec3 v_normal;
out vec3 v_fragPos;

void main()
{
	v_entityID = a_entityID;
	v_color = a_color;
	v_textureCoord = a_textureCoord;
	v_texIndex = a_texIndex;
	v_tilingFactor = a_tilingFactor;

	v_normal = normalize(a_normal);
	v_fragPos = a_position;

	gl_Position = u_viewProjection * vec4(a_position, 1.0);		
}

#type fragment
#version 450

#define MAX_DIRECTIONAL_LIGHTS 10
#define MAX_POINT_LIGHTS 100

layout (location = 0) out vec4 color;
layout (location = 1) out int entityID;
		
in vec4 v_color;
in vec2 v_textureCoord;
in flat float v_texIndex;
in float v_tilingFactor;
in flat int v_entityID;

in vec3 v_fragPos;
in vec3 v_normal;

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// lights counters
uniform int u_directionalLightCount;
uniform int u_pointLightCount;

// ambient strenghts
uniform float u_ambientStrenghtsDirectional[MAX_DIRECTIONAL_LIGHTS];
uniform float u_ambientStrenghtsPointLight[MAX_POINT_LIGHTS];

//lights themselves
uniform DirectionalLight directionals[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
// camera view position for specular lighting
uniform vec3 u_viewPosition;

uniform sampler2D u_textures[32];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, float ambientStrenght, float specularStrenght);
vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float ambientStrenght);

void main()
{
	vec3 normal = normalize(v_normal);
	vec4 textureColor = v_color;
	switch(int(v_texIndex)) {
		case 0: textureColor *= texture(u_textures[0], v_textureCoord * v_tilingFactor); break;
		case 1: textureColor *= texture(u_textures[1], v_textureCoord * v_tilingFactor); break;
		case 2: textureColor *= texture(u_textures[2], v_textureCoord * v_tilingFactor); break;
		case 3: textureColor *= texture(u_textures[3], v_textureCoord * v_tilingFactor); break;
		case 4: textureColor *= texture(u_textures[4], v_textureCoord * v_tilingFactor); break;
		case 5: textureColor *= texture(u_textures[5], v_textureCoord * v_tilingFactor); break;
		case 6: textureColor *= texture(u_textures[6], v_textureCoord * v_tilingFactor); break;
		case 7: textureColor *= texture(u_textures[7], v_textureCoord * v_tilingFactor); break;
		case 8: textureColor *= texture(u_textures[8], v_textureCoord * v_tilingFactor); break;
		case 9: textureColor *= texture(u_textures[9], v_textureCoord * v_tilingFactor); break;
		case 10: textureColor *= texture(u_textures[10], v_textureCoord * v_tilingFactor); break;
		case 11: textureColor *= texture(u_textures[11], v_textureCoord * v_tilingFactor); break;
		case 12: textureColor *= texture(u_textures[12], v_textureCoord * v_tilingFactor); break;
		case 13: textureColor *= texture(u_textures[13], v_textureCoord * v_tilingFactor); break;
		case 14: textureColor *= texture(u_textures[14], v_textureCoord * v_tilingFactor); break;
		case 15: textureColor *= texture(u_textures[15], v_textureCoord * v_tilingFactor); break;
		case 16: textureColor *= texture(u_textures[16], v_textureCoord * v_tilingFactor); break;
		case 17: textureColor *= texture(u_textures[17], v_textureCoord * v_tilingFactor); break;
		case 18: textureColor *= texture(u_textures[18], v_textureCoord * v_tilingFactor); break;
		case 19: textureColor *= texture(u_textures[19], v_textureCoord * v_tilingFactor); break;
		case 20: textureColor *= texture(u_textures[20], v_textureCoord * v_tilingFactor); break;
		case 21: textureColor *= texture(u_textures[21], v_textureCoord * v_tilingFactor); break;
		case 22: textureColor *= texture(u_textures[22], v_textureCoord * v_tilingFactor); break;
		case 23: textureColor *= texture(u_textures[23], v_textureCoord * v_tilingFactor); break;
		case 24: textureColor *= texture(u_textures[24], v_textureCoord * v_tilingFactor); break;
		case 25: textureColor *= texture(u_textures[25], v_textureCoord * v_tilingFactor); break;
		case 26: textureColor *= texture(u_textures[26], v_textureCoord * v_tilingFactor); break;
		case 27: textureColor *= texture(u_textures[27], v_textureCoord * v_tilingFactor); break;
		case 28: textureColor *= texture(u_textures[28], v_textureCoord * v_tilingFactor); break;
		case 29: textureColor *= texture(u_textures[29], v_textureCoord * v_tilingFactor); break;
		case 30: textureColor *= texture(u_textures[30], v_textureCoord * v_tilingFactor); break;
		case 31: textureColor *= texture(u_textures[31], v_textureCoord * v_tilingFactor); break;
	}

	float specularStrenght = 0.6; // TODO: move all hard code variables to uniforms/UBOs/SSBOs UPD maybe not ;)
	vec3 result = vec3(0.0, 0.0, 0.0);
	for (int i = 0; i < u_directionalLightCount; i++) {
		vec3 viewDirection = normalize(u_viewPosition - v_fragPos);
		result += CalculateDirectionalLight(directionals[i], normal, viewDirection, u_ambientStrenghtsDirectional[i], specularStrenght);
	}

	for (int i = 0; i < u_pointLightCount; i++) {
		vec3 viewDirection = normalize(u_viewPosition - v_fragPos);
		result += CalculatePointLights(pointLights[i], normal, v_fragPos, viewDirection, u_ambientStrenghtsPointLight[i]);
	}

	result = result * textureColor.xyz;

	color = vec4(result, 1.0);	
	entityID = v_entityID; // placeholder for entity ID
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir,
							   float ambientStrenght, float specularStrenght) {
	// TODO: add materials
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	// last parameter is shininess
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
	//combine results
	vec3 ambient = light.ambient * ambientStrenght;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec * specularStrenght;
	return (ambient + diffuse + specular);
}

vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float ambientStrenght) {
    vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
	 // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	 // combine results
    vec3 ambient = light.ambient * ambientStrenght;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
