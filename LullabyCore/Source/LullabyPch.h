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

typedef glm::quat quat;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;