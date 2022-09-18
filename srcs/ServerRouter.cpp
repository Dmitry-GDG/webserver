#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs)
{
	_configs = configs;
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
	_pollfds.clear();

	// # ifdef DEBUGMODE
	// 	printAllServersConfig(_configs, "DEBUG ServerRouter AllServersConfig");
	// # endif
	for (std::vector<t_config>::iterator iter = _configs.begin(); iter < _configs.end(); iter++)
		_servers.push_back(Server(*iter));
	// # ifdef DEBUGMODE
	// 	printAllServersVector(_servers, "DEBUG ServerRouter AllServersVector");
	// # endif
	_sdMaxCount = _servers.size();
}

ServerRouter::~ServerRouter() {}

std::vector<t_config> ServerRouter::getConfigs() const
	{ return _configs; }

bool ServerRouter::_launch(Server & server)
{
	int on = 1;
	struct sockaddr_in addr;
	int sd;
	struct pollfd pfd;

	// Create socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Can not start a server: can not create socket." << std::endl;
		return false;
	}
	// std::cout << "_sd = " << _sd << std::endl;

	// Set socket options
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		close (sd);
		std::cerr << "Can not start a server: can not set socket options." << std::endl;
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
		std::cerr << "Can not start a server: can not convert listen address." << std::endl;
		return false;
	}

	// Функция bind() задает порт или имя файла для привязки сокета
	if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(sd);
		std::cerr << "Can not start a server: can not set the port or filename to bind the socket to." << std::endl;
		return false;
	}

	// man listen
	// The backlog parameter defines the maximum length for the queue of pending connections. 
	// If a connection request arrives with the queue full, the client may receive an error with an indication of ECONNREFUSED.  
	// Alternatively, if the underlying protocol supports retransmission, the request may be ignored so that retries may succeed.
	if (listen(sd, SOMAXCONN) < 0)
	{
		close(sd);
		std::cerr << "Can not start a server: can not listen queue." << std::endl;
		return false;
	}

	server.setSd(sd);
	pfd.fd = sd;
	pfd.events = POLLIN;
	_pollfds.push_back(pfd);

	char serverIp[sizeof(struct sockaddr_in)];
	inet_ntop(AF_INET, &(addr.sin_addr), serverIp, sizeof(addr));

	std::cout << NC << timestamp() << YELLOS << "On socket descriptor " << NC << sd \
	<< YELLOS << " the " << NC << server.getConfig().serverName \
	<< YELLOS << " server started successfully and is listening on " << NC \
	<< serverIp << ":" << ntohs(addr.sin_port) << std::endl;
	// std::cout << "\nTo connect via this Mac use " \
	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
	// << NC << _hostname << YELLOS << ", port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << std::endl;

	return true;
}

void ServerRouter::start()
{
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

	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if (!_launch((*iter)))
			exitErr ("Check server config file and try again.");
	}

	# ifdef DEBUGMODE
	printPollfds(_pollfds, "DEBUG _pollfds");
	# endif

	while (42)
	{
		if (!_mainLoop())
			break;
	}
	for (std::vector<struct pollfd>::iterator iter = _pollfds.begin(); iter < _pollfds.end(); iter++)
	{
		if ((*iter).fd >= 0)
			close ((*iter).fd);
	}
}

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
// 				# ifdef DEBUGMODE
// 					std::cout << "DEBUG launch READ sd: " << i <<  std::endl;
// 				# endif
// 				for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
// 				{
// 					if ((*iter).readSd(i))
// 						break;
// 				}
// 				sd--;
// 			}
// 			else if (FD_ISSET(i, &writeActiveSdSets))
// 			{
// 				# ifdef DEBUGMODE
// 					std::cout << "DEBUG launch WRITE sd: " << i << std::endl;
// 				# endif
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

