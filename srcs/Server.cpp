#include "Server.hpp"

Server::Server(t_config config)
{
	_config = config;
	_colors[0] = GREENS;
	_colors[1] = BLUS;
	_colors[2] = VIOLETS;
	_colors[3] = AQUAMARINES;
	_nextColor = 0;
	// char hostname[HOSTNAME_LENGTH];
	// _hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
	_sdMaxCount = SD_MAXCOUNT;
}

Server::~Server() {}

t_config Server::getConfig() const
	{ return _config; }

bool Server::start()
{
	if (!_createSocket())
	{
		std::cerr << "Can not start server" << std::endl;
		return false;
	}

	std::cout << NC << timestamp() << YELLOS << "The server " << NC << _config.serverName << YELLOS << " was started sacsessfully.\n" << NC;
	// std::cout << "\nTo connect via this Mac use " \
	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
	// << NC << _hostname << YELLOS << ", port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << std::endl;

	return (true);
}

bool Server::_createSocket()
{
	int const options = 1;
	struct sockaddr_in addr;

	// Create socket
	if ((_sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Can not create socket" << std::endl;
		return false;
	}

	// Set socket options
	if (setsockopt(_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&options, sizeof(options)) < 0)
	{
		close (_sd);
		std::cerr << "Can not set socket options" << std::endl;
		return false;
	}

	// преобразуем адрес IPv4 или IPv6 из символьного представления в двоичную форму с сетевым порядком следования байтов
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atou16(_config.port.c_str()));
	if (inet_pton(addr.sin_family, _config.listen.c_str(), &(addr.sin_addr)) < 0)
	{
		close(_sd);
		std::cerr << "Can not convert listen address" << std::endl;
		return false;
	}

	// Функция bind() задает порт или имя файла для привязки сокета
	bind(_sd, (struct sockaddr*)&addr, sizeof(addr));
	// if (bind(_sd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	// {
	// 	close(_sd);
	// 	std::cerr << "Can not set the port or filename to bind the socket to" << std::endl;
	// 	return false;
	// }

	// man listen
	// The backlog parameter defines the maximum length for the queue of pending connections. 
	// If a connection request arrives with the queue full, the client may receive an error with an indication of ECONNREFUSED.  
	// Alternatively, if the underlying protocol supports retransmission, the request may be ignored so that retries may succeed.
	if (listen(_sd, _sdMaxCount) < 0)
	{
		close(_sd);
		std::cerr << "Can not listen queue" << std::endl;
		return false;
	}

	char str[sizeof(addr)];
	inet_ntop(AF_INET, &(addr.sin_addr), str, sizeof(addr));
	std::cout << "webserver: " << str << ":" << ntohs(addr.sin_port) << " ";
	return true;
}
