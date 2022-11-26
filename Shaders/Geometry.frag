#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in mat3 fragTBN;

layout(location = 0) out vec4 albedoAttachment;
layout(location = 1) out vec4 positionAttachment;
layout(location = 2) out vec4 normalAttachment;

layout(set = 1, binding = 0) uniform sampler2D albedoTexSampler;
layout(set = 1, binding = 1) uniform sampler2D normalTexSampler;

layout(push_constant) uniform MaterialObject
{
    layout(offset = 64)
    vec4 albedoColor;
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
    albedoAttachment    = texture(albedoTexSampler, fragTexCoord) * matObject.albedoColor;
    positionAttachment  = vec4(fragPos, 1.0);
    normalAttachment    = normalize(TangentToWorld());
}