#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform CameraBufferObject
{
    mat4 view;
    mat4 proj;
    mat4 projView;
} cbo;

layout( push_constant ) uniform Constants
{
    mat4 model;
} mbo;

void main() {
    gl_Position = cbo.projView * mbo.model * vec4(inPosition, 1.0);
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
}