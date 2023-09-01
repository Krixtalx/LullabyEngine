#include "stdafx.h"
#include "Core/Setup.h"

int main() {
	const auto setup = Lullaby::Setup::getInstance();
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