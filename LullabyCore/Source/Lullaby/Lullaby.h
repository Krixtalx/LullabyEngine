#pragma once
#include "glm/glm.hpp"

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


#include "Lullaby/Core/Application.h"
#include "Lullaby/Core/Renderer.h"
#include "Lullaby/Core/Shaders/PipelineBuilder.h"
#include "Lullaby/Core/DataContainers/Mesh.h"
#include <cstdint>


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;