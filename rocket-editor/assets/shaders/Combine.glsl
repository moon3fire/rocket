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

void main()
{
    vec3 mycolor = vec3(0.0);
    vec4 originalColor = texture(originalTexture, TexCoord);
    vec4 blurredColor = texture(blurredTexture, TexCoord);

    FragColor = originalColor + blurredColor;
    //  vec3 toneMapped = vec3(1.0) - exp(-mycolor * 0.8);

   // FragColor.rgb = pow(toneMapped, vec3(1.0 / 2.2));
}