#pragma once

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error at " << #x << ": " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0);

 namespace Utilities
 {
    struct LayoutTransitionInfo
    {
        VkImageLayout           oldLayout;
        VkImageLayout           newLayout;
        VkImage                 image;
        uint32_t                mipLevels;
        VkImageAspectFlags      aspectFlags;
        VkPipelineStageFlags    sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    };

    void ChangeLayout(VkCommandBuffer commandBuffer, const LayoutTransitionInfo& layoutTransitionInfo);
 }