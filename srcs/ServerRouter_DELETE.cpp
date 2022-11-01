#include "ServerRouter.hpp"

void ServerRouter::_prepareDeleteAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = DELETE;
	Server server = _getServer(connection->srvNbr);
	std::string contentTypeAndLengthAndData = "";
	if (!_delGetPath(connection, contentTypeAndLengthAndData))
		_addStatusCodePage(connection, contentTypeAndLengthAndData);
	
	connection->responseData.connectionAnswer += connection->responseData.statusCode + " " \
	+ connection->responseStatusCodesAll[connection->responseData.statusCode] + DELIMETER \
	+ timeStampHeader() + DELIMETER \
	+ "Server: \"" + WEBSERV_NAME + "\"" + DELIMETER \
	+ contentTypeAndLengthAndData;
	if (connection->responseData.statusCode == "200")
	{
		connection->responseData.connectionAnswer += "Content-Type: text/html; charset=utf-8";
		connection->responseData.connectionAnswer += DELIMETER;
		std::string htmlStr = "<html><head><meta http-equiv=\"refresh\" content=\"2; http://";
		htmlStr += _serverIp;
		htmlStr += ":" + std::to_string(server.getPort());
		htmlStr += "\" /></head><body><h1>File deleted.</h1></body></html>";
		connection->responseData.connectionAnswer += "Content-Length: " + std::to_string(htmlStr.size()) + DDELIMETER + htmlStr + DDELIMETER;
	}
}

bool ServerRouter::_delGetPath(t_connection * connection, std::string & contentTypeAndLengthAndData)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	std::string pathToServer = "./";

	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath connection->inputData.address \nconnection->inputData.address: " << NC << connection->inputData.address << "\n----------------------" << std::endl;
	#endif

	size_t i;
	bool isMethodAllowed = false;
	if (server.getConfig().root != "")
		pathToServer += server.getConfig().root + "/";

	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath pathToServer \npathToServer: " << NC << pathToServer << "\n----------------------" << std::endl;
	#endif
	std::string path = pathToServer + connection->inputData.address;
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath path \npath: " << NC << path << "\n----------------------" << std::endl;
	#endif

	const char * pathChar = path.c_str();

	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL)
	{
		msg = "Error! File " + path + " is not exist, sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
		return false;
	}
	fclose(fileOpen);

	// check methods
	size_t pos = findLastSlashInAddress(connection->inputData.address);
	std::string pathToFolderData = "";
	pathToFolderData.append(connection->inputData.address, 0, pos);
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath pathToFolderData \npathToFolderData: " << NC << pathToFolderData << "\n----------------------" << std::endl;
	#endif
	if (server.getConfig().methods.size() > 0)
	{
		#ifdef DEBUGMODE
			std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath methods.size() > 0 \nmethods.size() > 0 " << NC  << "\n----------------------" << std::endl;
		#endif
		for (std::vector<std::string>::const_iterator iter = server.getConfig().methods.begin(); iter < server.getConfig().methods.end(); iter++)
			if ((*iter) == connection->inputData.method)
			{
				isMethodAllowed = true;
				#ifdef DEBUGMODE
					std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath isMethodAllowed = true \nisMethodAllowed = true: " << NC  << "\n----------------------" << std::endl;
				#endif
			}
	}
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		#ifdef DEBUGMODE
			std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath locations.methods.size() > 0 \nlocations.methods.size() > 0 " << NC  << "\n----------------------" << std::endl;
		#endif
		if (pathToFolderData == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].methods.size() > 0)
			{
				isMethodAllowed = false;
				for (std::vector<std::string>::const_iterator iter = server.getConfig().locations[i].methods.begin(); iter < server.getConfig().locations[i].methods.end(); iter++)
					if ((*iter) == connection->inputData.method)
						isMethodAllowed = true;
			}
		}
	}
	if (!isMethodAllowed)
	{
		msg = "Error! Method " + connection->inputData.method + " is not allowed on this server, sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "405";
		return false;
	}

	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_DEL _delGetPath file found \nfile found: " << NC << path << "\n----------------------" << std::endl;
	#endif

	remove(pathChar);
	msg = "File " + path + " was removed, sd ";
	printMsg(connection->srvNbr, connection->clntSd, msg, "");
	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
	connection->responseData.statusCode = "200";

	(void) contentTypeAndLengthAndData;
	return true;
}
