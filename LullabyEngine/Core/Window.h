#pragma once
#include "Renderer.h"

namespace Lullaby {
	class Window final : public Singleton<Window> {
		friend class Singleton;
		std::string _title;
		ivec2 _resolution;

		Renderer* _renderer;
		GLFWwindow* _window;
		bool _isHeadless = false;
		const bool _disableTitleBar = true;

		Window();
		void releaseResources();

	public:
		~Window();
		void init(const std::string& title = "Lullaby Engine", int width = 1280, int height = 720, const bool headless = false);
		void renderLoop() const;
	};
}