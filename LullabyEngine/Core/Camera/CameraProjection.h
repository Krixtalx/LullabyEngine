#pragma once

namespace Lullaby {
	class CameraProjection {
	protected:
		static std::vector<std::shared_ptr<CameraProjection>> _cameraProjection;

	public:
		enum Projection {
			PERSPECTIVE, ORTHOGRAPHIC
		};

		class CameraProperties {
		public:
			int								_cameraType;

			vec3							_eye, _lookAt, _up;
			float							_zNear, _zFar;
			float							_aspect;
			float							_fovY, _fovX;
			vec2							_bottomLeftCorner;
			uint16_t						_width, _height;
			vec3							_n, _u, _v;
			mat4							_viewMatrix, _projectionMatrix, _viewProjectionMatrix;

			float	computeAspect() const;
			void	computeAxes(vec3& n, vec3& u, vec3& v) const;
			vec2	computeBottomLeftCorner() const;
			float	computeFovY() const;

			void	computeProjectionMatrices(CameraProperties* camera);
			void	computeViewMatrices();
			void	computeViewMatrix();

			void	zoom(float speed);

			void	setViewMatrix(float* vMatrix);
			void	setProjectionMatrix(float* pMatrix);
			float	getDistToLookAt();
		};

	public:
		virtual mat4 buildProjectionMatrix(CameraProperties* camera) = 0;
		virtual void zoom(CameraProperties* camera, float speed) = 0;
	};

	class PerspectiveProjection : public CameraProjection {
	public:
		virtual mat4 buildProjectionMatrix(CameraProperties* camera) override;
		virtual void zoom(CameraProperties* camera, float speed) override;
	};

	class OrthographicProjection : public CameraProjection {
	public:
		virtual mat4 buildProjectionMatrix(CameraProperties* camera) override;
		virtual void zoom(CameraProperties* camera, float speed) override;
	};
}

