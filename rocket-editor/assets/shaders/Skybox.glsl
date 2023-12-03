#type vertex
#version 450

layout (location = 0) in vec3 a_position;

out vec3 texCoords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    texCoords = a_position;
    vec4 pos = u_projection * u_view * vec4(a_position, 1.0f);
    gl_Position = pos.xyww;
}   

#type fragment
#version 450

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, texCoords);
}