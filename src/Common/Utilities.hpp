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
