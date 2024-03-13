#pragma once

#include <glm/gtx/quaternion.hpp>
namespace Lullaby {
	struct Transform {
		vec3 position = { 0,0,0 };
		quat rotation = {};
		vec3 scale = { 1,1,1 };

		void move(vec3 movement);
		void rotate(float angle, vec3 axis);
		void rescale(vec3 newScale);
		mat4x4 getMatrix() const;
	};

	inline void Transform::move(const vec3 movement) {
		position += movement;
	}

	inline void Transform::rotate(const float angle, const vec3 axis) {
		rotation = glm::rotate(rotation, angle, axis);
	}

	inline void Transform::rescale(const vec3 newScale) {
		scale = newScale;
	}

	inline mat4x4 Transform::getMatrix() const {
		return glm::translate(mat4x4(1.0f), position) * glm::toMat4(rotation) * glm::scale(mat4x4(1.0f), scale);
	}
}
