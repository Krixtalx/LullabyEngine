#pragma once
#include <glm/fwd.hpp>
namespace Lullaby {
	struct Transform {
		vec3 position;
		quat rotation;
		vec3 scale;

		void move(vec3 movement);
		void rotate(float angle, vec3 axis);
		void rescale(vec3 newScale);
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


}
