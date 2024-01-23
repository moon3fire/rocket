#type vertex
#version 450

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}

#type fragment
#version 450

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D originalTexture;
uniform sampler2D blurredTexture;

uniform float u_exposure;

vec3 ACESFilm(vec3 rgbColor)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((rgbColor*(a * rgbColor + b))/(rgbColor * (c * rgbColor + d) + e), 0.0, 1.0);
}

void main()
{
    vec3 mycolor = vec3(0.0);
    vec4 originalColor = texture(originalTexture, TexCoord);
    vec4 blurredColor = texture(blurredTexture, TexCoord);

    FragColor = vec4((originalColor.rgb + blurredColor.rgb) * u_exposure, 1.0);
    FragColor.rgb = ACESFilm(FragColor.rgb);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));
}