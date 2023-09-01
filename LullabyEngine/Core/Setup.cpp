#include "stdafx.h"
#include "Setup.h"

Lullaby::Setup::Setup() : _window(nullptr) {
	_renderer = VKRenderer::getInstance();
}

void Lullaby::Setup::releaseResources() {
	glfwDestroyWindow(_window); // - Cerramos y destruimos la ventana de la aplicación.
	_window = nullptr;
	glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
}

Lullaby::Setup::~Setup() {
	releaseResources();
}

void Lullaby::Setup::init(const std::string& title, int width, int height, const bool headless) {
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
		glfwGetMonitorWorkarea(monitors[0], nullptr, nullptr, &width, &height);

		// - Tamaño, título de la ventana, en ventana y no en pantalla completa, sin compartir recursos con otras ventanas.
		_title = title;
		_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		// - Comprobamos si la creación de la ventana ha tenido éxito.
		if (_window == nullptr) {
			glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
			throw std::runtime_error("Failed to open GLFW window!");
		}
		_resolution = { width, height };
	}
	_renderer->initRenderer(_window);
	if (!headless)
		_renderer->initSwapchain(_resolution);
	_renderer->initCommands();
	_renderer->initDefaultRenderpass();
	_renderer->initFramebuffers();

}

void Lullaby::Setup::renderLoop() const {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
	}
}
