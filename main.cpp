#include <iostream>
#include "Controller.h"

int main()
{
	std::cout << "UNIQ\n";
	auto* controller = new Controller();
	if (!controller->init())
	{
		std::cout << "Failed to initialize controller!\n";
	} else
	{
		std::cout << "Process: " << controller->moduleName << " (" << controller->processId << ")\n";
	}


	// dont do anything else, just wait for the user to press enter
	system("pause");

	return 0;
}
