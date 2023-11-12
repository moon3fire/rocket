#type vertex
#version 330 core
			
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;

uniform mat4 u_viewProjection;
uniform mat4 u_modelMatrix;

out vec4 frag_color;

void main()
{
	frag_color = a_color;
	gl_Position = u_viewProjection * u_modelMatrix * vec4(a_position, 1.0);		
}

#type fragment
#version 330 core

in vec4 frag_color;			
layout (location = 0) out vec4 color;	
		
void main()
{
	color = frag_color;
}