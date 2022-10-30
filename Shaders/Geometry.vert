#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform CameraBufferObject
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
    fragColor = inColor;
}