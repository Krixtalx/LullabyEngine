#include "stdafx.h"
#include "VKRenderer.h"

namespace Lullaby {
	class Setup : public Singleton<Setup> {
		friend class Singleton;
		std::string _title;
		ivec2 _resolution;

		VKRenderer* _renderer;
		GLFWwindow* _window;
		bool _isHeadless = false;

		Setup();
		void releaseResources();

	public:
		virtual ~Setup();
		void init(const std::string& title = "Lullaby Engine", int width = 1920, int height = 1080, const bool headless = false);
		void renderLoop() const;
	};
}