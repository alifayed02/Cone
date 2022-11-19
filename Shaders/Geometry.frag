#version 450

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 albedoAttachment;
layout(location = 1) out vec4 positionAttachment;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() {
    albedoAttachment    = texture(texSampler, fragTexCoord);
    positionAttachment  = vec4(fragPos, 1.0);
}