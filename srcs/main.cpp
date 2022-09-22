#include "main.hpp"
#include "ServerRouter.hpp"

int	main(int argc, char **argv)
{
	std::vector<t_config>		configs; //store vector of all server configs
	std::vector<std::string>	vectorConfigFile; //tmp store input config data in vector

	signal(SIGINT, ctrl_c_handler);
	std::string	configFile = ((argc > 1) ? argv[1] : "configs/configurationAcr.conf");
	// std::string	configFile = ((argc > 1) ? argv[1] : "configs/webserv.conf");
	printMsgToLogFile(datastamp() + timestamp() + "Starting...");
	try
	{
		if (!openConfigFileAndWriteToVectorConfigFile(configFile, vectorConfigFile))
			throw ParserException();
		parserVectorConfigFile(vectorConfigFile, configs);
		ServerRouter data(configs);
		// #ifdef DEBUGMODE
		// 	printAllServersConfig(data.getConfigs(), "DEBUG main AllServersConfig");
		// #endif
		data.start();
	}
	catch (const std::exception& e)
	{
		exitErr(e.what());
	}

	printMsgToLogFile(timestamp() + "Webserver stopped. Buy!\n ----------------------");
	return 0;
}

// make && ./webserver
