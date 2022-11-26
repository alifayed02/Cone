#version 450

#define POINT_LIGHTS_SIZE 10

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D albedoSampler;
layout(set = 0, binding = 1) uniform sampler2D positionSampler;
layout(set = 0, binding = 2) uniform sampler2D normalSampler;

struct PointLight
{
    vec3 position;
    vec3 color;
};

//layout(set = 0, binding = 3) uniform LightBuffer
//{
//    PointLight lights[POINT_LIGHTS_SIZE];
//} lbo;

vec4 CalculateAmbient();
vec4 CalculateDiffuse();

void main()
{
    // Pos = (1.47991, 2.8, -1.58609)
    vec3 lightPos = vec3(1.47991, 2.8, -1.58609);

    float distance    = length(lightPos - texture(positionSampler, fragTexCoord).xyz);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    vec4 baseColor  = texture(albedoSampler, fragTexCoord);
    vec4 ambient    = CalculateAmbient() * attenuation;
    vec4 diffuse    = CalculateDiffuse() * attenuation;

    outColor = (ambient + diffuse) * baseColor;
}

vec4 CalculateAmbient()
{
    float ambientStrength = 0.1;

    return vec4(ambientStrength * vec3(1.0, 1.0, 1.0), 1.0);
}

vec4 CalculateDiffuse()
{
    vec3 lightPos = vec3(1.47991, 2.8, -1.58609);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    vec3 norm = normalize(texture(normalSampler, fragTexCoord)).xyz;
    vec3 lightDir = normalize(lightPos - texture(positionSampler, fragTexCoord).xyz);
    float diff = max(dot(norm, lightDir), 0.0);

    return vec4(diff * lightColor, 1.0);
}