#type vertex
#version 450
			
layout (location = 0) in vec3 a_worldPosition;
layout (location = 1) in vec3 a_localPosition;
layout (location = 2) in vec4 a_color;
layout (location = 3) in float a_thickness;
layout (location = 4) in float a_fade;
layout (location = 5) in int a_entityID;

uniform mat4 u_viewProjection;

out vec3 v_localPosition;
out vec4 v_color;
out float v_thickness;
out float v_fade;
out flat int v_entityID;

void main() {
	v_localPosition = a_localPosition;
	v_color = a_color;
	v_thickness = a_thickness;
	v_fade = a_fade;

	v_entityID = a_entityID;

	gl_Position = u_viewProjection * vec4(a_worldPosition, 1.0);
}

#type fragment
#version 450

layout (location = 0) out vec4 color;
layout (location = 1) out int entityID;

in vec3 v_localPosition;
in vec4 v_color;
in float v_thickness;
in float v_fade;
in flat int v_entityID;

uniform vec3 u_viewPosition;

void main() {
	float distance = 1.0 - length(v_localPosition);
	float circle = smoothstep(0.0, v_fade, distance);
	circle *= smoothstep(v_thickness + v_fade, v_thickness, distance);

	if (circle == 0.0)
		discard;

	color = v_color;
	color.a *= circle;

	entityID = v_entityID;
}
