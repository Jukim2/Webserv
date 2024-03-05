#include "File/Directory.hpp"
#include "File/RegularFile.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: test filename" << std::endl;
		return 1;
	}

	Hafserv::Directory dir = Hafserv::Directory(argv[1]);
	std::cout << dir.toHtml();
	return 0;
}
