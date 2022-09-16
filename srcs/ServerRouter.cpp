#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs)
{
	_configs = configs;
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
	_sdMaxCount = -1;
	// printAllServersConfig(_configs);
	for (std::vector<t_config>::iterator iter = _configs.begin(); iter < _configs.end(); iter++)
		_servers.push_back(Server(*iter));
	printAllServersVector(_servers);
}

ServerRouter::~ServerRouter() {}

std::vector<t_config> ServerRouter::getConfigs() const
	{ return _configs; }

void ServerRouter::launch()
{
	fd_set allSds, readSds, writeSds;

	std::cout << NC << timestamp() << YELLOS << NAME << ": Welcome to the " << WEBSERV_NAME << " v."<< WEBSERV_VERSION << " by " \
	<< WEBSERV_AUTHORS << "\nIt was tested on MAC OS.\n" \
	<< "It was launched at " << _hostname << "\n" << NC;
	// std::cout << "\nTo connect via this Mac use " \
	// << NC << "localhost" << YELLOS << " (or 127.0.0.1), port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << YELLOS << "\nTo connect via another Mac in school's local network use " \
	// << NC << _hostname << YELLOS << ", port: " \
	// << NC << _port << YELLOS << " and password: " << NC << _password \
	// << std::endl;

	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if (!(*iter).start())
			exitErr ("Check server config file and try again.");
	}

	FD_ZERO(&allSds);
	FD_ZERO(&readSds);
	FD_ZERO(&writeSds);

	// while (21)
	// {
	// 	allSds = _getAllSds();
	// 	readSds = allSds;
	// 	writeSds = allSds;
	// }
	
}

fd_set ServerRouter::_getAllSds()
{
	fd_set allSds;
	FD_ZERO(&allSds);
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
		(*iter).sdSet(allSds, _sdMaxCount);
	return allSds;
}

