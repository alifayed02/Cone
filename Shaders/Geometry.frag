#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in mat3 fragTBN;

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gPosition;
layout(location = 2) out vec4 gNormal;

layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;
layout(set = 1, binding = 1) uniform sampler2D normalTexSampler;
layout(set = 1, binding = 2) uniform sampler2D metallicRoughnessTexSampler;

layout(push_constant) uniform MaterialObject
{
    layout(offset = 64)
    vec4    albedoColor;
    float   metallicFactor;
    float   roughnessFactor;
} matObject;

vec4 TangentToWorld()
{
    vec3 normal = texture(normalTexSampler, fragTexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 result = normalize(fragTBN * normal);

    return vec4(result, 1.0);
}

void main()
{
    // Metallic in A channel
    gAlbedo = vec4(texture(albedoTexSampler, fragTexCoord).rgb, texture(metallicRoughnessTexSampler, fragTexCoord).b * matObject.metallicFactor);

    // Roughness in A Channel
    gPosition = vec4(fragPos, texture(metallicRoughnessTexSampler, fragTexCoord).g * matObject.roughnessFactor);

    gNormal = normalize(TangentToWorld());
}