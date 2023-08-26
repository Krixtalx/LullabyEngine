#include "stdafx.h"
#include "CameraProjection.h"

// Static

std::vector<std::shared_ptr<Lullaby::CameraProjection>> Lullaby::CameraProjection::_cameraProjection{
	std::shared_ptr<Lullaby::CameraProjection>(new Lullaby::PerspectiveProjection()),
	std::shared_ptr<Lullaby::CameraProjection>(new Lullaby::OrthographicProjection())
};

float Lullaby::CameraProjection::CameraProperties::computeAspect() const {
	if (_height == 0)
		return 0;
	return static_cast<float>(_width) / static_cast<float>(_height);
}

void Lullaby::CameraProjection::CameraProperties::computeAxes(vec3& n, vec3& u, vec3& v) const {
	n = glm::normalize(_eye - _lookAt);							// z axis

	if (glm::all(glm::epsilonEqual(n, -_up, glm::epsilon<float>()))) {		// x axis: UP x n is 0 as both vectors are parallel. Since up and n are normalized we can check if they are equal (with epsilon checkup)
		u = glm::normalize(glm::cross(vec3(0.0f, 0.0f, -1.0f), n));
	} else if (glm::all(glm::epsilonEqual(n, _up, glm::epsilon<float>()))) {
		u = glm::normalize(glm::cross(vec3(0.0f, 0.0f, 1.0f), n));
	} else {
		u = glm::normalize(glm::cross(_up, n));
	}
	v = glm::normalize(glm::cross(n, u));					// y axis
}

vec2 Lullaby::CameraProjection::CameraProperties::computeBottomLeftCorner() const {
	const float halfWidth = _width / 2.0f;
	const float halfHeight = _height / 2.0f;

	return vec2(-halfWidth, -halfHeight);
}

float Lullaby::CameraProjection::CameraProperties::computeFovY() const {
	return 2.0f * glm::atan(glm::tan(_fovX / 2.0f) / _aspect);
}

void Lullaby::CameraProjection::CameraProperties::computeProjectionMatrices(CameraProperties* camera) {
	_projectionMatrix = _cameraProjection[_cameraType]->buildProjectionMatrix(camera);
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}

void Lullaby::CameraProjection::CameraProperties::computeViewMatrices() {
	this->computeViewMatrix();
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}

void Lullaby::CameraProjection::CameraProperties::computeViewMatrix() {
	_viewMatrix = glm::lookAt(_eye, _lookAt, _up);
}

void Lullaby::CameraProjection::CameraProperties::zoom(float speed) {
	CameraProjection::_cameraProjection[this->_cameraType]->zoom(this, speed);
}

void Lullaby::CameraProjection::CameraProperties::setViewMatrix(float* vMatrix) {
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			_viewMatrix[r][c] = vMatrix[c + (4 * r)];
		}
	}
	_eye = vec3(vMatrix[12], vMatrix[13], vMatrix[14]);
	computeViewMatrices();
}

void Lullaby::CameraProjection::CameraProperties::setProjectionMatrix(float* pMatrix) {
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			_projectionMatrix[r][c] = pMatrix[c + (4 * r)];
		}
	}
}

float Lullaby::CameraProjection::CameraProperties::getDistToLookAt() {
	return glm::distance(_eye, _lookAt);
}

// Projection

mat4 Lullaby::PerspectiveProjection::buildProjectionMatrix(CameraProperties* camera) {
	return glm::perspective(camera->_fovY, camera->_aspect, camera->_zNear, camera->_zFar);
}

void Lullaby::PerspectiveProjection::zoom(CameraProperties* camera, const float speed) {
	const float angle = camera->_fovY - speed;
	if (angle < glm::pi<float>() && angle > 0.0f) {
		camera->_fovY = angle;
		camera->computeProjectionMatrices(camera);
	}
}

mat4 Lullaby::OrthographicProjection::buildProjectionMatrix(CameraProperties* camera) {
	const glm::vec2 bottomLeftCorner = camera->_bottomLeftCorner;
	return glm::ortho(bottomLeftCorner.x, -bottomLeftCorner.x, bottomLeftCorner.y, -bottomLeftCorner.y, camera->_zNear, camera->_zFar);
}

void Lullaby::OrthographicProjection::zoom(CameraProperties* camera, const float speed) {
	const float units = -speed;
	const float raspect = camera->_aspect;
	glm::vec2 bottomLeftCorner = camera->_bottomLeftCorner;

	if (bottomLeftCorner.x - units >= 0 || bottomLeftCorner.y - units >= 0) return;

	bottomLeftCorner -= vec2(units * raspect, units);
	camera->_bottomLeftCorner = bottomLeftCorner;

	camera->computeProjectionMatrices(camera);
}
