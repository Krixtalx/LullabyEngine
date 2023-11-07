#include "stdafx.h"
#include "Core/Window.h"

int main() {
	const auto setup = Lullaby::Window::getInstance();
	fmt::print(fg(fmt::color::aqua), "Starting Lullaby Engine...\n");

	try {
		setup->init();
		setup->renderLoop();
	} catch (const std::exception& exception) {
		fmt::print(stderr, fg(fmt::color::red), "{}\n", exception.what());
	}
	fmt::print(fg(fmt::color::aqua), "Closing Lullaby Engine...\n");
	return 0;
}
