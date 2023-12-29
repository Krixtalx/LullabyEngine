#include "Lullaby/Lullaby.h"
#include <fmt/color.h>

int main() {
	auto app = Lullaby::Application();
	fmt::print(fg(fmt::color::aquamarine), "Starting Lullaby Engine...\n");

	try {
		app.init();
		app.renderLoop();
	} catch (const std::exception& exception) {
		fmt::print(stderr, fg(fmt::color::red), "{}\n", exception.what());
	}
	fmt::print(fg(fmt::color::aquamarine), "Closing Lullaby Engine...\n");
	return 0;
}
