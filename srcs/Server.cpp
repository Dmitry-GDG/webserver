#include "Server.hpp"

Server::Server(t_config config)
{
	_config = config;
}

Server::~Server() {}

t_config Server::getConfig() const
	{ return _config; }

bool Server::start()
{
	std::cout << "server-start" << std::endl;
	return false;
}
