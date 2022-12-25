#pragma once

#define MAX_POINT_LIGHTS_SIZE 10
#define MAX_DIRECTIONAL_LIGHTS_SIZE 10

#define VK_NO_PROTOTYPES
#include <volk.h>

#include "vk-bootstrap/src/VkBootstrap.h"

#include "vk_mem_alloc.h"

#include <vector>
#include <string>
#include <string_view>
#include <span>
#include <array>
#include <unordered_map>
#include <optional>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <memory>
#include <functional>
#include <exception>

#include <cmath>

#include <Common/Utilities.hpp>