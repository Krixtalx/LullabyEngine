#pragma once

#include "CameraProjection.h"
//#include "Core/Model3D.h"

namespace Lullaby {
	class Camera {
		friend class GUI;
		friend class Scene;

	protected:
		Camera* _backupCamera = nullptr;
		CameraProjection::CameraProperties	_properties;

		std::string _name;
		//Model3D* trackedModel;

	protected:
		void copyCameraAttributes(const Camera* camera);
		void setBackupCamera();

	public:
		Camera();
		Camera(uint16_t width, uint16_t height);
		Camera(const Camera& camera);
		virtual ~Camera();
		void reset();
		void lookPosition(const vec3& position);
		/*void lookModel(Model3D* model);
		void trackModel(Model3D* model);*/
		void untrackModel();


		Camera& operator=(const Camera& camera) = delete;

		mat4 getViewMatrix() const { return _properties._viewMatrix; }
		mat4 getProjectionMatrix() const { return _properties._projectionMatrix; }
		mat4 getViewProjectionMatrix() const { return _properties._viewProjectionMatrix; }
		vec3 getCameraPosition() const { return _properties._eye; }
		//Model3D* getTrackedModel() const;

		void saveCamera();
		void setBottomLeftCorner(const vec2& bottomLeft);
		void setCameraType(CameraProjection::Projection projection);
		void setFovX(float fovX);
		void setFovY(float fovY);
		void setLookAt(const vec3& position);
		void setPosition(const vec3& position);
		void setRaspect(uint16_t width, uint16_t height);
		void setUp(const vec3& up);
		void setZFar(float zfar);
		void setZNear(float znear);
		void updateMatrices();

		// Movements

		void boom(float speed);
		void crane(float speed);
		void dolly(float speed);
		void orbitXZ(float speed);
		void orbitY(float speed);
		void pan(float speed);
		void tilt(float speed);
		void truck(float speed);
		void zoom(float speed);
		void rotate(float speed);
	};
}

