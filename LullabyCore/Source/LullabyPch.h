#pragma once
#define _USE_MATH_DEFINES
#define GLM_ENABLE_EXPERIMENTAL
#define IMGUI_DEFINE_MATH_OPERATORS

#include <vector>
#include <memory>
#include <fstream>
#include "fmt/core.h"
#include "fmt/color.h"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
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

// Rename
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;

typedef glm::vec3 vec3;
typedef glm::mat3 mat3;

typedef glm::vec2 vec2;
typedef glm::uvec3 uvec3;
typedef glm::uvec2 uvec2;

typedef glm::ivec3 ivec3;
typedef glm::ivec2 ivec2;