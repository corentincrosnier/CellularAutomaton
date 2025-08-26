#include "HephGui.hpp"

int	main(int ac, char** av) {
	HephGui     gui;
	HephResult	result = gui.create();

	HEPH_PRINT_RESULT(result);

	if (!result.valid()) {
		return (1);
	}

	gui.run();
	gui.destroy();
	return (0);
}
