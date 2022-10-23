#include "ServerRouter.hpp"

void ServerRouter::_prepareDeleteAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = DELETE;
	Server server = _getServer(connection->srvNbr);
	std::string path = _delGetPath(connection);

}

std::string ServerRouter::_delGetPath(t_connection * connection)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	std::string pathToServer = "./";

	size_t i;
	if (server.getConfig().root != "")
		pathToServer += server.getConfig().root + "/";
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		if (connection->inputData.address == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				pathToServer += server.getConfig().locations[i].root + "/";
		}
	}

	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath pathToServer \npathToServer: " << NC << pathToServer << "\n----------------------" << std::endl;
	#endif
	std::string path = pathToServer + connection->inputData.address;
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath path \npath: " << NC << path << "\n----------------------" << std::endl;
	#endif
}
