#type vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;
layout(location = 3) in vec3 a_normal;
layout(location = 4) in float a_texIndex;
layout(location = 5) in float a_tilingFactor;
layout(location = 6) in int a_entityID;

uniform	mat4 u_viewProjection;

out vec4 v_color;
out vec2 v_texCoord;
out float v_texIndex;
out float v_tilingFactor;
out flat int v_entityID;

// for lighting calculations
out vec3 v_fragPos;
out vec3 v_normal;

void main() {
	v_color = a_color;
	v_texCoord = a_texCoord;
	v_tilingFactor = a_tilingFactor;
	v_texIndex = a_texIndex;
	v_entityID = a_entityID;

	//lighting
	v_fragPos = a_position;
	v_normal = a_normal;

	gl_Position = u_viewProjection * vec4(a_position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entityID;
layout(location = 2) out vec4 bloomColor;

in vec4 v_color;
in vec2 v_texCoord;
in float v_texIndex;
in float v_tilingFactor;
in flat int v_entityID;

//lighting calculations
in vec3 v_fragPos;
in vec3 v_normal;

uniform vec3 u_viewPosition;

uniform sampler2D u_textures[32];

uniform bool u_isHDREnabled;
uniform bool u_isPostProcessingEnabled;

vec3 CalculateAllLights();

void main()
{
	vec4 textureColor = v_color;

	switch(int(v_texIndex)) {
		case 0: textureColor *= texture(u_textures[0], v_texCoord * v_tilingFactor); break;
		case 1: textureColor *= texture(u_textures[1], v_texCoord * v_tilingFactor); break;
		case 2: textureColor *= texture(u_textures[2], v_texCoord * v_tilingFactor); break;
		case 3: textureColor *= texture(u_textures[3], v_texCoord * v_tilingFactor); break;
		case 4: textureColor *= texture(u_textures[4], v_texCoord * v_tilingFactor); break;
		case 5: textureColor *= texture(u_textures[5], v_texCoord * v_tilingFactor); break;
		case 6: textureColor *= texture(u_textures[6], v_texCoord * v_tilingFactor); break;
		case 7: textureColor *= texture(u_textures[7], v_texCoord * v_tilingFactor); break;
		case 8: textureColor *= texture(u_textures[8], v_texCoord * v_tilingFactor); break;
		case 9: textureColor *= texture(u_textures[9], v_texCoord * v_tilingFactor); break;
		case 10: textureColor *= texture(u_textures[10], v_texCoord * v_tilingFactor); break;
		case 11: textureColor *= texture(u_textures[11], v_texCoord * v_tilingFactor); break;
		case 12: textureColor *= texture(u_textures[12], v_texCoord * v_tilingFactor); break;
		case 13: textureColor *= texture(u_textures[13], v_texCoord * v_tilingFactor); break;
		case 14: textureColor *= texture(u_textures[14], v_texCoord * v_tilingFactor); break;
		case 15: textureColor *= texture(u_textures[15], v_texCoord * v_tilingFactor); break;
		case 16: textureColor *= texture(u_textures[16], v_texCoord * v_tilingFactor); break;
		case 17: textureColor *= texture(u_textures[17], v_texCoord * v_tilingFactor); break;
		case 18: textureColor *= texture(u_textures[18], v_texCoord * v_tilingFactor); break;
		case 19: textureColor *= texture(u_textures[19], v_texCoord * v_tilingFactor); break;
		case 20: textureColor *= texture(u_textures[20], v_texCoord * v_tilingFactor); break;
		case 21: textureColor *= texture(u_textures[21], v_texCoord * v_tilingFactor); break;
		case 22: textureColor *= texture(u_textures[22], v_texCoord * v_tilingFactor); break;
		case 23: textureColor *= texture(u_textures[23], v_texCoord * v_tilingFactor); break;
		case 24: textureColor *= texture(u_textures[24], v_texCoord * v_tilingFactor); break;
		case 25: textureColor *= texture(u_textures[25], v_texCoord * v_tilingFactor); break;
		case 26: textureColor *= texture(u_textures[26], v_texCoord * v_tilingFactor); break;
		case 27: textureColor *= texture(u_textures[27], v_texCoord * v_tilingFactor); break;
		case 28: textureColor *= texture(u_textures[28], v_texCoord * v_tilingFactor); break;
		case 29: textureColor *= texture(u_textures[29], v_texCoord * v_tilingFactor); break;
		case 30: textureColor *= texture(u_textures[30], v_texCoord * v_tilingFactor); break;
		case 31: textureColor *= texture(u_textures[31], v_texCoord * v_tilingFactor); break;
	}

	// discarding if transparent
	if (textureColor.w == 0) {
		discard;
	}

	// color attachment 1
	vec4 mycolor = vec4(textureColor.xyz * CalculateAllLights(), 1.0);
	
	if (u_isHDREnabled && !u_isPostProcessingEnabled) {
		float exposure = 0.5f; // temp
		vec3 toneMapped = vec3(1.0f) - exp(-mycolor.rgb * exposure);
		mycolor.rgb = pow(toneMapped, vec3(1.0f / 2.2f));
	}

	color = mycolor;
	
	// color attachment 2
	entityID = v_entityID;
	
	// color attachment 3
	float brightness = dot(mycolor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.15f)
		bloomColor = vec4(mycolor.rgb, 1.0);
	else
		bloomColor = vec4(0.0, 0.0, 0.0, 1.0);
}
