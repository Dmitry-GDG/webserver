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
				// _sendAnswer(connection);
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

// int ServerRouter::_sendAnswer(t_connection * connection)
// {
// 	std::string answer = 

// }

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
	connection.methods.push_back("GET");
	connection.methods.push_back("POST");
	connection.methods.push_back("DELETE");
	_connections.push_back(connection);
}

void ServerRouter::_responseStatusCodesInit()
{
	_responseStatusCodes.clear();
	// _responseStatusCodes["100"] = "Continue";
	std::vector<std::pair<std::string, std::string> > vr = {{"100", "Continue"}, {"101", "Switching Protocols"}, {"102", "Processing"}, {"103", "Early Hints"}, {"200", "OK"}, {"201", "Created"}, {"202", "Accepted"}, {"203", "Non-Authoritative Information"}, {"204", "No Content"}, {"205", "Reset Content"}, {"206", "Partial Content"}, {"207", "Multi-Status"}, {"208", "Already Reported"}, {"226", "IM Used"}, {"300", "Multiple Choices"}, {"301", "Moved Permanently"}, {"302", "Found"}, {"303", "See Other"}, {"304", "Not Modified"}, {"305", "Use Proxy"}, {"307", "Temporary Redirect"}, {"308", "Permanent Redirect"}, {"400", "Bad Request"}, {"401", "Unauthorized"}, {"402", "Payment Required"}, {"403", "Forbidden"}, {"404", "Not Found"}, {"405", "Method Not Allowed"}, {"406", "Not Acceptable"}, {"407", "Proxy Authentication Required"}, {"408", "Request Timeout"}, {"409", "Conflict"}, {"410", "Gone"}, {"411", "Length Required"}, {"412", "Precondition Failed"}, {"413", "Payload Too Large"}, {"414", "URI Too Long"}, {"415", "Unsupported Media Type"}, {"416", "Range Not Satisfiable"}, {"417", "Expectation Failed"}, {"418", "I'm a teapot"}, {"421", "Misdirected Request"}, {"422", "Unprocessable Content"}, {"423", "Locked"}, {"424", "Failed Dependency"}, {"425", "Too Early"}, {"426", "Upgrade Required"}, {"428", "Precondition Required"}, {"429", "Too Many Requests"}, {"431", "Request Header Fields Too Large"}, {"451", "Unavailable For Legal Reasons"}, {"500", "Internal Server Error"}, {"501", "Not Implemented"}, {"502", "Bad Gateway"}, {"503", "Service Unavailable"}, {"504", "Gateway Timeout"}, {"505", "HTTP Version Not Supported"}, {"506", "Variant Also Negotiates"}, {"507", "Insufficient Storage"}, {"508", "Loop Detected"}, {"510", "Not Extended"}, {"511", "Network Authentication Required"}};
	// std::cout << "!!!!! vr[0].first = " <<  vr[0].first << ",  vr[0].second = " <<  vr[0].second << std::endl;
	// for (size_t i = 0; i < 100; i++)
	// 	_responseStatusCodes[vr[i].first] = vr[i].second;
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

