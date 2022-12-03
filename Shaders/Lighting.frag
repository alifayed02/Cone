#version 450

#define MAX_POINT_LIGHTS_SIZE 10

struct PointLight
{
    vec3    position;
    vec3    color;
    float   radius;
};

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D albedoSampler;
layout(set = 0, binding = 1) uniform sampler2D positionSampler;
layout(set = 0, binding = 2) uniform sampler2D normalSampler;

layout(set = 0, binding = 3) uniform LightBuffer
{
    PointLight  lights[MAX_POINT_LIGHTS_SIZE];
    int         numPointLights;
    vec3        viewPos;
    mat4        viewMatrix;
} lbo;

vec4 CalculatePointLight(PointLight light);

void main()
{
    vec4 result = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    for(int i = 0; i < lbo.numPointLights; i++)
    {
        result += CalculatePointLight(lbo.lights[i]);
    }

    outColor = result;
}

vec4 CalculatePointLight(PointLight light)
{
    vec3 fragPos = texture(positionSampler, fragTexCoord).xyz;
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 normal = normalize(texture(normalSampler, fragTexCoord)).xyz;

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    // Combine results
    vec3 ambient = vec3(0.01f, 0.01f, 0.01f) * vec3(texture(albedoSampler, fragTexCoord));
    vec3 diffuse = light.color * diff * vec3(texture(albedoSampler, fragTexCoord));

    ambient *= attenuation;
    diffuse *= attenuation;

    return vec4((ambient + diffuse), 1.0f);
}