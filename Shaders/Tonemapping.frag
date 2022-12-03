#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform ExposurePushConstant
{
    float exposure;
} exposureObject;

layout(set = 0, binding = 0) uniform sampler2D hdrSampler;

void main()
{
    vec3 hdrColor = texture(hdrSampler, fragTexCoord).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposureObject.exposure);

    outColor = vec4(mapped, 1.0);
}