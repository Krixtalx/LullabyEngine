#include "stdafx.h"
#include "Window.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


WNDPROC original_proc;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_NCCALCSIZE:
	{
		// Remove the window's standard sizing border
		if (wParam == TRUE && lParam != NULL) {
			NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
			pParams->rgrc[0].top += 1;
			pParams->rgrc[0].right -= 1;
			pParams->rgrc[0].bottom -= 1;
			pParams->rgrc[0].left += 1;
		}
		return 0;
	}
	case WM_NCPAINT:
	{
		// Prevent the non-client area from being painted
		return 0;
	}
	case WM_NCHITTEST:
	{
		// Expand the hit test area for resizing
		const int borderWidth = 8; // Adjust this value to control the hit test area size

		POINTS mousePos = MAKEPOINTS(lParam);
		POINT clientMousePos = { mousePos.x, mousePos.y };
		ScreenToClient(hWnd, &clientMousePos);

		RECT windowRect;
		GetClientRect(hWnd, &windowRect);

		if (clientMousePos.y >= windowRect.bottom - borderWidth) {
			if (clientMousePos.x <= borderWidth)
				return HTBOTTOMLEFT;
			if (clientMousePos.x >= windowRect.right - borderWidth)
				return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}
		if (clientMousePos.y <= borderWidth) {
			if (clientMousePos.x <= borderWidth)
				return HTTOPLEFT;
			if (clientMousePos.x >= windowRect.right - borderWidth)
				return HTTOPRIGHT;
			return HTTOP;
		}
		if (clientMousePos.x <= borderWidth) {
			return HTLEFT;
		}
		if (clientMousePos.x >= windowRect.right - borderWidth) {
			return HTRIGHT;
		}

		break;
	}
	case WM_NCACTIVATE:
	{
		// Prevent non-client area from being redrawn during window activation
		return TRUE;
	}
	}

	return CallWindowProc(original_proc, hWnd, uMsg, wParam, lParam);
}
void disableTitlebar(GLFWwindow* window) {
	HWND hWnd = glfwGetWin32Window(window);

	LONG_PTR lStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
	lStyle |= WS_THICKFRAME;
	lStyle &= ~WS_CAPTION;
	SetWindowLongPtr(hWnd, GWL_STYLE, lStyle);

	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	original_proc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
	SetWindowPos(hWnd, nullptr, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOMOVE);
}

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
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

		int count = 0;
		const auto monitors = glfwGetMonitors(&count);
		glfwGetMonitorWorkarea(monitors[0], nullptr, nullptr, &_resolution.x, &_resolution.y);
		_resolution.x = width;
		_resolution.y = height;

		// - Tamaño, título de la ventana, en ventana y no en pantalla completa, sin compartir recursos con otras ventanas.
		_title = title;
		_window = glfwCreateWindow(_resolution.x, _resolution.y, title.c_str(), nullptr, nullptr);

		// - Comprobamos si la creación de la ventana ha tenido éxito.
		if (_window == nullptr) {
			glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
			throw std::runtime_error("Failed to open GLFW window!");
		}
		if (_disableTitleBar)
			disableTitlebar(_window);
		glfwSetWindowSizeCallback(_window, VKRenderer::resizeCallback);
	}
	_renderer->initRenderer(_window);
	if (!headless)
		_renderer->initSwapchain(_resolution);
	_renderer->initCommands();
	_renderer->initDefaultRenderpass();
	_renderer->initFramebuffers();
	_renderer->initSyncStructures();
	_renderer->initPipelines();
	_renderer->sampleModel();
	fmt::print(fg(fmt::color::aquamarine), "Lullaby renderer initialized\n");
}

void Lullaby::Window::renderLoop() const {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		_renderer->render();
	}
}
