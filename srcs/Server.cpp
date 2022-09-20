#include "Server.hpp"

Server::Server(t_config config)
{
	_config = config;
	// _connections.clear();
	_colors[0] = GREENS;
	_colors[1] = BLUS;
	_colors[2] = VIOLETS;
	_colors[3] = AQUAMARINES;
	_nextColor = 0;
	// _sdMaxCount = SD_MAXCOUNT;
	// char hostname[HOSTNAME_LENGTH];
	// _hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
}

Server & Server::operator=(const Server & server)
{
	_config = server._config;
	// _connections = server._connections;
	_colors[0] = server._colors[0];
	_colors[1] = server._colors[1];
	_colors[2] = server._colors[2];
	_colors[3] = server._colors[3];
	_nextColor = server._nextColor;
	// _sdMaxCount = server._sdMaxCount;
	return *this;
}

Server::~Server() {}

const t_config & Server::getConfig() const
	{ return _config; }

int Server::getSd() const
	{ return _sd; }

void Server::setSd(int sd)
	{ _sd = sd; }

size_t Server::getServNbr() const
	{ return _srvrNbr; }

void Server::setSrvrNbr(size_t indx)
	{ _srvrNbr = indx; }

// bool Server::start()
// {
// 	int on = 1;
// 	struct sockaddr_in addr;

// 	// Create socket
// 	if ((_sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
// 	{
// 		std::cerr << "Can not start a server: can not create socket." << std::endl;
// 		return false;
// 	}
// 	// std::cout << "_sd = " << _sd << std::endl;

// 	// Set socket options
// 	if (setsockopt(_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
// 	{
// 		close (_sd);
// 		std::cerr << "Can not start a server: can not set socket options." << std::endl;
// 		return false;
// 	}

// 	// преобразуем адрес IPv4 или IPv6 из символьного представления в двоичную форму с сетевым порядком следования байтов
// 	addr.sin_family = AF_INET;
// 	addr.sin_port = htons(atou16(_config.port.c_str()));
// 	if (inet_pton(addr.sin_family, _config.ip.c_str(), &(addr.sin_addr)) < 0)
// 	{
// 		close(_sd);
// 		std::cerr << "Can not start a server: can not convert listen address." << std::endl;
// 		return false;
// 	}

// 	// Функция bind() задает порт или имя файла для привязки сокета
// 	if (bind(_sd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
// 	{
// 		close(_sd);
// 		std::cerr << "Can not start a server: can not set the port or filename to bind the socket to." << std::endl;
// 		return false;
// 	}

// 	// man listen
// 	// The backlog parameter defines the maximum length for the queue of pending connections. 
// 	// If a connection request arrives with the queue full, the client may receive an error with an indication of ECONNREFUSED.  
// 	// Alternatively, if the underlying protocol supports retransmission, the request may be ignored so that retries may succeed.
// 	if (listen(_sd, _sdMaxCount) < 0)
// 	{
// 		close(_sd);
// 		std::cerr << "Can not start a server: can not listen queue." << std::endl;
// 		return false;
// 	}

// 	inet_ntop(AF_INET, &(addr.sin_addr), _serverIp, sizeof(addr));

// 	std::cout << NC << timestamp() << YELLOS << "On socket descriptor " << NC << _sd \
// 	<< YELLOS << " the " << NC << _config.serverName \
// 	<< YELLOS << " server started successfully and is listening on " << NC \
// 	<< _serverIp << ":" << ntohs(addr.sin_port) << std::endl;
// 	// std::cout << "\nTo connect via this Mac use " \
// 	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
// 	// << NC << _port << YELLOS << " and password: " << NC << _password \
// 	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
// 	// << NC << _hostname << YELLOS << ", port: " \
// 	// << NC << _port << YELLOS << " and password: " << NC << _password \
// 	// << std::endl;

// 	return (true);
// }

// void Server::sdSet(fd_set & allActiveSdSets, int & sdMaxCountRouter)
// {
// 	FD_SET(_sd, & allActiveSdSets);
// 	sdMaxCountRouter = _sd > sdMaxCountRouter ? _sd : sdMaxCountRouter;
// 	std::vector<int> allSds = _getAllSds();
// 	for (std::vector<int>::iterator iter = allSds.begin(); iter < allSds.end(); iter++)
// 	{
// 		FD_SET(*iter, & allActiveSdSets);
// 		sdMaxCountRouter = *iter > sdMaxCountRouter ? *iter : sdMaxCountRouter;
// 	}
// }

// std::vector<int> Server::_getAllSds() const
// {
// 	std::vector<int> allSds;
// 	allSds.clear();
// 	for (std::vector<Connection>::const_iterator iter = _connections.begin(); iter < _connections.end(); iter++)
// 		allSds.push_back((*iter).getSd());
// 	return allSds;
// }

// bool Server::_foundSd(int sd) const
// {
// 	for (std::vector<Connection>::const_iterator iter = _connections.begin(); iter < _connections.end(); iter++)
// 	{
// 		if ((*iter).getSd() == sd)
// 			return true;
// 	}
// 	return false;
// }

// bool Server::readSd(int sd)
// {
// 	if (sd != _sd && !_foundSd(sd))
// 		return false;
// 	if (sd == _sd)
// 	{
// 		struct sockaddr_in newAddr;
// 		socklen_t newAddrSize = sizeof(newAddr);
// 		int newSd = accept (_sd, (sockaddr *)&newAddr, &newAddrSize);
// 		if (newSd < 0)
// 			return false;
// 		if (_connections.size() < _sdMaxCount)
// 		{
// 			fcntl(newSd, F_SETFL, O_NONBLOCK);
// 			_connections.push_back(Connection(newSd));
// 		}
// 		else
// 			close(newSd);
// 	}
// 	// else
// 	// {
// 	// 	// exitErr ("By!");
// 	// 	if (_foundSd(sd))
// 	// }
// 	return true;
// }

