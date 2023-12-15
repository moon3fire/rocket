#type vertex

// nothing here yet :)

#type fragment

// forward shading file :)
// this is being included after the SPECIFIED shader code
// define these functions ABOVE the main to use them

#define MAX_POINT_LIGHTS 100

struct PointLight {
	vec3 position;
	vec3 color;
	vec3 ambient;
	float intensity;
	float radius;
};

uniform int u_pointLightCount;
uniform PointLight pointLights[MAX_POINT_LIGHTS];


vec3 gammaCorrect(vec3 color) {
    return pow(color, vec3(1.0 / 1.7));
}

vec3 CalculatePointLight(PointLight light, vec3 fragPos, vec3 normal) {
    vec3 lightDir = normalize(light.position - fragPos);

    float diffuseFactor = max(dot(normal, lightDir), 0.0);
	
	float linear = 0.7;
	float quadratic = 1.8;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * (distance * distance));
	float attenuationFactor = clamp(1.0 - distance / light.radius, 0.0, 1.0);

    return (light.ambient * diffuseFactor * attenuation) + (light.color * attenuation) * attenuationFactor * light.intensity;
}

// as this function does the all calculations, need to declare it where we want to use
vec3 CalculateAllLights() {
	vec3 result = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < u_pointLightCount; i++) {
		result += CalculatePointLight(pointLights[i], v_fragPos, v_normal);
	}

	return gammaCorrect(result);
}