#pragma once
#define _USE_MATH_DEFINES
#define GLM_ENABLE_EXPERIMENTAL
#define IMGUI_DEFINE_MATH_OPERATORS

#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "fmt/core.h"
#include "fmt/color.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_NO_SPACESHIP_OPERATOR
#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/ext/vector_relational.hpp>   
#include <glm/gtc/epsilon.hpp>     
#include <glm/gtx/euler_angles.hpp>

// Gui
//#include "imgui.h"
//#include "imgui_impl_glfw.h"
//#include "imgui_stdlib.h"
//#include "imgui_internal.h"

#include "Lullaby/Typedefs.h"