#include <iostream>

#include "Config/WebservConfig.hpp"
#include "File/ConfigFile.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: test filename" << std::endl;
		return 1;
	}

	try
	{
		Hafserv::ConfigFile configFile = Hafserv::ConfigFile(argv[1]);
		std::cout << configFile << std::endl;
		std::cout << "-----------------------------------" << std::endl;
		configFile.include();
		std::cout << configFile << std::endl;
		Hafserv::WebservConfig config = Hafserv::WebservConfig(configFile);
		std::cout << config << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}
