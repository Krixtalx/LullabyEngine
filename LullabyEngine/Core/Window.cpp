#include "stdafx.h"
#include "Window.h"

Lullaby::Window::Window() : _window(nullptr) {
	_renderer = VKRenderer::getInstance();
}

void Lullaby::Window::releaseResources() {
	glfwDestroyWindow(_window); // - Cerramos y destruimos la ventana de la aplicación.
	_window = nullptr;
	glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
}

Lullaby::Window::~Window() {
	releaseResources();
}

void Lullaby::Window::init(const std::string& title, int width, int height, const bool headless) {
	_isHeadless = headless;
	if (!headless) {
		// - Inicializa GLFW. Es un proceso que sólo debe realizarse una vez en la aplicación
		if (glfwInit() != GLFW_TRUE) {
			throw std::runtime_error("Failed to initialize GLFW!");
		}
		glfwWindowHint(GLFW_SAMPLES, 4); // - Activa antialiasing con 4 muestras.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		int count = 0;
		const auto monitors = glfwGetMonitors(&count);
		glfwGetMonitorWorkarea(monitors[0], nullptr, nullptr, &_resolution.x, &_resolution.y);

		// - Tamaño, título de la ventana, en ventana y no en pantalla completa, sin compartir recursos con otras ventanas.
		_title = title;
		_window = glfwCreateWindow(_resolution.x, _resolution.y, title.c_str(), nullptr, nullptr);
		// - Comprobamos si la creación de la ventana ha tenido éxito.
		if (_window == nullptr) {
			glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
			throw std::runtime_error("Failed to open GLFW window!");
		}
	}
	_renderer->initRenderer(_window);
	if (!headless)
		_renderer->initSwapchain(_resolution);
	_renderer->initCommands();
	_renderer->initDefaultRenderpass();
	_renderer->initFramebuffers();
	_renderer->initSyncStructures();
	_renderer->initPipelines();
	_renderer->sampleTriangle();
	std::cout << "Lullaby renderer initialized" << std::endl;
}

void Lullaby::Window::renderLoop() const {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		_renderer->render();
	}
}
