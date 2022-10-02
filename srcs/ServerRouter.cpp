#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs)
{
	_configs = configs;
	_responseStatusCodesInit();
	_contentTypesInit();
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
	// _pollfds.clear();

	// #ifdef DEBUGMODE
	// 	printAllServersConfig(_configs, "DEBUG ServerRouter AllServersConfig");
	// #endif
	for (std::vector<t_config>::iterator iter = _configs.begin(); iter < _configs.end(); iter++)
		_servers.push_back(Server(*iter));
	_pollfdsQty = _servers.size();
	_pollfdsInit();
	std::string arr[] = {"GET", "POST", "DELETE"};
	std::vector<std::string> metods(std::begin(arr), std::end(arr));
	_allowedMethods.clear();
	_allowedMethods = metods;
	// #ifdef DEBUGMODE
	// 	printAllServersVector(_servers, "DEBUG ServerRouter AllServersVector");
	// #endif
}

ServerRouter::~ServerRouter() {}

bool ServerRouter::_launch(Server & server, int indx)
{
	int on = 1;
	struct sockaddr_in addr;
	int sd;
	struct pollfd pfd;
	std::string msg;

	// Create socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		msg = "Can not start a server: can not create socket.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}
	// std::cout << "_sd = " << _sd << std::endl;

	// Set socket options
	// SOL_SOCKET - присвоение параметра на уровне библиотеки сокетов
	// SO_REUSEADDR - разрешает повторное использование локальных адресов
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		close (sd);
		msg = "Can not start a server: can not set socket options.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// преобразуем адрес IPv4 или IPv6 из символьного представления в двоичную форму с сетевым порядком следования байтов
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atou16(server.getConfig().port.c_str()));
	addr.sin_len = sizeof(addr);
	// addr.sin_addr.s_addr = inet_addr(server.getConfig().ip.c_str());
	// bzero(&(addr.sin_zero), 8);
	if (inet_pton(addr.sin_family, server.getConfig().ip.c_str(), &(addr.sin_addr)) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not convert listen address.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// Функция bind() задает порт или имя файла для привязки сокета
	if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not set the port to bind the socket to.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// man listen
	// The backlog parameter defines the maximum length for the queue of pending connections. 
	// If a connection request arrives with the queue full, the client may receive an error with an indication of ECONNREFUSED.  
	// Alternatively, if the underlying protocol supports retransmission, the request may be ignored so that retries may succeed.
	if (listen(sd, SOMAXCONN) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not listen queue.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	server.setSd(sd);
	server.setSrvrNbr(indx);
	pfd.fd = sd;
	pfd.events = POLLIN;
	_pollfds[indx] = pfd;

	char serverIp[sizeof(struct sockaddr_in)];
	inet_ntop(AF_INET, &(addr.sin_addr), serverIp, sizeof(addr));

	std::cout << NC << timestamp() << YELLOS << "On socket descriptor " << NC << sd \
	<< YELLOS << " the " << NC << server.getConfig().serverName \
	<< YELLOS << " server[" << server.getServNbr() << "] started successfully and is listening on " << NC \
	<< serverIp << ":" << ntohs(addr.sin_port) << std::endl;
	// std::cout << "\nTo connect via this Mac use " \
	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
	// << NC << _hostname << YELLOS << ", port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << std::endl;

	msg = "On sd " + std::to_string(sd) + " the '" + server.getConfig().serverName + "' server[" + std::to_string(server.getServNbr()) + "] started successfully and is listening on " + serverIp + ":" + std::to_string(ntohs(addr.sin_port));
	printMsgToLogFile(-1, -1, msg, "");

	return true;
}

void ServerRouter::start()
{
	std::string msg;

	std::cout << NC << timestamp() << YELLOS << NAME << ": Welcome to the " << WEBSERV_NAME << " v."<< WEBSERV_VERSION << " by " \
	<< WEBSERV_AUTHORS << "\nIt was tested on MAC OS.\n" \
	<< "It was launched at " << NC << _hostname << "\n";
	// std::cout << "\nTo connect via this Mac use " \
	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
	// << NC << _hostname << YELLOS << ", port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << std::endl;
	
	msg = "Webserver ";
	msg += WEBSERV_NAME;
	msg += " started at " + _hostname;
	printMsgToLogFile(-1, -1, msg, "");

	int indx = 0;
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if (!_launch(*iter, indx))
			exitErr ("Check server config file and try again.");
		indx++;
	}

	// #ifdef DEBUGMODE
	// printPollfds(_pollfds, "DEBUG _pollfds", _pollfdsQty);
	// #endif

	while (42)
	{
		if (!_mainLoop())
		{
			std::cout << "!_mainLoop()" << std::endl;
			break;
		}
		_checkTimeout(); //disconnect by timeout
		// std::cout << "Hi42!" << std::endl;
	}

	_closeSockets();
}

bool ServerRouter::_mainLoop()
{
	// int sd = -1;
	socklen_t socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in addrNew;
	std::string msg;

	// std::cout << "_mainLoop()" << std::endl;
	int ret = poll(_pollfds, _pollfdsQty, -1);
	if (ret < 0 || ret == 0)
	{
		_closeSockets();
		if (ret < 0)
			exitErr ("Poll error.");
		exitErr ("Poll error timeout.");
	}
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		// std::cout << "size_t i = 0; i < _pollfdsQty; i++" << std::endl;
		if (_pollfds[i].revents == 0)
		{
			// std::cout << "revents == 0" << std::endl;
			continue;
		}
		if (_isSocketServer(_pollfds[i].fd))
		{
			std::cout << "_isSocketServer" << std::endl;
			int sd = accept(_pollfds[i].fd, (struct sockaddr *)&addrNew, &socklen);
			fcntl(sd, F_SETFL, O_NONBLOCK);
			if (sd < 0)
			{
				// if (errno != EWOULDBLOCK)
				// {
					msg = "sd " + std::to_string(_pollfds[i].fd) + ": Accept error.";
					printMsgErr(-1, -1, msg, "");
					printMsgToLogFile(-1, -1, msg, "");
					return false;
				// }
				break ;
			}
			_pollfds[_pollfdsQty].fd = sd;
			_pollfds[_pollfdsQty].events = POLLIN;
			_pollfdsQty++;
			_saveConnection(sd, i, inet_ntoa(((struct sockaddr_in*)&addrNew)->sin_addr), ntohs(((struct sockaddr_in*)&addrNew)->sin_port));
			msg = "new connection from " + _getConnection(sd)->fromIp + ":" + std::to_string(_getConnection(sd)->fromPort) + ", sd: ";
			printMsg(i, sd, msg, "");
			printMsgToLogFile(i, sd, msg, "");
		}
		else
		{
			// std::cout << "else" << std::endl;
			int clntSd = _pollfds[i].fd;
			t_connection * connection = _getConnection(clntSd);
			connection->pfd = &(_pollfds[i]);
			char buf[BUF_SIZE + 1];
			if (_pollfds[i].revents & POLLIN) // есть данные для чтения
			{
				// std::cout << "_pollfds[i].revents & POLLIN" << std::endl;
				// _setConnectionLastActivity(connection->lastActivityTime);
				_readSd(connection);

				// #ifdef DEBUGMODE
				// 	printConnection(* connection, "DEBUGMODE _mainLoop printConnection 1", 1);
				// #endif

				if (connection->requestProcessingStep == READ_DONE)
					connection->pfd->events = POLLOUT;

				// #ifdef DEBUGMODE
				// 	printConnection(* connection, "DEBUGMODE _mainLoop printConnection 2", 1);
				// #endif

				// _pollfds[i].revents = 0;
			}
			else if (_pollfds[i].revents & POLLOUT) // запись возможна
			{
				std::cout << "POLOUT" << std::endl;
				int err = _sendAnswer(connection);

				if (connection->requestProcessingStep == WRITE_DONE)
				{
					connection->pfd->events = POLLIN;
					connection->inputStr = "";
				}

				if (err < 0)
				{
					msg = "client closed sd ";
					printMsg(connection->srvNbr, clntSd, msg, "");
					printMsgToLogFile(connection->srvNbr, clntSd, msg, "");
					_closeConnection (clntSd);
					continue ;
				}

				std::cout << "POLOUT2" << std::endl;
			}
			else if (recv(clntSd, buf, BUF_SIZE, MSG_PEEK) == 0)
			{
				msg = "client closed sd ";
				printMsg(connection->srvNbr, clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, clntSd, msg, "");
				_closeConnection (clntSd);
			}
			// else
			// 	_closeConnection (clntSd);
		}
		// std::cout << "Hi!" << std::endl;
	}
	return true;
}

int ServerRouter::_sendAnswer(t_connection * connection)
{
	std::string msg;
	std::string delim = DELIMETER;
	connection->responseData.connectionAnswer.clear();
	// std::string answer = connection->inputData.httpVersion + " ";
	connection->responseData.connectionAnswer = connection->inputData.httpVersion + " ";
	if (connection->inputData.dataType == HTTP)
	{
		if (std::find(connection->allowedMethods.begin(), connection->allowedMethods.end(), connection->inputData.method) == connection->allowedMethods.end() )
		{
			msg = "Error! Unknown method from sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			// _addStatusCode(answer, connection, "405");
			connection->responseData.statusCode = "405";
			connection->responseData.connectionAnswer += connection->responseData.statusCode + " " + connection->responseStatusCodesAll[connection->responseData.statusCode] + DELIMETER;
		}
		else if (connection->inputData.method == "GET")
			_prepareGetAnswer(connection);
		else if (connection->inputData.method == "POST")
			_preparePostAnswer(connection);
		else if (connection->inputData.method == "DELETE")
			_prepareDeleteAnswer(connection);
	}


	// #ifdef DEBUGMODE
	// 	std::cout << "**** DEBUGMODE ServerRouter//_sendanswer answer ****\nAnswer:\n" << connection->responseData.connectionAnswer << std::endl;
	// #endif

	// if (!connection->responseData.lenAnswer)
	// 	connection->responseData.lenAnswer = connection->responseData.connectionAnswer.str().length();
	// int sizePacket, bytesSent;

	// sizePacket = (connection->responseData.lenAnswer - connection->responseData.lenSent) > BUF_SIZE ? BUF_SIZE : (connection->responseData.lenAnswer - connection->responseData.lenSent);

	// connection->responseData.connectionAnswer.rdbuf()->sgetn(connection->responseData.buf, sizePacket);
	// std::cout << "connection->responseData.connectionAnswer: " << connection->responseData.connectionAnswer << "\n";
	if (!connection->responseData.lenAnswer)
		connection->responseData.lenAnswer = connection->responseData.connectionAnswer.length();

	int sizePacket, bytesSent;

	sizePacket = (connection->responseData.lenAnswer - connection->responseData.lenSent) > BUF_SIZE ? BUF_SIZE : (connection->responseData.lenAnswer - connection->responseData.lenSent);

	// connection->responseData.buf = (connection->responseData.connectionAnswer.substr (connection->responseData.lenSent, sizePacket)).c_str();


	// connection->responseData.connectionAnswer.rdbuf()->sgetn(connection->responseData.buf, sizePacket);
	bytesSent = send(connection->clntSd, (connection->responseData.connectionAnswer.substr (connection->responseData.lenSent, sizePacket)).c_str(), sizePacket, 0);
	if (bytesSent < 0)
		return bytesSent;
	connection->responseData.lenSent += bytesSent;
	msg = "sent " + std::to_string(connection->responseData.lenSent) + "/" + std::to_string(connection->responseData.lenAnswer) + " bytes to sd ";
	printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
	if (connection->responseData.lenAnswer <= connection->responseData.lenSent)
	{
		connection->requestProcessingStep = WRITE_DONE;
		connection->responseData.lenAnswer = 0;
	}
	// _setConnectionLastActivity(connection->lastActivityTime);
	return bytesSent; //?
}

void ServerRouter::_prepareGetAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = GET;
	Server server = _getServer(connection->srvNbr);
	bool correctAddr = false;
	if (connection->inputData.address != "/")
	{
		for (size_t i = 0; i < server.getConfig().locations.size(); i++)
		{
			if (connection->inputData.address == server.getConfig().locations[i].path)
				correctAddr = true;
		}
	}
	// if (!correctAddr)
	// {
	// 	msg = "Error! Unknown location addr from sd ";
	// 	printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
	// 	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
	// 	connection->responseData.statusCode = "404";
	// }

	std::string contentTypeAndLength = "";
	// _addFileToAnswer(contentTypeAndLength, connection)
	if (!_addFileToAnswer(contentTypeAndLength, connection))
		; // Подумать, что вернуть, если не откроется файл

	connection->responseData.connectionAnswer += connection->responseData.statusCode \
	+ " " + connection->responseStatusCodesAll[connection->responseData.statusCode] \
	+ DELIMETER + "Server: \"" + WEBSERV_NAME + "\"" + DELIMETER \
	+ contentTypeAndLength + DDELIMETER;



}

void ServerRouter::_preparePostAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = POST;
	Server server = _getServer(connection->srvNbr);

}

void ServerRouter::_prepareDeleteAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = DELETE;
	Server server = _getServer(connection->srvNbr);

}

bool ServerRouter::_addFileToAnswer(std::string & contentTypeAndLength, t_connection * connection)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	// std::string path = server.getConfig().listen + connection->inputdata.address;
	// std::string path = "./" + connection->inputData.address;

	std::string path = "./";

	size_t i;
	if (server.getConfig().root != "")
		path += server.getConfig().root + "/";
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		if (connection->inputData.address == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				path += server.getConfig().locations[i].root + "/";
	// 		if (server.getConfig().locations[i].index != "")
	// 			path += server.getConfig().locations[i].index;
	// 		else
	// 			path += "index.html";
	// 		break;
		}
	}
	// if (i == server.getConfig().locations.size())
	// {
	// 	if (server.getConfig().index != "")
	// 		path += server.getConfig().index;
	// 	else
	// 		path += "index.html";
	// }

	path += connection->inputData.address;

	// size_t i;
	// for (i = 0; i < server.getConfig().locations.size(); i++)
	// {
	// 	if (connection->inputdata.address == server.getConfig().locations[i].path)
	// 	{
	// 		if (server.getConfig().locations[i].root != "")
	// 			path += server.getConfig().locations[i].root;
	// 		if (server.getConfig().locations[i].index != "")
	// 			path += server.getConfig().locations[i].index;
	// 		break;
	// 	}
	// }
	// if (i == server.getConfig().locations.size())
	// {
	// 	if (server.getConfig().root != "")
	// 		path += server.getConfig().root;
	// 	if (server.getConfig().index != "")
	// 		path += server.getConfig().index;
	// }

	const char * pathChar = path.c_str();
	struct stat buf;
	
	lstat(pathChar, & buf);
	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL)
	{
		msg = "Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
	}
	else if (S_ISREG(buf.st_mode)) //it's path to file
	{
		msg = "the file " + path + " was sucsessfully opened, sd ";
		printMsg(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "200";

		fseek(fileOpen, 0L, SEEK_END); // перемотать на конец файла
		size_t fileLength = ftell(fileOpen);
		std::cout << "fileLength: " << fileLength << std::endl;

		std::string pathTmp = path;
		pathTmp.erase(pathTmp.begin());
		size_t dot = pathTmp.find('.');
		std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
		// std::string ext = path.substr(path.find('.'), 6);
		std::cout << RED << "ext = " << ext << NC << std::endl;
		std::string contType;
		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
			contType = connection->contentTypesAll[ext];
		else
			contType = "text/html";

		// exec(file --brief --mime-type path);

		char * buffer = (char *)malloc(sizeof(char) * fileLength);
		if (!buffer)
		{
			fclose(fileOpen);
			// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
			return false;
		}
		// std::cout << "fileLength: " << fileLength << std::endl;
		// std::cout << "buffer.size(): " << sizeof(buffer) << std::endl;
		fseek(fileOpen, 0L, SEEK_SET); // перейти на начало файла
		if (fileLength != fread(buffer, 1, fileLength, fileOpen))
		{
			free(buffer);
			fclose(fileOpen);
			// fputs( "Read data size is not equal to actual file size.", stderr );
			return false;
		}

		connection->responseData.fileToSendInBinary = buffer;
		// std::cout << "fileToSendInBinary.size(): " << connection->responseData.fileToSendInBinary.size() << std::endl;
		// std::cout << "buffer: " << buffer << std::endl;
		fclose(fileOpen);
		free(buffer);

		// // char * S = (char *)malloc(sizeof(char) * fileLength);     //Символьный массив в 255 символов
    	// char S[1598740] = {};
		// /*НАЧАЛО РАБОТЫ С ФАЙЛОМ*/
		// // if (!S)
		// // {
		// // 	// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
		// // 	return false;
		// // }
		// std::ifstream in(pathChar, std::ios::binary);
		// in.read((char*)&S, sizeof(S));          //перенос байтов из файла в "х"
		// // in.read((char*)S, sizeof(S));          //в случае с массивом можно и так
		// in.close();
		// /*КОНЕЦ РАБОТЫ С ФАЙЛОМ*/
		// std::cout << S << '\n';
		// // free (S);
		// connection->responseData.fileToSendInBinary = S;
		// std::cout << "std::to_string(fileLength): " << std::to_string(fileLength) << "\n";
		// std::cout << "pathChar: " << pathChar <<"\n";
		std::string line, S;
		S = "";
		std::ifstream in(pathChar, std::ios::binary);
		if (in.is_open())
		{
			while (getline(in, line))
			{
				// std::cout << line << std::endl;
				S += line + "\n";
			}
		}
		in.close();
		// std::cout << "S:\n" << S << "\n";
		connection->responseData.fileToSendInBinary = S;

		// std::string ff = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
		// ff += "\t\t<title>The Periodic Table</title>\n\t</head>\n\t<body>\n";
		// ff += "\t\t<h2>Dmitri Mendeleev\'s periodic table of the elements</h2>\n\t\t<br>\n\t\t<table style = 'border: 1px blue solid;'>\n";
		// ff += "\t\t</table>\n\t</body>\n</html>";

		contentTypeAndLength += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
		// contentTypeAndLength += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(ff.size()) + DDELIMETER + ff + DDELIMETER;
		// std::stringstream bufFile;
		// bufFile << fileOpen.rdbuf();
	}
	else if (S_ISDIR(buf.st_mode)) //it's path to dir
	{
		//  + connection->inputData.address
		fclose(fileOpen);
		size_t i;
		for (i = 0; i < server.getConfig().locations.size(); i++)
		{
			if (connection->inputData.address == server.getConfig().locations[i].path)
			{
				// if (server.getConfig().locations[i].root != "")
				// 	path += server.getConfig().locations[i].root + "//";
				if (server.getConfig().locations[i].index != "")
					path += server.getConfig().locations[i].index;
				else
					path += "index.html";
				break;
			}
		}
		if (i == server.getConfig().locations.size())
		{
			if (server.getConfig().index != "")
				path += server.getConfig().index;
			else
				path += "index.html";
		}
		std::cout << RED << path << NC << std::endl;

		const char * pathChar2 = path.c_str();
		struct stat buf2;
		lstat(pathChar2, & buf2);
		FILE * fileOpen2 = fopen(pathChar2, "rb"); //r - read only, b - in binary
		if (fileOpen2 == NULL)
		{
			msg = "Error! Can not open the file " + path + ", sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "404";
		}
		if (S_ISREG(buf2.st_mode))  //it's path to file
		{
			std::cout << RED << "HI!!!" << NC << std::endl;
			msg = "the file " + path + " was sucsessfully opened, sd ";
			printMsg(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "200";

			fseek(fileOpen2, 0L, SEEK_END); // перемотать на конец файла
			size_t fileLength2 = ftell(fileOpen2);
			std::cout << "fileLength2: " << fileLength2 << std::endl;

			size_t dot = path.find(".");
			std::string ext = path.substr(dot + 1, path.length() - dot);
			std::string contType;
			if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
				contType = connection->contentTypesAll[ext];
			else
				contType = "text/html";

			char * buffer2 = (char *)malloc(fileLength2);
			if (!buffer2)
			{
				fclose(fileOpen2);
				// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
				return false;
			}
			fseek(fileOpen2, 0L, SEEK_SET); // перейти на начало файла
			if (fileLength2 != fread(buffer2, 1, fileLength2, fileOpen2))
			{
				free(buffer2);
				fclose(fileOpen2);
				// fputs( "Read data size is not equal to actual file size.", stderr );
				return false;
			}

			connection->responseData.fileToSendInBinary = buffer2;
			fclose(fileOpen2);
			free(buffer2);

			// std::string ff = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
			// ff += "\t\t<title>The Periodic Table</title>\n\t</head>\n\t<body>\n";
			// ff += "\t\t<h2>Dmitri Mendeleev\'s periodic table of the elements</h2>\n\t\t<br>\n\t\t<table style = 'border: 1px blue solid;'>\n";
			// ff += "\t\t</table>\n\t</body>\n</html>";

			contentTypeAndLength += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength2) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
			// contentTypeAndLength += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(ff.size()) + DDELIMETER + ff + DDELIMETER;
			// std::stringstream bufFile;
			// bufFile << fileOpen2.rdbuf();
		}
	}
	else
	{
		msg = "Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
	}
	return true;
}

// void ServerRouter::_addFileToAnswer(std::string & contentTypeAndLength, t_connection * connection)
// {
// 	std::string msg;
// 	Server server = _getServer(connection->srvNbr);
// 	// std::string path = server.getConfig().listen + connection->inputdata.address;
// 	std::string path = "." + connection->inputData.address;

// 	size_t i;
// 	if (server.getConfig().root != "")
// 		path += server.getConfig().root;
// 	for (i = 0; i < server.getConfig().locations.size(); i++)
// 	{
// 		if (connection->inputData.address == server.getConfig().locations[i].path)
// 		{
// 			if (server.getConfig().locations[i].root != "")
// 				path += server.getConfig().locations[i].root;
// 			if (server.getConfig().locations[i].index != "")
// 				path += server.getConfig().locations[i].index;
// 			else
// 				path += "index.html";
// 			break;
// 		}
// 	}
// 	if (i == server.getConfig().locations.size())
// 	{
// 		if (server.getConfig().index != "")
// 			path += server.getConfig().index;
// 		else
// 			path += "index.html";
// 	}

// 	// size_t i;
// 	// for (i = 0; i < server.getConfig().locations.size(); i++)
// 	// {
// 	// 	if (connection->inputdata.address == server.getConfig().locations[i].path)
// 	// 	{
// 	// 		if (server.getConfig().locations[i].root != "")
// 	// 			path += server.getConfig().locations[i].root;
// 	// 		if (server.getConfig().locations[i].index != "")
// 	// 			path += server.getConfig().locations[i].index;
// 	// 		break;
// 	// 	}
// 	// }
// 	// if (i == server.getConfig().locations.size())
// 	// {
// 	// 	if (server.getConfig().root != "")
// 	// 		path += server.getConfig().root;
// 	// 	if (server.getConfig().index != "")
// 	// 		path += server.getConfig().index;
// 	// }

// 	const char * pathChar = path.c_str();
// 	struct stat buf;
	
// 	lstat(pathChar, & buf);
// 	FILE * file = fopen(pathChar, "rb"); //r - read only, b - in binary
// 	if (!S_ISREG(buf.st_mode) || file == NULL)
// 	{
// 		msg = "Error! Can not open the file " + path + ", sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "404";
// 	}
// 	else
// 	{
// 		msg = "the file " + path + " was sucsessfully opened, sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "200";

// 		fseek(file, 0L, SEEK_END);
// 		int fileLength = ftell(file);

// 		int dot = path.find(".");
// 		std::string ext = path.substr(dot + 1, path.length() - dot);
// 		std::string contType;
// 		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
// 			contType = connection->contentTypesAll[ext];
// 		else
// 			contType = "text/html";

// 		std::string ff = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
// 		ff += "\t\t<title>The Periodic Table</title>\n\t</head>\n\t<body>\n";
// 		ff += "\t\t<h2>Dmitri Mendeleev\'s periodic table of the elements</h2>\n\t\t<br>\n\t\t<table style = 'border: 1px blue solid;'>\n";
// 		ff += "\t\t</table>\n\t</body>\n</html>";

// 		// contentTypeAndLength += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER;
// 		contentTypeAndLength += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(ff.size()) + DDELIMETER + ff + DDELIMETER;
// 		// std::stringstream bufFile;
// 		// bufFile << file.rdbuf();
// 		(void)fileLength;
// 		fclose(file);
// 	}
// }

int ServerRouter::_readSd(t_connection * connection)
{
	char buf[BUF_SIZE + 1];
	std::string msg;

	int qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
	if (qtyBytes == 0) // ?? было ли всё полностью получено? не было ли разрыва?
	{
		// msg = "finished reading data from sd ";
		// printMsg(connection->srvNbr, connection->clntSd, msg, "");
		// printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		// connection->requestProcessingStep = READ_DONE;
		// connection->responseData.statusCode = "200";
		return 0;
	}
	else if (qtyBytes > 0)
	{
		buf[qtyBytes] = '\0';
		msg = "got " + std::to_string(qtyBytes) + " bytes from sd ";
		printMsg(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");

		std::string tmp = buf;
		std::string tmpEnd = "";
		size_t pos = tmp.find(DDELIMETER);
		// std::cout << "pos: " << pos << "\tlenInputStr: " << tmp.size() << std::endl;
		if (connection->requestProcessingStep == NOT_DEFINED_REQUEST_PROCESSING_STEP)
		{
			connection->inputStrHeader = tmp.substr (0, pos);
			connection->requestProcessingStep = READ_DONE;
			connection->responseData.statusCode = "200";
			if (pos < tmp.size() - 4)
			{
				tmpEnd = tmp.substr (pos + 4);
				size_t posEnd = tmpEnd.find(DDELIMETER);
				if (posEnd < tmpEnd.size())
				{
					connection->inputStrBody = tmpEnd.substr(0, posEnd);
					msg = "finished reading data from sd ";
					printMsg(connection->srvNbr, connection->clntSd, msg, "");
					printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
				}
				else
				{
					connection->inputStrBody = tmpEnd;
					connection->requestProcessingStep = READ;
					connection->responseData.statusCode = "100";
				}
			}
			else
			{
				msg = "finished reading data from sd ";
				printMsg(connection->srvNbr, connection->clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			}
		}
		else
		{
			connection->inputStrBody += tmp.substr (0, pos);
			if (pos < tmp.size())
			{
				connection->requestProcessingStep = READ_DONE;
				connection->responseData.statusCode = "200";
				msg = "finished reading data from sd ";
				printMsg(connection->srvNbr, connection->clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			}
		}
		connection->inputStr += buf;
		if (connection->requestProcessingStep == READ_DONE)
		{
			if (!_parseInputData(connection))
			{
				while (qtyBytes > 0)
					qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
				// connection->inputStr = "";
				connection->responseData.statusCode = "100";
				return 0;
			}
			// connection->inputStr = "";
		}




	}
	return qtyBytes;
}

// void ServerRouter::_addStatusCode(std::string & answer, t_connection * connection, std::string code)
// {
// 	if (connection->responseStatusCodesAll.find(code) != connection->responseStatusCodesAll.end())
// 		answer += code + " " + connection->responseStatusCodesAll[code] + DELIMETER;
// 	else
// 		answer += "200 " + connection->responseStatusCodesAll["200"] + DELIMETER;
// }

// bool ServerRouter::_responseCheckMethod()
// {
// 	return true;
// }

void ServerRouter::_removeSdFromPollfds(int clntSd)
{
	// _pollfds[indx].fd = -1;
	// for (size_t i = 0; i < _pollfdsQty; i++)
	// {
	// 	if (_pollfds[i].fd < 0)
	// 	{
	// 		for (size_t j = i; j < _pollfdsQty - 1; j++)
	// 			_pollfds[j] = _pollfds[j + 1];
	// 		_pollfdsQty--;
	// 	}
	// }
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd == clntSd)
		{
			_pollfds[i].fd = -1;
			for (size_t j = i; j < _pollfdsQty - 1; j++)
				_pollfds[j] = _pollfds[j + 1];
			_pollfdsQty--;
		}
	}
}

void ServerRouter::_removeConnection(int clntSd)
{
	for (std::vector<t_connection>::iterator iter = _connections.begin(); iter < _connections.end(); iter++)
	{
		if ((*iter).clntSd == clntSd)
			_connections.erase(iter);
	}
}

void ServerRouter::_closeSockets()
{
	for (size_t i = 0; i < sizeof(_servers); i++)
	{
		if (_pollfds[i].fd >= 0)
			close (_pollfds[i].fd);
	}
}

void ServerRouter::_closeConnection(int clntSd)
{
	close (clntSd);
	_removeSdFromPollfds(clntSd);
	_removeConnection(clntSd);
}

bool ServerRouter::_isSocketServer(int fd)
{
	// #ifdef DEBUGMODE
	// 	std::cout << "**** DEBUG _isSocketServer ****\nfd: " << fd << std::endl;
	// #endif
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		// #ifdef DEBUGMODE
		// 	std::cout << "_sd: " << (*iter).getSd() << std::endl;
		// #endif
		if ((*iter).getSd() == fd)
		{
			// #ifdef DEBUGMODE
			// 	std::cout << "-------------------------" << std::endl;
			// #endif
			return true;
		}
	}
	// #ifdef DEBUGMODE
	// 	std::cout << "-------------------------" << std::endl;
	// #endif
	return false;
}

t_connection * ServerRouter::_getConnection(int clntSd)
{
	for (std::vector<t_connection>::iterator iter = _connections.begin(); iter < _connections.end(); iter++)
	{
		if ((*iter).clntSd == clntSd)
			return &(*iter);
	}
	return NULL;
}

void ServerRouter::_saveConnection(int sdFrom, int srvNbr, std::string fromIP, unsigned fromPort)
{
	t_connection connection;
	connectionClear(connection);
	connection.srvNbr = srvNbr;
	connection.fromIp = fromIP;
	connection.fromPort = fromPort;
	connection.clntSd = sdFrom;
	connection.allowedMethods = _allowedMethods;
	connection.responseStatusCodesAll = _responseStatusCodes;
	connection.contentTypesAll = _contentTypes;
	_setConnectionLastActivity(connection.lastActivityTime);
	_connections.push_back(connection);
}

void ServerRouter::_setConnectionLastActivity(long int & lastActivityTime)
{
	struct timeval	tm;
	gettimeofday(&tm, NULL);
	lastActivityTime = tm.tv_sec;
}

void ServerRouter::_checkTimeout()
{
	std::string msg, msg1;
	struct timeval	tm1;
	gettimeofday(&tm1, NULL);
	for (std::vector<t_connection>::iterator iter = _connections.begin(); iter < _connections.end(); iter++)
	{
		// #ifdef DEBUGMODE
		// 	std::cout << "**** DEBUGMODE _checkTimeout ****\ntime now: " << tm1.tv_sec << "\tlastActivityTime: " << (*iter).lastActivityTime << "\tTIMEOUT: " << TIMEOUT << "\n--------" << std::endl;
		// #endif
		bool keepAlive = false;
		for (std::map<std::string, std::string>::iterator iterF = (*iter).inputData.headerFields.begin(); iterF != (*iter).inputData.headerFields.end(); iterF++)
		{
			size_t pos = (*iterF).second.find("keep-alive");
    		if (pos != std::string::npos)
				keepAlive = true;
			// if ((*iterF).second == "keep-alive")
		}
		if ((tm1.tv_sec - (*iter).lastActivityTime >= TIMEOUT) && !keepAlive)
		{
			msg = "closed sd ";
			msg1 = " by timeout";
			printMsg((*iter).srvNbr, (*iter).clntSd, msg, msg1);
			printMsgToLogFile((*iter).srvNbr, (*iter).clntSd, msg, msg1);
			_closeConnection((*iter).clntSd);
		}
	}
}

void ServerRouter::_responseStatusCodesInit()
{
	_responseStatusCodes.clear();
	std::pair<std::string, std::string> responseStatusCodesArr[] =
	{
		std::make_pair("100", "Continue"), std::make_pair("101", "Switching Protocols"), std::make_pair("102", "Processing"), 
		std::make_pair("103", "Early Hints"), std::make_pair("200", "OK"), std::make_pair("201", "Created"), std::make_pair("202", "Accepted"), 
		std::make_pair("203", "Non-Authoritative Information"), std::make_pair("204", "No Content"), std::make_pair("205", "Reset Content"), 
		std::make_pair("206", "Partial Content"), std::make_pair("207", "Multi-Status"), std::make_pair("208", "Already Reported"), 
		std::make_pair("226", "IM Used"), std::make_pair("300", "Multiple Choices"), std::make_pair("301", "Moved Permanently"), 
		std::make_pair("302", "Found"), std::make_pair("303", "See Other"), std::make_pair("304", "Not Modified"), 
		std::make_pair("305", "Use Proxy"), std::make_pair("307", "Temporary Redirect"), std::make_pair("308", "Permanent Redirect"), 
		std::make_pair("400", "Bad Request"), std::make_pair("401", "Unauthorized"), std::make_pair("402", "Payment Required"), 
		std::make_pair("403", "Forbidden"), std::make_pair("404", "Not Found"), std::make_pair("405", "Method Not Allowed"), 
		std::make_pair("406", "Not Acceptable"), std::make_pair("407", "Proxy Authentication Required"), 
		std::make_pair("408", "Request Timeout"), std::make_pair("409", "Conflict"), std::make_pair("410", "Gone"), 
		std::make_pair("411", "Length Required"), std::make_pair("412", "Precondition Failed"), std::make_pair("413", "Payload Too Large"), 
		std::make_pair("414", "URI Too Long"), std::make_pair("415", "Unsupported Media Type"), std::make_pair("416", "Range Not Satisfiable"), 
		std::make_pair("417", "Expectation Failed"), std::make_pair("418", "I'm a teapot"), std::make_pair("421", "Misdirected Request"), 
		std::make_pair("422", "Unprocessable Content"), std::make_pair("423", "Locked"), std::make_pair("424", "Failed Dependency"), 
		std::make_pair("425", "Too Early"), std::make_pair("426", "Upgrade Required"), std::make_pair("428", "Precondition Required"), 
		std::make_pair("429", "Too Many Requests"), std::make_pair("431", "Request Header Fields Too Large"), 
		std::make_pair("451", "Unavailable For Legal Reasons"), std::make_pair("500", "Internal Server Error"), 
		std::make_pair("501", "Not Implemented"), std::make_pair("502", "Bad Gateway"), std::make_pair("503", "Service Unavailable"), 
		std::make_pair("504", "Gateway Timeout"), std::make_pair("505", "HTTP Version Not Supported"), 
		std::make_pair("506", "Variant Also Negotiates"), std::make_pair("507", "Insufficient Storage"), std::make_pair("508", "Loop Detected"), 
		std::make_pair("510", "Not Extended"), std::make_pair("511", "Network Authentication Required")
	};

	#define ARRSIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
	for (size_t i = 0; i < ARRSIZE(responseStatusCodesArr); i++)
		_responseStatusCodes[responseStatusCodesArr[i].first] = responseStatusCodesArr[i].second;
	#undef  ARRSIZE
}

void ServerRouter::_contentTypesInit()
{
	_contentTypes.clear();
	std::pair<std::string, std::string> contentTypesArr[] =
	{
		std::make_pair("gif", "image/gif"), std::make_pair("jpg", "image/jpeg"), std::make_pair("jpeg", "image/jpeg"), 
		std::make_pair("png", "image/png"), std::make_pair("svg", "image/svg+xml"), std::make_pair("webp", "image/webp"), 
		std::make_pair("ico", "image/vnd.microsoft.icon"), std::make_pair("css", "text/css"), std::make_pair("csv", "text/csv"), 
		std::make_pair("html", "text/html"), std::make_pair("htm", "text/html"), std::make_pair("php", "text/php"), 
		std::make_pair("xml", "text/xml"), std::make_pair("pdf", "application/pdf"), std::make_pair("mp3", "audio/mpeg3"),
		std::make_pair("js", "application/x-javascript")


// std::make_pair(".3dm	x-world/x-3dmf
// std::make_pair(".3dmf	x-world/x-3dmf
// std::make_pair(".a	application/octet-stream
// std::make_pair(".aab	application/x-authorware-bin
// std::make_pair(".aam	application/x-authorware-map
// std::make_pair(".aas	application/x-authorware-seg
// std::make_pair(".abc	text/vnd.abc
// std::make_pair(".acgi	text/html
// std::make_pair(".afl	video/animaflex
// std::make_pair(".ai	application/postscript
// std::make_pair(".aif	audio/aiff
// std::make_pair(".aif	audio/x-aiff
// std::make_pair(".aifc	audio/aiff
// std::make_pair(".aifc	audio/x-aiff
// std::make_pair(".aiff	audio/aiff
// std::make_pair(".aiff	audio/x-aiff
// std::make_pair(".aim	application/x-aim
// std::make_pair(".aip	text/x-audiosoft-intra
// std::make_pair(".ani	application/x-navi-animation
// std::make_pair(".aos	application/x-nokia-9000-communicator-add-on-software
// std::make_pair(".aps	application/mime
// std::make_pair(".arc	application/octet-stream
// std::make_pair(".arj	application/arj
// std::make_pair(".arj	application/octet-stream
// std::make_pair(".art	image/x-jg
// std::make_pair(".asf	video/x-ms-asf
// std::make_pair(".asm	text/x-asm
// std::make_pair(".asp	text/asp
// std::make_pair(".asx	application/x-mplayer2
// std::make_pair(".asx	video/x-ms-asf
// std::make_pair(".asx	video/x-ms-asf-plugin
// std::make_pair(".au	audio/basic
// std::make_pair(".au	audio/x-au
// std::make_pair(".avi	application/x-troff-msvideo
// std::make_pair(".avi	video/avi
// std::make_pair(".avi	video/msvideo
// std::make_pair(".avi	video/x-msvideo
// std::make_pair(".avs	video/avs-video
// std::make_pair(".bcpio	application/x-bcpio
// std::make_pair(".bin	application/mac-binary
// std::make_pair(".bin	application/macbinary
// std::make_pair(".bin	application/octet-stream
// std::make_pair(".bin	application/x-binary
// std::make_pair(".bin	application/x-macbinary
// std::make_pair(".bm	image/bmp
// std::make_pair(".bmp	image/bmp
// std::make_pair(".bmp	image/x-windows-bmp
// std::make_pair(".boo	application/book
// std::make_pair(".book	application/book
// std::make_pair(".boz	application/x-bzip2
// std::make_pair(".bsh	application/x-bsh
// std::make_pair(".bz	application/x-bzip
// std::make_pair(".bz2	application/x-bzip2
// std::make_pair(".c	text/plain
// std::make_pair(".c	text/x-c
// std::make_pair(".c++	text/plain
// std::make_pair(".cat	application/vnd.ms-pki.seccat
// std::make_pair(".cc	text/plain
// std::make_pair(".cc	text/x-c
// std::make_pair(".ccad	application/clariscad
// std::make_pair(".cco	application/x-cocoa
// std::make_pair(".cdf	application/cdf
// std::make_pair(".cdf	application/x-cdf
// std::make_pair(".cdf	application/x-netcdf
// std::make_pair(".cer	application/pkix-cert
// std::make_pair(".cer	application/x-x509-ca-cert
// std::make_pair(".cha	application/x-chat
// std::make_pair(".chat	application/x-chat
// std::make_pair(".class	application/java
// std::make_pair(".class	application/java-byte-code
// std::make_pair(".class	application/x-java-class
// std::make_pair(".com	application/octet-stream
// std::make_pair(".com	text/plain
// std::make_pair(".conf	text/plain
// std::make_pair(".cpio	application/x-cpio
// std::make_pair(".cpp	text/x-c
// std::make_pair(".cpt	application/mac-compactpro
// std::make_pair(".cpt	application/x-compactpro
// std::make_pair(".cpt	application/x-cpt
// std::make_pair(".crl	application/pkcs-crl
// std::make_pair(".crl	application/pkix-crl
// std::make_pair(".crt	application/pkix-cert
// std::make_pair(".crt	application/x-x509-ca-cert
// std::make_pair(".crt	application/x-x509-user-cert
// .csh	application/x-csh
// .csh	text/x-script.csh
// .css	application/x-pointplus
// .css	text/css
// .cxx	text/plain
// .dcr	application/x-director
// .deepv	application/x-deepv
// .def	text/plain
// .der	application/x-x509-ca-cert
// .dif	video/x-dv
// .dir	application/x-director
// .dl	video/dl
// .dl	video/x-dl
// .doc	application/msword
// .dot	application/msword
// .dp	application/commonground
// .drw	application/drafting
// .dump	application/octet-stream
// .dv	video/x-dv
// .dvi	application/x-dvi
// .dwf	drawing/x-dwf (old)
// .dwf	model/vnd.dwf
// .dwg	application/acad
// .dwg	image/vnd.dwg
// .dwg	image/x-dwg
// .dxf	application/dxf
// .dxf	image/vnd.dwg
// .dxf	image/x-dwg
// .dxr	application/x-director
// .el	text/x-script.elisp
// .elc	application/x-bytecode.elisp (compiled elisp)
// .elc	application/x-elc
// .env	application/x-envoy
// .eps	application/postscript
// .es	application/x-esrehber
// .etx	text/x-setext
// .evy	application/envoy
// .evy	application/x-envoy
// .exe	application/octet-stream
// .f	text/plain
// .f	text/x-fortran
// .f77	text/x-fortran
// .f90	text/plain
// .f90	text/x-fortran
// .fdf	application/vnd.fdf
// .fif	application/fractals
// .fif	image/fif
// .fli	video/fli
// .fli	video/x-fli
// .flo	image/florian
// .flx	text/vnd.fmi.flexstor
// .fmf	video/x-atomic3d-feature
// .for	text/plain
// .for	text/x-fortran
// .fpx	image/vnd.fpx
// .fpx	image/vnd.net-fpx
// .frl	application/freeloader
// .funk	audio/make
// .g	text/plain
// .g3	image/g3fax
// .gif	image/gif
// .gl	video/gl
// .gl	video/x-gl
// .gsd	audio/x-gsm
// .gsm	audio/x-gsm
// .gsp	application/x-gsp
// .gss	application/x-gss
// .gtar	application/x-gtar
// .gz	application/x-compressed
// .gz	application/x-gzip
// .gzip	application/x-gzip
// .gzip	multipart/x-gzip
// .h	text/plain
// .h	text/x-h
// .hdf	application/x-hdf
// .help	application/x-helpfile
// .hgl	application/vnd.hp-hpgl
// .hh	text/plain
// .hh	text/x-h
// .hlb	text/x-script
// .hlp	application/hlp
// .hlp	application/x-helpfile
// .hlp	application/x-winhelp
// .hpg	application/vnd.hp-hpgl
// .hpgl	application/vnd.hp-hpgl
// .hqx	application/binhex
// .hqx	application/binhex4
// .hqx	application/mac-binhex
// .hqx	application/mac-binhex40
// .hqx	application/x-binhex40
// .hqx	application/x-mac-binhex40
// .hta	application/hta
// .htc	text/x-component
// .htm	text/html
// .html	text/html
// .htmls	text/html
// .htt	text/webviewhtml
// .htx	text/html
// .ice	x-conference/x-cooltalk
// .ico	image/x-icon
// .idc	text/plain
// .ief	image/ief
// .iefs	image/ief
// .iges	application/iges
// .iges	model/iges
// .igs	application/iges
// .igs	model/iges
// .ima	application/x-ima
// .imap	application/x-httpd-imap
// .inf	application/inf
// .ins	application/x-internett-signup
// .ip	application/x-ip2
// .isu	video/x-isvideo
// .it	audio/it
// .iv	application/x-inventor
// .ivr	i-world/i-vrml
// .ivy	application/x-livescreen
// .jam	audio/x-jam
// .jav	text/plain
// .jav	text/x-java-source
// .java	text/plain
// .java	text/x-java-source
// .jcm	application/x-java-commerce
// .jfif	image/jpeg
// .jfif	image/pjpeg
// .jfif-tbnl	image/jpeg
// .jpe	image/jpeg
// .jpe	image/pjpeg
// .jpeg	image/jpeg
// .jpeg	image/pjpeg
// .jpg	image/jpeg
// .jpg	image/pjpeg
// .jps	image/x-jps
// .js	application/x-javascript
// .js	application/javascript
// .js	application/ecmascript
// .js	text/javascript
// .js	text/ecmascript
// .jut	image/jutvision
// .kar	audio/midi
// .kar	music/x-karaoke
// .ksh	application/x-ksh
// .ksh	text/x-script.ksh
// .la	audio/nspaudio
// .la	audio/x-nspaudio
// .lam	audio/x-liveaudio
// .latex	application/x-latex
// .lha	application/lha
// .lha	application/octet-stream
// .lha	application/x-lha
// .lhx	application/octet-stream
// .list	text/plain
// .lma	audio/nspaudio
// .lma	audio/x-nspaudio
// .log	text/plain
// .lsp	application/x-lisp
// .lsp	text/x-script.lisp
// .lst	text/plain
// .lsx	text/x-la-asf
// .ltx	application/x-latex
// .lzh	application/octet-stream
// .lzh	application/x-lzh
// .lzx	application/lzx
// .lzx	application/octet-stream
// .lzx	application/x-lzx
// .m	text/plain
// .m	text/x-m
// .m1v	video/mpeg
// .m2a	audio/mpeg
// .m2v	video/mpeg
// .m3u	audio/x-mpequrl
// .man	application/x-troff-man
// .map	application/x-navimap
// .mar	text/plain
// .mbd	application/mbedlet
// .mc$	application/x-magic-cap-package-1.0
// .mcd	application/mcad
// .mcd	application/x-mathcad
// .mcf	image/vasa
// .mcf	text/mcf
// .mcp	application/netmc
// .me	application/x-troff-me
// .mht	message/rfc822
// .mhtml	message/rfc822
// .mid	application/x-midi
// .mid	audio/midi
// .mid	audio/x-mid
// .mid	audio/x-midi
// .mid	music/crescendo
// .mid	x-music/x-midi
// .midi	application/x-midi
// .midi	audio/midi
// .midi	audio/x-mid
// .midi	audio/x-midi
// .midi	music/crescendo
// .midi	x-music/x-midi
// .mif	application/x-frame
// .mif	application/x-mif
// .mime	message/rfc822
// .mime	www/mime
// .mjf	audio/x-vnd.audioexplosion.mjuicemediafile
// .mjpg	video/x-motion-jpeg
// .mm	application/base64
// .mm	application/x-meme
// .mme	application/base64
// .mod	audio/mod
// .mod	audio/x-mod
// .moov	video/quicktime
// .mov	video/quicktime
// .movie	video/x-sgi-movie
// .mp2	audio/mpeg
// .mp2	audio/x-mpeg
// .mp2	video/mpeg
// .mp2	video/x-mpeg
// .mp2	video/x-mpeq2a
// .mp3	audio/mpeg3
// .mp3	audio/x-mpeg-3
// .mp3	video/mpeg
// .mp3	video/x-mpeg
// .mpa	audio/mpeg
// .mpa	video/mpeg
// .mpc	application/x-project
// .mpe	video/mpeg
// .mpeg	video/mpeg
// .mpg	audio/mpeg
// .mpg	video/mpeg
// .mpga	audio/mpeg
// .mpp	application/vnd.ms-project
// .mpt	application/x-project
// .mpv	application/x-project
// .mpx	application/x-project
// .mrc	application/marc
// .ms	application/x-troff-ms
// .mv	video/x-sgi-movie
// .my	audio/make
// .mzz	application/x-vnd.audioexplosion.mzz
// .nap	image/naplps
// .naplps	image/naplps
// .nc	application/x-netcdf
// .ncm	application/vnd.nokia.configuration-message
// .nif	image/x-niff
// .niff	image/x-niff
// .nix	application/x-mix-transfer
// .nsc	application/x-conference
// .nvd	application/x-navidoc
// .o	application/octet-stream
// .oda	application/oda
// .omc	application/x-omc
// .omcd	application/x-omcdatamaker
// .omcr	application/x-omcregerator
// .p	text/x-pascal
// .p10	application/pkcs10
// .p10	application/x-pkcs10
// .p12	application/pkcs-12
// .p12	application/x-pkcs12
// .p7a	application/x-pkcs7-signature
// .p7c	application/pkcs7-mime
// .p7c	application/x-pkcs7-mime
// .p7m	application/pkcs7-mime
// .p7m	application/x-pkcs7-mime
// .p7r	application/x-pkcs7-certreqresp
// .p7s	application/pkcs7-signature
// .part	application/pro_eng
// .pas	text/pascal
// .pbm	image/x-portable-bitmap
// .pcl	application/vnd.hp-pcl
// .pcl	application/x-pcl
// .pct	image/x-pict
// .pcx	image/x-pcx
// .pdb	chemical/x-pdb
// .pdf	application/pdf
// .pfunk	audio/make
// .pfunk	audio/make.my.funk
// .pgm	image/x-portable-graymap
// .pgm	image/x-portable-greymap
// .pic	image/pict
// .pict	image/pict
// .pkg	application/x-newton-compatible-pkg
// .pko	application/vnd.ms-pki.pko
// .pl	text/plain
// .pl	text/x-script.perl
// .plx	application/x-pixclscript
// .pm	image/x-xpixmap
// .pm	text/x-script.perl-module
// .pm4	application/x-pagemaker
// .pm5	application/x-pagemaker
// .png	image/png
// .pnm	application/x-portable-anymap
// .pnm	image/x-portable-anymap
// .pot	application/mspowerpoint
// .pot	application/vnd.ms-powerpoint
// .pov	model/x-pov
// .ppa	application/vnd.ms-powerpoint
// .ppm	image/x-portable-pixmap
// .pps	application/mspowerpoint
// .pps	application/vnd.ms-powerpoint
// .ppt	application/mspowerpoint
// .ppt	application/powerpoint
// .ppt	application/vnd.ms-powerpoint
// .ppt	application/x-mspowerpoint
// .ppz	application/mspowerpoint
// .pre	application/x-freelance
// .prt	application/pro_eng
// .ps	application/postscript
// .psd	application/octet-stream
// .pvu	paleovu/x-pv
// .pwz	application/vnd.ms-powerpoint
// .py	text/x-script.phyton
// .pyc	application/x-bytecode.python
// .qcp	audio/vnd.qcelp
// .qd3	x-world/x-3dmf
// .qd3d	x-world/x-3dmf
// .qif	image/x-quicktime
// .qt	video/quicktime
// .qtc	video/x-qtc
// .qti	image/x-quicktime
// .qtif	image/x-quicktime
// .ra	audio/x-pn-realaudio
// .ra	audio/x-pn-realaudio-plugin
// .ra	audio/x-realaudio
// .ram	audio/x-pn-realaudio
// .ras	application/x-cmu-raster
// .ras	image/cmu-raster
// .ras	image/x-cmu-raster
// .rast	image/cmu-raster
// .rexx	text/x-script.rexx
// .rf	image/vnd.rn-realflash
// .rgb	image/x-rgb
// .rm	application/vnd.rn-realmedia
// .rm	audio/x-pn-realaudio
// .rmi	audio/mid
// .rmm	audio/x-pn-realaudio
// .rmp	audio/x-pn-realaudio
// .rmp	audio/x-pn-realaudio-plugin
// .rng	application/ringing-tones
// .rng	application/vnd.nokia.ringing-tone
// .rnx	application/vnd.rn-realplayer
// .roff	application/x-troff
// .rp	image/vnd.rn-realpix
// .rpm	audio/x-pn-realaudio-plugin
// .rt	text/richtext
// .rt	text/vnd.rn-realtext
// .rtf	application/rtf
// .rtf	application/x-rtf
// .rtf	text/richtext
// .rtx	application/rtf
// .rtx	text/richtext
// .rv	video/vnd.rn-realvideo
// .s	text/x-asm
// .s3m	audio/s3m
// .saveme	application/octet-stream
// .sbk	application/x-tbook
// .scm	application/x-lotusscreencam
// .scm	text/x-script.guile
// .scm	text/x-script.scheme
// .scm	video/x-scm
// .sdml	text/plain
// .sdp	application/sdp
// .sdp	application/x-sdp
// .sdr	application/sounder
// .sea	application/sea
// .sea	application/x-sea
// .set	application/set
// .sgm	text/sgml
// .sgm	text/x-sgml
// .sgml	text/sgml
// .sgml	text/x-sgml
// .sh	application/x-bsh
// .sh	application/x-sh
// .sh	application/x-shar
// .sh	text/x-script.sh
// .shar	application/x-bsh
// .shar	application/x-shar
// .shtml	text/html
// .shtml	text/x-server-parsed-html
// .sid	audio/x-psid
// .sit	application/x-sit
// .sit	application/x-stuffit
// .skd	application/x-koan
// .skm	application/x-koan
// .skp	application/x-koan
// .skt	application/x-koan
// .sl	application/x-seelogo
// .smi	application/smil
// .smil	application/smil
// .snd	audio/basic
// .snd	audio/x-adpcm
// .sol	application/solids
// .spc	application/x-pkcs7-certificates
// .spc	text/x-speech
// .spl	application/futuresplash
// .spr	application/x-sprite
// .sprite	application/x-sprite
// .src	application/x-wais-source
// .ssi	text/x-server-parsed-html
// .ssm	application/streamingmedia
// .sst	application/vnd.ms-pki.certstore
// .step	application/step
// .stl	application/sla
// .stl	application/vnd.ms-pki.stl
// .stl	application/x-navistyle
// .stp	application/step
// .sv4cpio	application/x-sv4cpio
// .sv4crc	application/x-sv4crc
// .svf	image/vnd.dwg
// .svf	image/x-dwg
// .svr	application/x-world
// .svr	x-world/x-svr
// .swf	application/x-shockwave-flash
// .t	application/x-troff
// .talk	text/x-speech
// .tar	application/x-tar
// .tbk	application/toolbook
// .tbk	application/x-tbook
// .tcl	application/x-tcl
// .tcl	text/x-script.tcl
// .tcsh	text/x-script.tcsh
// .tex	application/x-tex
// .texi	application/x-texinfo
// .texinfo	application/x-texinfo
// .text	application/plain
// .text	text/plain
// .tgz	application/gnutar
// .tgz	application/x-compressed
// .tif	image/tiff
// .tif	image/x-tiff
// .tiff	image/tiff
// .tiff	image/x-tiff
// .tr	application/x-troff
// .tsi	audio/tsp-audio
// .tsp	application/dsptype
// .tsp	audio/tsplayer
// .tsv	text/tab-separated-values
// .turbot	image/florian
// .txt	text/plain
// .uil	text/x-uil
// .uni	text/uri-list
// .unis	text/uri-list
// .unv	application/i-deas
// .uri	text/uri-list
// .uris	text/uri-list
// .ustar	application/x-ustar
// .ustar	multipart/x-ustar
// .uu	application/octet-stream
// .uu	text/x-uuencode
// .uue	text/x-uuencode
// .vcd	application/x-cdlink
// .vcs	text/x-vcalendar
// .vda	application/vda
// .vdo	video/vdo
// .vew	application/groupwise
// .viv	video/vivo
// .viv	video/vnd.vivo
// .vivo	video/vivo
// .vivo	video/vnd.vivo
// .vmd	application/vocaltec-media-desc
// .vmf	application/vocaltec-media-file
// .voc	audio/voc
// .voc	audio/x-voc
// .vos	video/vosaic
// .vox	audio/voxware
// .vqe	audio/x-twinvq-plugin
// .vqf	audio/x-twinvq
// .vql	audio/x-twinvq-plugin
// .vrml	application/x-vrml
// .vrml	model/vrml
// .vrml	x-world/x-vrml
// .vrt	x-world/x-vrt
// .vsd	application/x-visio
// .vst	application/x-visio
// .vsw	application/x-visio
// .w60	application/wordperfect6.0
// .w61	application/wordperfect6.1
// .w6w	application/msword
// .wav	audio/wav
// .wav	audio/x-wav
// .wb1	application/x-qpro
// .wbmp	image/vnd.wap.wbmp
// .web	application/vnd.xara
// .wiz	application/msword
// .wk1	application/x-123
// .wmf	windows/metafile
// .wml	text/vnd.wap.wml
// .wmlc	application/vnd.wap.wmlc
// .wmls	text/vnd.wap.wmlscript
// .wmlsc	application/vnd.wap.wmlscriptc
// .word	application/msword
// .wp	application/wordperfect
// .wp5	application/wordperfect
// .wp5	application/wordperfect6.0
// .wp6	application/wordperfect
// .wpd	application/wordperfect
// .wpd	application/x-wpwin
// .wq1	application/x-lotus
// .wri	application/mswrite
// .wri	application/x-wri
// .wrl	application/x-world
// .wrl	model/vrml
// .wrl	x-world/x-vrml
// .wrz	model/vrml
// .wrz	x-world/x-vrml
// .wsc	text/scriplet
// .wsrc	application/x-wais-source
// .wtk	application/x-wintalk
// .xbm	image/x-xbitmap
// .xbm	image/x-xbm
// .xbm	image/xbm
// .xdr	video/x-amt-demorun
// .xgz	xgl/drawing
// .xif	image/vnd.xiff
// .xl	application/excel
// .xla	application/excel
// .xla	application/x-excel
// .xla	application/x-msexcel
// .xlb	application/excel
// .xlb	application/vnd.ms-excel
// .xlb	application/x-excel
// .xlc	application/excel
// .xlc	application/vnd.ms-excel
// .xlc	application/x-excel
// .xld	application/excel
// .xld	application/x-excel
// .xlk	application/excel
// .xlk	application/x-excel
// .xll	application/excel
// .xll	application/vnd.ms-excel
// .xll	application/x-excel
// .xlm	application/excel
// .xlm	application/vnd.ms-excel
// .xlm	application/x-excel
// .xls	application/excel
// .xls	application/vnd.ms-excel
// .xls	application/x-excel
// .xls	application/x-msexcel
// .xlt	application/excel
// .xlt	application/x-excel
// .xlv	application/excel
// .xlv	application/x-excel
// .xlw	application/excel
// .xlw	application/vnd.ms-excel
// .xlw	application/x-excel
// .xlw	application/x-msexcel
// .xm	audio/xm
// .xml	application/xml
// .xml	text/xml
// .xmz	xgl/movie
// .xpix	application/x-vnd.ls-xpix
// .xpm	image/x-xpixmap
// .xpm	image/xpm
// .x-png	image/png
// .xsr	video/x-amt-showrun
// .xwd	image/x-xwd
// .xwd	image/x-xwindowdump
// .xyz	chemical/x-pdb
// .z	application/x-compress
// .z	application/x-compressed
// .zip	application/x-compressed
// .zip	application/x-zip-compressed
// .zip	application/zip
// .zip	multipart/x-zip
// .zoo	application/octet-stream
// .zsh	text/x-script.zsh


	};

	#define ARRSIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
	for (size_t i = 0; i < ARRSIZE(contentTypesArr); i++)
		_contentTypes[contentTypesArr[i].first] = contentTypesArr[i].second;
	#undef  ARRSIZE
}

void ServerRouter::_pollfdsInit()
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		_pollfds[i].fd = 0;
		_pollfds[i].events = 0;
		_pollfds[i].revents = 0;
	}
}

short ServerRouter::getEvents(int sd)
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd == sd)
			return _pollfds[i].events;
	}
	return -1;
}

short ServerRouter::getRevents(int sd)
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd == sd)
			return _pollfds[i].revents;
	}
	return -1;
}

std::vector<t_config> ServerRouter::getConfigs() const
	{ return _configs; }

std::string ServerRouter::getHostname() const
	{ return _hostname; }

std::vector<Server> ServerRouter::getServers() const
	{ return _servers; }

const pollfd * ServerRouter::getPollfds() const
	{ return  _pollfds; }

size_t ServerRouter::getPollfdsQty() const
	{ return _pollfdsQty; }

std::vector<t_connection> ServerRouter::getConnections() const
	{ return _connections; }

std::map<std::string, std::string> ServerRouter::getResponseStatusCodes() const
	{ return _responseStatusCodes; }

std::map<std::string, std::string> ServerRouter::getContentTypes() const
	{ return _contentTypes; }

Server ServerRouter::_getServer(int srvNbr) const
{
	for (std::vector<Server>::const_iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if ((*iter).getServNbr() == srvNbr)
			return *iter;
	}
	return _servers[0];
}


// fd_set ServerRouter::_getAllActiveSdSets()
// {
// 	fd_set allActiveSdSets;
// 	FD_ZERO(&allActiveSdSets);
// 	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
// 	{
// 		(*iter).sdSet(allActiveSdSets, _sdMaxCount);
// 	}
// 	return allActiveSdSets;
// }

