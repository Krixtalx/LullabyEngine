#include "stdafx.h"
#include "Core/Window.h"

int main() {
	const auto setup = Lullaby::Window::getInstance();
	std::cout << "Starting Lullaby Engine..." << std::endl;

	try {
		setup->init();
		setup->renderLoop();
	} catch (const std::exception& exception) {
		std::cerr << exception.what() << std::endl;
	}
	std::cout << "Closing Lullaby Engine..." << std::endl;
	return 0;
}
