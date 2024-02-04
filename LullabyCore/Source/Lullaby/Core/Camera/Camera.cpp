#include "LullabyPch.h"
#include "Camera.h"

#include "CameraProjection.h"


// Public methods

Lullaby::Camera::Camera() : Camera(1024, 576) {}

Lullaby::Camera::Camera(const uint16_t width, const uint16_t height) : _backupCamera(nullptr), _name("New camera") {
	this->_properties._cameraType = CameraProjection::PERSPECTIVE;

	this->_properties._eye = vec3(0.0f, 0.0f, 4.0f);
	this->_properties._lookAt = vec3(0.0f, 0.0f, 0.0f);
	//this->_properties._eye = vec3(0.0f, 3.0f, 10.0f);
	//this->_properties._lookAt = vec3(0.0f, 3.0f, 0.0f);
	this->_properties._up = vec3(0.0f, 1.0f, 0.0f);

	this->_properties._zNear = 0.1f;
	this->_properties._zFar = 1000.0f;

	this->_properties._width = width;
	this->_properties._height = height;
	this->_properties._aspect = this->_properties.computeAspect();

	this->_properties._bottomLeftCorner = vec2(-2.0f * this->_properties._aspect, -2.0f);
	this->_properties._fovX = 80.0f * glm::pi<float>() / 180.0f;
	this->_properties._fovY = this->_properties.computeFovY();

	this->_properties.computeAxes(this->_properties._n, this->_properties._u, this->_properties._v);
	this->_properties.computeViewMatrix();
	this->_properties.computeProjectionMatrices(&this->_properties);

	this->saveCamera();
}

Lullaby::Camera::Camera(const Lullaby::Camera& camera) : _backupCamera(nullptr) {
	this->copyCameraAttributes(&camera);
}

Lullaby::Camera::~Camera() {
	delete _backupCamera;
}

void Lullaby::Camera::reset() {
	this->copyCameraAttributes(_backupCamera);
}

void Lullaby::Camera::lookPosition(const vec3& position) {
	const vec3 movVec = position - this->_properties._lookAt;
	this->setLookAt(position);
	this->setPosition(this->_properties._eye + movVec);
}

//void Lullaby::Camera::lookModel(Model3D* model) {
//	const AABB aabb = model->getAABB();
//	vec3 pos = aabb.max();
//	pos.y += 20;
//	this->setPosition(pos);
//	this->setLookAt(aabb.center());
//}

//void Lullaby::Camera::trackModel(Model3D* model) {
//	trackedModel = model;
//}

//void Lullaby::Camera::untrackModel() {
//	trackedModel = nullptr;
//}
//
//Lullaby::Model3D* Lullaby::Camera::getTrackedModel() const {
//	return trackedModel;
//}

void Lullaby::Camera::saveCamera() {
	delete _backupCamera;

	_backupCamera = new Lullaby::Camera(*this);
}

void Lullaby::Camera::setBottomLeftCorner(const vec2& bottomLeft) {
	this->_properties._bottomLeftCorner = bottomLeft;
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::setCameraType(const Lullaby::CameraProjection::Projection projection) {
	this->_properties._cameraType = projection;
	this->_properties.computeViewMatrices();
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::setFovX(const float fovX) {
	this->_properties._fovX = fovX;
	this->_properties._fovY = this->_properties.computeFovY();
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::setFovY(const float fovY) {
	this->_properties._fovY = fovY;
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::setLookAt(const vec3& position) {
	this->_properties._lookAt = position;
	this->_properties.computeAxes(this->_properties._n, this->_properties._u, this->_properties._v);
	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::setPosition(const vec3& position) {
	this->_properties._eye = position;
	this->_properties.computeAxes(this->_properties._n, this->_properties._u, this->_properties._v);
	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::setRaspect(const uint16_t width, const uint16_t height) {
	this->_properties._width = width;
	this->_properties._height = height;
	this->_properties._aspect = this->_properties.computeAspect();
	this->_properties._bottomLeftCorner = vec2(this->_properties._bottomLeftCorner.y * this->_properties._aspect, this->_properties._bottomLeftCorner.y);
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::setUp(const vec3& up) {
	this->_properties._up = up;
	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::setZFar(const float zfar) {
	this->_properties._zFar = zfar;
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::setZNear(const float znear) {
	this->_properties._zNear = znear;
	this->_properties.computeProjectionMatrices(&this->_properties);
}

void Lullaby::Camera::updateMatrices() {
	this->_properties.computeViewMatrix();
	this->_properties.computeProjectionMatrices(&_properties);
}

// [Movements] 

void Lullaby::Camera::boom(const float speed) {
	const glm::mat4 translationMatrix = glm::translate(mat4(1.0f), this->_properties._v * speed);			// Translation in y axis

	this->_properties._eye = vec3(translationMatrix * vec4(this->_properties._eye, 1.0f));
	this->_properties._lookAt = vec3(translationMatrix * vec4(this->_properties._lookAt, 1.0f));

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::crane(const float speed) {
	boom(-speed);					// Implemented as another method to take advantage of nomenclature
}

void Lullaby::Camera::dolly(const float speed) {
	const mat4 translationMatrix = glm::translate(mat4(1.0f), -this->_properties._n * speed);			// Translation in z axis
	this->_properties._eye = vec3(translationMatrix * vec4(this->_properties._eye, 1.0f));
	this->_properties._lookAt = vec3(translationMatrix * vec4(this->_properties._lookAt, 1.0f));

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::orbitXZ(const float speed) {
	const mat4 rotationMatrix = glm::rotate(mat4(1.0f), speed, this->_properties._u);					// We will pass over the scene, x or z axis could be used


	this->_properties._eye = vec3(rotationMatrix * vec4(this->_properties._eye - this->_properties._lookAt, 1.0f)) + this->_properties._lookAt;
	this->_properties._u = vec3(rotationMatrix * vec4(this->_properties._u, 0.0f));
	this->_properties._v = vec3(rotationMatrix * vec4(this->_properties._v, 0.0f));
	this->_properties._n = vec3(rotationMatrix * vec4(this->_properties._n, 0.0f));
	this->_properties._up = glm::normalize(glm::cross(this->_properties._n, this->_properties._u));						// Free rotation => we can look down or up

	/*if (trackedModel)
		setLookAt(trackedModel->getAABB().center());*/

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::orbitY(const float speed) {
	const mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), speed, this->_properties._v);

	this->_properties._eye = vec3(rotationMatrix * vec4(this->_properties._eye - this->_properties._lookAt, 1.0f)) + this->_properties._lookAt;
	this->_properties._u = vec3(rotationMatrix * vec4(this->_properties._u, 0.0f));
	this->_properties._v = vec3(rotationMatrix * vec4(this->_properties._v, 0.0f));
	this->_properties._n = vec3(rotationMatrix * vec4(this->_properties._n, 0.0f));
	this->_properties._up = glm::normalize(glm::cross(this->_properties._n, this->_properties._u));								// This movement doesn't change UP, but it could occur as a result of previous operations

	/*if (trackedModel)
		setLookAt(trackedModel->getAABB().center());*/

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::pan(const float speed) {
	const mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), speed, this->_properties._v);

	// Up vector can change, not in the original position tho. Example: orbit XZ (rotated camera) + pan
	this->_properties._u = vec3(rotationMatrix * vec4(this->_properties._u, 0.0f));
	this->_properties._v = vec3(rotationMatrix * vec4(this->_properties._v, 0.0f));
	this->_properties._n = vec3(rotationMatrix * vec4(this->_properties._n, 0.0f));
	this->_properties._up = glm::normalize(glm::cross(this->_properties._n, this->_properties._u));
	this->_properties._lookAt = vec3(rotationMatrix * vec4(this->_properties._lookAt - this->_properties._eye, 1.0f)) + this->_properties._eye;

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::tilt(const float speed) {
	const mat4 rotationMatrix = glm::rotate(mat4(1.0f), speed, this->_properties._u);

	const auto n = glm::vec3(rotationMatrix * glm::vec4(this->_properties._n, 0.0f));
	const float alpha = glm::acos(glm::dot(n, glm::vec3(0.0f, 1.0f, 0.0f)));

	if (alpha < speed || alpha >(glm::pi<float>() - speed)) {
		return;
	}

	this->_properties._v = glm::vec3(rotationMatrix * glm::vec4(this->_properties._v, 0.0f));
	this->_properties._n = n;
	this->_properties._up = glm::normalize(glm::cross(this->_properties._n, this->_properties._u));											// It could change because of the rotation
	this->_properties._lookAt = glm::vec3(rotationMatrix * glm::vec4(this->_properties._lookAt - this->_properties._eye, 1.0f)) + this->_properties._eye;

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::truck(const float speed) {
	const mat4 translationMatrix = glm::translate(mat4(1.0f), this->_properties._u * speed);				// Translation in x axis

	this->_properties._eye = vec3(translationMatrix * vec4(this->_properties._eye, 1.0f));
	this->_properties._lookAt = vec3(translationMatrix * vec4(this->_properties._lookAt, 1.0f));

	this->_properties.computeViewMatrices();
}

void Lullaby::Camera::zoom(const float speed) {
	this->_properties.zoom(speed);
}

void Lullaby::Camera::rotate(const float speed) {
	const auto rotationMatrix = glm::rotate(mat4(1.0f), speed, _properties._n);
	_properties._v = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(this->_properties._v, 0.0f)));
	_properties._u = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(this->_properties._u, 0.0f)));
	this->_properties._up = glm::normalize(glm::cross(this->_properties._n, this->_properties._u));
	this->_properties.computeViewMatrices();
}

/// [Private methods]

void Lullaby::Camera::copyCameraAttributes(const Lullaby::Camera* camera) {
	this->_properties = camera->_properties;

	if (camera->_backupCamera) {
		delete this->_backupCamera;
		this->_backupCamera = new Lullaby::Camera(*camera->_backupCamera);
	}
}

void Lullaby::Camera::setBackupCamera() {
	this->_backupCamera = new Lullaby::Camera();

	delete this->_backupCamera->_backupCamera;
	this->_backupCamera->_backupCamera = nullptr;

	this->_backupCamera->_properties = this->_properties;
}
