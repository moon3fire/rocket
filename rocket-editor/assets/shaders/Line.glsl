#type vertex
#version 450 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in int a_entityID;

uniform mat4 u_viewProjection;

out vec4 v_color;
out flat int v_entityID;

void main() {
	v_color = a_color;
	v_entityID = a_entityID;
	gl_Position = u_viewProjection * vec4(a_position, 1.0);
}

#type fragment
#version 450 core

layout (location = 0) out vec4 color;
layout (location = 1) out int entityID;

in vec4 v_color;
in flat int v_entityID;

void main() {
	color = v_color;
	entityID = v_entityID;
}