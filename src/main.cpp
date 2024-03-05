#include <iostream>

#include "Config/WebservConfig.hpp"
#include "Webserv.hpp"

using namespace Hafserv;

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
		return 1;
	}

	Webserv &webserv = Webserv::getInstance();
	try
	{
		std::string confPath = (argc == 1) ? "./conf/default.conf" : std::string(argv[1]);
		ConfigFile configFile = ConfigFile(confPath);
		configFile.include();
		WebservConfig config = WebservConfig(configFile);

		for (std::vector<ServerConfig>::const_iterator it = config.getHttpConfig().getServers().begin();
			 it != config.getHttpConfig().getServers().end(); it++)
		{
			Server *server = new Server(*it);
			webserv.addServer(server);
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "thrown: " << e.what() << '\n';
		return 1;
	}

	webserv.runWebserv();

	return 0;
}
