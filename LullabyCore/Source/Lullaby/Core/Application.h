#pragma once
#include "Renderer.h"

namespace Lullaby {
	class Application final {
		std::string _title;
		ivec2 _resolution;

		Renderer* _renderer;
		GLFWwindow* _window;
		bool _isHeadless = false;
		const bool _disableTitleBar = false;

		void releaseResources();

	public:
		Application();
		~Application();
		void init(const std::string& title = "Lullaby Engine", int width = 1280, int height = 720, bool headless = false);
		void renderLoop() const;
	};
}