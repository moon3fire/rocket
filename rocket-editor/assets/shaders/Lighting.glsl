//#type vertex
//#version 450
//
//layout (location = 0) in vec3 a_position;
//layout (location = 1) in vec2 a_textureCoord;
//layout (location = 2) in vec4 a_color;
//layout (location = 3) in vec3 a_normal;
//layout (location = 4) in float a_texIndex;
//layout (location = 5) in float a_tilingFactor;
//layout (location = 6) in int a_entityID;
//
//uniform mat4 u_viewProjection;
//
//out vec4 v_color;
//out vec2 v_textureCoord;
//out vec3 v_normal;
//out vec3 v_fragPos;
//
//void main()
//{
//	v_color = a_color;
//	v_textureCoord = a_textureCoord;
//	v_normal = normalize(a_normal);
//	v_fragPos = a_position;
//	gl_Position = u_viewProjection * vec4(a_position, 1.0);		
//}
//
//
//#type fragment
//#version 450
//
//#define MAX_DIRECTIONAL_LIGHTS 10
//
//layout (location = 0) out vec4 color;
//layout (location = 1) out int entityID;
//
//in vec4 v_color;
//in vec2 v_textureCoord;
//in vec3 v_normal;
//in vec3 v_fragPos;
//
//struct DirectionalLight {
//	vec3 direction;
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//};
//
//uniform int u_directionalLightCount;
//uniform float u_ambientStrenghts[MAX_DIRECTIONAL_LIGHTS];
//uniform DirectionalLight lights[MAX_DIRECTIONAL_LIGHTS];
//uniform vec3 u_viewPosition;
//
//void main()
//{
//	vec3 v = u_viewPosition;
//	int t = u_directionalLightCount;
//	float p = u_ambientStrenghts[0];
//	vec3 a = lights[0].ambient;
//	vec3 b = lights[0].direction;
//	vec3 c = lights[0].diffuse;
//	vec3 d = lights[0].specular;
//	color = vec4(0.0, 0.0, 0.0, 1.0);
//}
//