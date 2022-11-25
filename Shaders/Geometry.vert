#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out mat3 fragTBN;

layout(set = 0, binding = 0) uniform CameraBufferObject
{
    mat4 view;
    mat4 proj;
    mat4 projView;
} cbo;

layout(push_constant) uniform PerModel
{
    mat4 model;
} mbo;

void main()
{
    gl_Position = cbo.projView * mbo.model * vec4(inPosition, 1.0);
    fragPos     = vec3(mbo.model * vec4(inPosition, 1.0));

    fragNormal = normalize(inNormal);

    // Normal Matrix
    mat3 normalMatrix = mat3(mbo.model);

    // Transform vertex normals and tangents to model space
    vec3 normal = normalize(normalMatrix * inNormal);
    vec3 tangent = normalize(normalMatrix * inTangent.xyz);

    // Gram-Schmidt process to reorthoganlize vectors
    tangent = normalize(tangent - dot(tangent, normal) * normal);

    // Calculate bitangent vector
    vec3 bitangent = cross(normal, tangent) * inTangent.w;

    // Create TBN matrix to change normal map tangent space to model space
    fragTBN = mat3(tangent, bitangent, normal);

    fragTexCoord = inTexCoord;
}