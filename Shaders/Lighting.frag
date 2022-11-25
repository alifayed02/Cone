#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D albedoSampler;
layout(set = 0, binding = 1) uniform sampler2D positionSampler;
layout(set = 0, binding = 2) uniform sampler2D normalSampler;

void main()
{
    outColor = texture(normalSampler, fragTexCoord);
}