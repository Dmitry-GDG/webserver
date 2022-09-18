#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs)
{
	_configs = configs;
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";
	_sdMaxCount = -1;
	_connections.clear();
	// # ifdef DEBUGMODE
	// 	printAllServersConfig(_configs, "DEBUG ServerRouter AllServersConfig");
	// # endif
	for (std::vector<t_config>::iterator iter = _configs.begin(); iter < _configs.end(); iter++)
		_servers.push_back(Server(*iter));
	// # ifdef DEBUGMODE
	// 	printAllServersVector(_servers, "DEBUG ServerRouter AllServersVector");
	// # endif
}

ServerRouter::~ServerRouter() {}

std::vector<t_config> ServerRouter::getConfigs() const
	{ return _configs; }

void ServerRouter::launch()
{
	fd_set allActiveSdSets, readActiveSdSets, writeActiveSdSets;
	int	sd;

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
		if (!(*iter).start())
			exitErr ("Check server config file and try again.");
	}

	FD_ZERO(&allActiveSdSets);
	FD_ZERO(&readActiveSdSets);
	FD_ZERO(&writeActiveSdSets);

	while (21)
	{
		allActiveSdSets = _getAllActiveSdSets();
		readActiveSdSets = allActiveSdSets;
		writeActiveSdSets = allActiveSdSets;

		sd = select(_sdMaxCount + 1, &readActiveSdSets, &writeActiveSdSets, 0, 0);
		if (sd < 0)
			continue;
		for (int i = 0; i < _sdMaxCount && sd > 0; i++)
		{
			if (FD_ISSET(i, &readActiveSdSets))
			{
				# ifdef DEBUGMODE
					std::cout << "DEBUG launch READ sd: " << i <<  std::endl;
				# endif
				for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
				{
					if ((*iter).readSd(i))
						break;
				}
				sd--;
			}
			else if (FD_ISSET(i, &writeActiveSdSets))
			{
				# ifdef DEBUGMODE
					std::cout << "DEBUG launch WRITE sd: " << i << std::endl;
				# endif
				sd--;
			}
		}
	}
	std::cout << "By!" << std::endl;
	exit (0);
}

fd_set ServerRouter::_getAllActiveSdSets()
{
	fd_set allActiveSdSets;
	FD_ZERO(&allActiveSdSets);
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		(*iter).sdSet(allActiveSdSets, _sdMaxCount);
	}
	return allActiveSdSets;
}

