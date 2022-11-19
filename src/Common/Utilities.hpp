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
     void ChangeLayout(VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage image, VkImageAspectFlags aspectFlags, VkPipelineStageFlags sourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
 }