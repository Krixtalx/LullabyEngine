#include "LullabyPch.h"
#include "AABB.h"

AABB::AABB(const vec3& min, const vec3& max) :
	_max(max), _min(min) {
}

AABB::AABB(const AABB& aabb) :
	_max(aabb._max), _min(aabb._min) {
}

AABB::~AABB() = default;

AABB& AABB::operator=(const AABB& aabb) = default;

AABB AABB::dot(const mat4& matrix) const {
	return { matrix * vec4(_min, 1.0f), matrix * vec4(_max, 1.0f) };
}

std::vector<vec3> AABB::corners() const {
	std::vector<vec3> corners(8);
	corners[0] = _min;
	corners[1] = { _min.x, _min.y, _max.z };
	corners[2] = { _min.x, _max.y, _min.z };
	corners[3] = { _min.x, _max.y, _max.z };
	corners[4] = { _max.x, _min.y, _min.z };
	corners[5] = { _max.x, _min.y, _max.z };
	corners[6] = { _max.x, _max.y, _min.z };
	corners[7] = _max;
	return corners;
}

void AABB::update(const AABB& aabb) {
	this->update(aabb.max());
	this->update(aabb.min());
}

void AABB::update(const vec3& point) {
	if (point.x < _min.x) { _min.x = point.x; }
	if (point.y < _min.y) { _min.y = point.y; }
	if (point.z < _min.z) { _min.z = point.z; }

	if (point.x > _max.x) { _max.x = point.x; }
	if (point.y > _max.y) { _max.y = point.y; }
	if (point.z > _max.z) { _max.z = point.z; }
}

std::ostream& operator<<(std::ostream& os, const AABB& aabb) {
	os << "Maximum corner: " << aabb.max().x << ", " << aabb.max().y << ", " << aabb.max().z << "\n";
	os << "Minimum corner: " << aabb.min().x << ", " << aabb.min().y << ", " << aabb.min().z << "\n";

	return os;
}
