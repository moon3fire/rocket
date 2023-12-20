#type vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
    v_texCoord = a_texCoord;
    gl_Position = vec4(a_position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 fragColor;

in vec2 v_texCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

uniform bool u_isHDREnabled;

void main()
{
    const float gamma = 2.2;

    vec3 hdrColor = texture(scene, v_texCoord).rgb;      
    vec3 bloomColor = texture(bloomBlur, v_texCoord).rgb;
    if(bloom)
        hdrColor += bloomColor; // additive blending
    
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    if (u_isHDREnabled) {
		vec3 toneMapped = vec3(1.0f) - exp(-result.rgb * exposure);
		result.rgb = pow(toneMapped, vec3(1.0f / gamma));
	}
	
    // also gamma correct while we're at it       
    fragColor = vec4(result, 1.0);
    fragColor = vec4(texture(scene, v_texCoord).rgb, 1.0);
    //fragColor = vec4(hdrColor, 1.0);
}
