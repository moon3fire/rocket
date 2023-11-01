#type vertex
#version 330 core
			
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_textureCoord;
layout (location = 2) in vec4 a_color;

uniform mat4 u_viewProjection;
	
out vec4 v_color;
out vec2 v_textureCoord;

void main()
{
	v_color = a_color;
	v_textureCoord = a_textureCoord;
	gl_Position = u_viewProjection * vec4(a_position, 1.0);		
}

#type fragment
#version 330 core

layout (location = 0) out vec4 color;
		
in vec4 v_color;
in vec2 v_textureCoord;

uniform sampler2D u_texture;
uniform vec4 u_color;
uniform float u_textureCoordMultiplier;

void main()
{
	//color = texture(u_texture, v_textureCoord * u_textureCoordMultiplier) * u_color;
	color = v_color;
}