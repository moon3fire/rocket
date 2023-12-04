#type vertex
#version 450

layout (location = 0) in vec3 a_position;
layout (location = 3) in vec3 a_normal;
layout (location = 6) in int a_entityID;

out vec3 v_normal;
out vec3 v_position;
out flat int v_entityID;

uniform mat4 u_model;
uniform mat4 u_viewProjection;

void main()
{
    v_entityID = a_entityID;
    v_normal = mat3(transpose(inverse(u_model))) * a_normal; 
    v_position = vec3(u_model * vec4(a_position, 1.0));
    gl_Position = u_viewProjection * vec4(a_position, 1.0);
}   

#type fragment
#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 1) out int entityID;

in vec3 v_normal;
in vec3 v_position;
in flat int v_entityID;

uniform vec3 u_viewPosition;
uniform samplerCube skybox;

void main()
{
    float ratio = 1.00 / 1.52; // TODO: make uniform to be setable from the GUI, 1.52 is the glass
    vec3 I = normalize(v_position - u_viewPosition);
    vec3 R = refract(I, normalize(v_normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
    entityID = v_entityID;
}