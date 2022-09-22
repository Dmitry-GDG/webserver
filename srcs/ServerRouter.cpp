#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs)
{
	_configs = configs;
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
}

ServerRouter::~ServerRouter() {}

std::vector<t_config> ServerRouter::getConfigs() const
	{ return _configs; }

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
		std::cerr << msg << std::endl;
		printMsgToLogFile(timestamp() + msg);
		return false;
	}
	// std::cout << "_sd = " << _sd << std::endl;

	// Set socket options
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		close (sd);
		msg = "Can not start a server: can not set socket options.";
		std::cerr << msg << std::endl;
		printMsgToLogFile(timestamp() + msg);
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
		std::cerr << msg << std::endl;
		printMsgToLogFile(timestamp() + msg);
		return false;
	}

	// Функция bind() задает порт или имя файла для привязки сокета
	if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not set the port or filename to bind the socket to.";
		std::cerr << msg << std::endl;
		printMsgToLogFile(timestamp() + msg);
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
		std::cerr << msg << std::endl;
		printMsgToLogFile(timestamp() + msg);
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

	printMsgToLogFile(timestamp() + "On sd " + std::to_string(sd) + " the '" + server.getConfig().serverName + "' server[" + std::to_string(server.getServNbr()) + "] started successfully and is listening on " + serverIp + ":" + std::to_string(ntohs(addr.sin_port)));

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
	
	printMsgToLogFile(timestamp() + "Webserver " + WEBSERV_NAME + " started at " + _hostname);

	int indx = 0;
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if (!_launch(*iter, indx))
		{
			msg = "Check server config file and try again.";
			printMsgToLogFile(timestamp() + msg);
			exitErr (msg);
		}
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
		{
			msg = "Poll error.";
			printMsgToLogFile(timestamp() + msg);
			exitErr (msg);
		}
		msg = "Poll error timeout.";
		printMsgToLogFile(timestamp() + msg);
		exitErr (msg);
	}
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].revents == 0)
			continue;
		if (_isSocketServer(_pollfds[i].fd))
		{
			int sd = accept(_pollfds[i].fd, (struct sockaddr *)&addrNew, &socklen);
			if (sd < 0)
			{
				if (errno != EWOULDBLOCK)
				{
					msg = "Accept error.";
					std::cerr << msg << std::endl;
					printMsgToLogFile(timestamp() + msg);
					return false;
				}
				break ;
			}
			_pollfds[_pollfdsQty].fd = sd;
			_pollfds[_pollfdsQty].events = POLLIN;
			_pollfdsQty++;
			_saveConnection(sd, i, inet_ntoa(((struct sockaddr_in*)&addrNew)->sin_addr), ntohs(((struct sockaddr_in*)&addrNew)->sin_port));
			msg = "new connection from " + _getConnection(sd)->fromIp + ":" + std::to_string(_getConnection(sd)->fromPort) + ", sd: ";
			printMsg(i, sd, msg, "");
			printMsgToLogFile(timestamp() + "server[" + std::to_string(i) + "]: " + msg + std::to_string(sd));
		}
		else if (_pollfds[i].revents == POLLIN) // есть данные для чтения
		{
			int clntSd = _pollfds[i].fd;
			t_connection * connection = _getConnection(clntSd);
			printMsg(connection->srvNbr, clntSd, "now is reading sd: ", "");
			printMsgToLogFile(timestamp() + "server[" + std::to_string(connection->srvNbr) + "]: now is reading sd " + std::to_string(clntSd));
			_readSd(connection);
			#ifdef DEBUGMODE
				printConnection(connection, "DEBUGMODE _mainLoop printConnection");
			#endif


			// _pollfds[i].revents = 0;
		}
		else if (_pollfds[i].revents == POLLOUT) // запись возможна
		{

		}
		// else if (_pollfds[i].revents != POLLIN && _pollfds[i].revents != POLLOUT)
		// {
		// 	int clntSd = _pollfds[i].fd;
		// 	t_connection * connection = _getConnection(clntSd);
		// 	printMsg(connection->srvNbr, clntSd, "client closed sd: ", "");
		// 	close (_pollfds[i].fd);
		// 	_removeSdFromPollfds(i);
		// 	_removeConnection(clntSd);
		// }
		std::cout << "Hi!" << std::endl;
	}
	return true;
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
	// std::string err;

	int qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
	std::cout << "server[" << connection->srvNbr << "]: " << qtyBytes << " bytes read" << std::endl;
	if (qtyBytes == 0)
	{
		connection->status = READ_DONE;
		std::cout << "less 0" << std::endl;
		return qtyBytes;
	}
	else if (qtyBytes > 0)
	{
		buf[qtyBytes] = '\0';
		if (!parseInputData(buf, connection))
		{
			while (qtyBytes > 0)
				qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
			connection->status = READ_DONE;
			// if (qtyBytes < 0)
			// 	std::cout << "less 0" << std::endl;
			return 0;
		}




	}
	// else if (qtyBytes < 0)
	// 	std::cout << "less 0" << std::endl;
	return qtyBytes;
}

// void ServerRouter::_parseInputData(char * buf, t_connection * connection)
// {
// 	std::string inpt = buf;
// 	#ifdef DEBUGMODE
// 		std::cout << "Input: " << inpt << ", size: " << inpt.size() << std::endl;
// 	#endif
// 	std::string inptLine = inpt.substr(0, inpt.find('\n', 0));
// 	if (inptLine.find("HTTP") != std::string::npos)
// 	{
// 		connection->inputdata.dataType = HEADERS;
// 		size_t first = 0;
// 		size_t second = inptLine.find(' ', first);
// 		connection->inputdata.method = inptLine.substr(first, second);

// 	}
// }

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
	connection.methods.push_back("GET");
	connection.methods.push_back("POST");
	connection.methods.push_back("DELETE");
	_connections.push_back(connection);
}


// void ServerRouter::_saveConnection(int sdFrom, int srvNbr, std::string fromIP, unsigned fromPort)
// {
// 	_connections[sdFrom].srvNbr = srvNbr;
// 	_connections[sdFrom].position = 0;
// 	_connections[sdFrom].status = READ;
// 	_connections[sdFrom].fromIp = fromIP;
// 	_connections[sdFrom].fromPort = fromPort;
// }

// void ServerRouter::launch()
// {
// 	fd_set allActiveSdSets, readActiveSdSets, writeActiveSdSets;
// 	int	sd;

// 	std::cout << NC << timestamp() << YELLOS << NAME << ": Welcome to the " << WEBSERV_NAME << " v."<< WEBSERV_VERSION << " by " \
// 	<< WEBSERV_AUTHORS << "\nIt was tested on MAC OS.\n" \
// 	<< "It was launched at " << NC << _hostname << "\n";
// 	// std::cout << "\nTo connect via this Mac use " \
// 	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
// 	// << NC << _port << YELLOS << " and password: " << NC << _password \
// 	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
// 	// << NC << _hostname << YELLOS << ", port: " \
// 	// << NC << _port << YELLOS << " and password: " << NC << _password \
// 	// << std::endl;

// 	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
// 	{
// 		if (!(*iter).start())
// 			exitErr ("Check server config file and try again.");
// 	}

// 	FD_ZERO(&allActiveSdSets);
// 	FD_ZERO(&readActiveSdSets);
// 	FD_ZERO(&writeActiveSdSets);

// 	while (21)
// 	{
// 		allActiveSdSets = _getAllActiveSdSets();
// 		readActiveSdSets = allActiveSdSets;
// 		writeActiveSdSets = allActiveSdSets;

// 		sd = select(_sdMaxCount + 1, &readActiveSdSets, &writeActiveSdSets, 0, 0);
// 		if (sd < 0)
// 			continue;
// 		for (int i = 0; i < _sdMaxCount && sd > 0; i++)
// 		{
// 			if (FD_ISSET(i, &readActiveSdSets))
// 			{
// 				#ifdef DEBUGMODE
// 					std::cout << "DEBUG launch READ sd: " << i <<  std::endl;
// 				#endif
// 				for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
// 				{
// 					if ((*iter).readSd(i))
// 						break;
// 				}
// 				sd--;
// 			}
// 			else if (FD_ISSET(i, &writeActiveSdSets))
// 			{
// 				#ifdef DEBUGMODE
// 					std::cout << "DEBUG launch WRITE sd: " << i << std::endl;
// 				#endif
// 				sd--;
// 			}
// 		}
// 	}
// 	std::cout << "By!" << std::endl;
// 	exit (0);
// }

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

