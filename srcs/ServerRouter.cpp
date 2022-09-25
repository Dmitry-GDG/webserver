#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs)
{
	_configs = configs;
	_responseStatusCodesInit();
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
	// _pollfds.clear();

	// #ifdef DEBUGMODE
	// 	printAllServersConfig(_configs, "DEBUG ServerRouter AllServersConfig");
	// #endif
	for (std::vector<t_config>::iterator iter = _configs.begin(); iter < _configs.end(); iter++)
		_servers.push_back(Server(*iter));
	// #ifdef DEBUGMODE
	// 	printAllServersVector(_servers, "DEBUG ServerRouter AllServersVector");
	// #endif
	_pollfdsQty = _servers.size();
	_pollfdsInit();
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
			break;
	}

	_closeSockets();
}

bool ServerRouter::_mainLoop()
{
	// int sd = -1;
	socklen_t socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in addrNew;
	std::string msg;

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
		if (_pollfds[i].revents == 0)
			continue;
		if (_isSocketServer(_pollfds[i].fd))
		{
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
			int clntSd = _pollfds[i].fd;
			t_connection * connection = _getConnection(clntSd);
			connection->pfd = &(_pollfds[i]);
			char buf[BUF_SIZE + 1];
			if (_pollfds[i].revents & POLLIN) // есть данные для чтения
			{
				_readSd(connection);

				#ifdef DEBUGMODE
					printConnection(* connection, "DEBUGMODE _mainLoop printConnection", 1);
				#endif



				if (connection->status == READ_DONE)
					connection->pfd->events = POLLOUT;


				// _pollfds[i].revents = 0;
			}
			else if (_pollfds[i].revents & POLLOUT) // запись возможна
			{
				std::cout << "POLOUT" << std::endl;
				_sendAnswer(connection);
				if (connection->status == WRITE_DONE)
				{
					connection->pfd->events = POLLIN;
					connection->inputStr = "";
				}
			}
			else if (recv(clntSd, buf, BUF_SIZE, MSG_PEEK) == 0)
			{
				msg = "client closed sd ";
				printMsg(connection->srvNbr, clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, clntSd, msg, "");
				close (_pollfds[i].fd);
				_removeConnection(clntSd);
				_removeSdFromPollfds(i);
			}
			std::cout << "Hi!" << std::endl;
		}
	}
	return true;
}

int ServerRouter::_sendAnswer(t_connection * connection)
{
	std::string delim = DELIMETER;
	std::string answer = connection->inputdata.httpVersion + " ";

	return 0; //?
}

void ServerRouter::_removeSdFromPollfds(int indx)
{
	_pollfds[indx].fd = -1;
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd < 0)
		{
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

int ServerRouter::_readSd(t_connection * connection)
{
	char buf[BUF_SIZE + 1];
	std::string msg;

	int qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
	if (qtyBytes == 0)
	{
		msg = "finished reading data from sd ";
		printMsg(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->status = READ_DONE;
		return qtyBytes;
	}
	else if (qtyBytes > 0)
	{
		buf[qtyBytes] = '\0';
		connection->inputStr += buf;
		msg = "got " + std::to_string(qtyBytes) + " bytes from sd ";
		printMsg(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		if (!parseInputData(buf, connection))
		{
			while (qtyBytes > 0)
				qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
			connection->status = READ_DONE;
			return 0;
		}




	}
	return qtyBytes;
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
	connection.clntSd = sdFrom;
	connection.status = READ;
	connection.fromIp = fromIP;
	connection.fromPort = fromPort;
	connection.inputStr.clear();
	connection.responseStatusCodes = _responseStatusCodes;
	connection.methods.push_back("GET");
	connection.methods.push_back("POST");
	connection.methods.push_back("DELETE");
	_connections.push_back(connection);
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

