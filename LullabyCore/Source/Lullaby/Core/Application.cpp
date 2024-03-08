#include "LullabyPch.h"
#include "Application.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Lullaby/IO/ObjLoader.h"

WNDPROC original_proc;

LRESULT CALLBACK WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, LPARAM lParam) {
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
		constexpr int borderWidth = 8; // Adjust this value to control the hit test area size

		const POINTS mousePos = MAKEPOINTS(lParam);
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
		if (clientMousePos.y <= borderWidth + 30)
			return HTCAPTION;
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
	const int width = windowRect.right - windowRect.left;
	const int height = windowRect.bottom - windowRect.top;

	original_proc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));
	SetWindowPos(hWnd, nullptr, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOMOVE);
}

Lullaby::Application::Application() : _resolution(0, 0), _window(nullptr) {
	_renderer = Renderer::getInstance();
}

void Lullaby::Application::releaseResources() {
	glfwDestroyWindow(_window); // - Cerramos y destruimos la ventana de la aplicación.
	_window = nullptr;
	glfwTerminate(); // - Liberamos los recursos que ocupaba GLFW.
}

Lullaby::Application::~Application() {
	releaseResources();
}

void Lullaby::Application::init(const std::string& title, const int width, const int height, const bool headless) {
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
		glfwSetWindowSizeCallback(_window, Renderer::resizeCallback);
	}
	_renderer->initRenderer(_window);
	if (!headless)
		_renderer->initSwapchain(_resolution);
	_renderer->initCommands();
	_renderer->initDefaultRenderpass();
	_renderer->initFramebuffers();
	_renderer->initSyncStructures();
	_renderer->initPipelines();
	WorldManager::getInstance();
	Lullaby::ObjLoader::parse(R"(C:\GitHub\LullabyEngine\Assets\Models\Dragon.obj)");
	fmt::print(fg(fmt::color::aquamarine), "Lullaby renderer initialized\n");
}

/**
 * \brief Put the logo in the app window and taskbar
 * \param path where the icon is. Can be a jpg, png or any fileformat supported by std_image.
 * \return true if the logo could be loaded and putted, false otherwise
 */
bool Lullaby::Application::registerLogo(const std::string& path) const {
	if (std::filesystem::exists(path)) {
		GLFWimage images[1];
		images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, nullptr, 4);
		if (images[0].pixels) {
			glfwSetWindowIcon(_window, 1, images);
			stbi_image_free(images[0].pixels);
			return true;
		}
	}
	return false;
}

void Lullaby::Application::renderLoop() const {
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		_renderer->render();
	}
}
