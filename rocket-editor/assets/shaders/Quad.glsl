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

	v_normal = a_normal;
	v_fragPos = a_position;

	gl_Position = u_viewProjection * vec4(a_position, 1.0);		
}

#type fragment
#version 450

layout (location = 0) out vec4 color;
layout (location = 1) out int entityID;
		
in vec4 v_color;
in vec2 v_textureCoord;
in flat float v_texIndex;
in float v_tilingFactor;
in flat int v_entityID;

in vec3 v_fragPos;
in vec3 v_normal;

uniform sampler2D u_textures[32];

//temp
uniform vec3 u_viewPosition;
uniform vec3 u_diffusePosition;
uniform vec3 u_diffuseColor; 

void main()
{
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

	float ambientStrenght = 0.2f;
	vec3 ambient = vec3(ambientStrenght * vec4(1.0, 1.0, 1.0, 1.0));

	vec3 norm = normalize(v_normal);

	vec3 lightDir = normalize(u_diffusePosition - v_fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * u_diffuseColor;

	float specularStrenght = 0.5;
	vec3 viewDirection = normalize(u_viewPosition - v_fragPos);
	vec3 reflectDirection = reflect(-lightDir, norm);
	// 32 is the shininess
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 256);
	vec3 specular = specularStrenght * spec * u_diffuseColor;


	vec3 result = (ambient + diffuse + specular) * textureColor.xyz;
	
	color = vec4(result, 1.0);
	entityID = v_entityID; // placeholder for entity ID
}