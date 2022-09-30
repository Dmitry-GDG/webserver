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
	#ifdef DEBUGMODE
		printAllServersVector(_servers, "DEBUG ServerRouter AllServersVector");
	#endif
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
				std::cout << "_pollfds[i].revents & POLLIN" << std::endl;
				// _setConnectionLastActivity(connection->lastActivityTime);
				_readSd(connection);

				// #ifdef DEBUGMODE
				// 	printConnection(* connection, "DEBUGMODE _mainLoop printConnection 1", 1);
				// #endif

				if (connection->requestProcessingStep == READ_DONE)
					connection->pfd->events = POLLOUT;

				#ifdef DEBUGMODE
					printConnection(* connection, "DEBUGMODE _mainLoop printConnection 2", 1);
				#endif

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


	#ifdef DEBUGMODE
		std::cout << "**** DEBUGMODE ServerRouter//_sendanswer answer ****\nAnswer:\n" << connection->responseData.connectionAnswer << std::endl;
	#endif

	// if (!connection->responseData.lenAnswer)
	// 	connection->responseData.lenAnswer = connection->responseData.connectionAnswer.str().length();
	// int sizePacket, bytesSent;

	// sizePacket = (connection->responseData.lenAnswer - connection->responseData.lenSent) > BUF_SIZE ? BUF_SIZE : (connection->responseData.lenAnswer - connection->responseData.lenSent);

	// connection->responseData.connectionAnswer.rdbuf()->sgetn(connection->responseData.buf, sizePacket);
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
	if (connection->responseData.lenAnswer <= connection->responseData.lenSent)
		connection->requestProcessingStep = WRITE_DONE;
	// _setConnectionLastActivity(connection->lastActivityTime);
	return bytesSent; //?
}

void ServerRouter::_prepareGetAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = GET;
	Server server = _getServer(connection->srvNbr);
	bool correctAddr = false;
	if (connection->inputData.address != "\\")
	{
		for (size_t i = 0; i < server.getConfig().locations.size(); i++)
		{
			if (connection->inputData.address == server.getConfig().locations[i].path)
				correctAddr = true;
		}
	}
	if (!correctAddr)
	{
		msg = "Error! Unknown location addr from sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
	}

	std::string contentTypeAndLength = "";
	_addFileToAnswer(contentTypeAndLength, connection);

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

void ServerRouter::_addFileToAnswer(std::string & contentTypeAndLength, t_connection * connection)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	// std::string path = server.getConfig().listen + connection->inputdata.address;
	std::string path = "." + connection->inputData.address;

	size_t i;
	if (server.getConfig().root != "")
		path += server.getConfig().root;
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		if (connection->inputData.address == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				path += server.getConfig().locations[i].root;
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
	FILE * file = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (!S_ISREG(buf.st_mode) || file == NULL)
	{
		msg = "Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
	}
	else
	{
		msg = "The file " + path + " was sucsessfully opened, sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "200";

		fseek(file, 0L, SEEK_END);
		int fileLength = ftell(file);

		int dot = path.find(".");
		std::string ext = path.substr(dot + 1, path.length() - dot);
		std::string contType;
		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
			contType = connection->contentTypesAll[ext];
		else
			contType = "text/html";

		contentTypeAndLength = "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER;
		// std::stringstream bufFile;
		// bufFile << file.rdbuf();
		fclose(file);
	}
}

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
		std::make_pair("xml", "text/xml"), std::make_pair("pdf", "application/pdf")
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

