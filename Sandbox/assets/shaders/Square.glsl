#type vertex
#version 330 core
			
layout (location = 0) in vec3 a_position;

uniform mat4 u_viewProjection;
uniform mat4 u_modelMatrix;

void main()
{
	gl_Position = u_viewProjection * u_modelMatrix * vec4(a_position, 1.0);		
}

#type fragment
#version 330 core

layout (location = 0) out vec4 color;
uniform vec3 u_squareColor;
		
void main()
{
	color = vec4(u_squareColor, 1.0);
}