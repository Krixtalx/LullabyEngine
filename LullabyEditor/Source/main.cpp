#include "Lullaby/Lullaby.h"
#include <fmt/color.h>

int main() {
	auto app = Lullaby::Application();
	print(fg(fmt::color::aquamarine), "Starting Lullaby Engine...\n");

	try {
		app.init();
		app.registerLogo(R"(C:\GitHub\LullabyEngine\Logo.png)");
		app.renderLoop();
	} catch (const std::exception& exception) {
		print(stderr, fg(fmt::color::red), "{}\n", exception.what());
	}
	print(fg(fmt::color::aquamarine), "Closing Lullaby Engine...\n");
	return 0;
}
