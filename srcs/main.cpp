#include "main.hpp"
#include "ServerRouter.hpp"

int	main(int argc, char **argv)
{
	std::vector<t_config>		configs; //store vector of all server configs
	std::vector<std::string>	vectorConfigFile; //tmp store input config data in vector

	signal(SIGINT, ctrl_c_handler);
	std::string	configFile = ((argc > 1) ? argv[1] : "configs/configurationAcr.conf");
	// std::string	configFile = ((argc > 1) ? argv[1] : "configs/webserv.conf");
	printMsgToLogFile(-1, -1, datastamp(), "");
	printMsgToLogFile(-1, -1, "Starting...", "");
	printMsg(-1, -1, "Starting...", "");
	try
	{
		if (!openConfigFileAndWriteToVectorConfigFile(configFile, vectorConfigFile))
			throw ParserException();
		parserVectorConfigFile(vectorConfigFile, configs);
		ServerRouter data(configs, configFile);
		// #ifdef DEBUGMODE
		// 	printWebServer(data, "DEBUG main WebServer");
		// #endif
		data.start();
	}
	catch (const std::exception& e)
	{
		exitErr(e.what());
	}

	printMsgToLogFile(-1, -1, "Webserver stopped. Buy!\n ----------------------", "");
	return 0;
}

// make && ./webserver
// in Terminal:
// head="POST localhost:4242/downloads/%D0%BA%D0%BE%D1%80%D0%BE%D0%B2%D0%B0.txt?log=User&pass=123456 HTTP/1.1\r\nHost: localhost\r\nAuthorization: Basic myname:passwordinbs64\r\nContent-type: application/x-www-form-urlencoded\r\nContent-length: 10\r\nConnection: Close\r\n\r\nstatus=mymessage\r\n\r\n" ; echo $head | nc localhost 4242
// test by Client:
// c++ client.cpp && ./a.out && rm a.out
