#include "curen_init.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main() {
    Curen::CurenInit curenInitializer;

	try
	{
		curenInitializer.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}
