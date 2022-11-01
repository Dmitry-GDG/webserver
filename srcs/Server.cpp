#include "Server.hpp"

Server::Server(t_config config)
{
	_config = config;
	_colors[0] = GREENS;
	_colors[1] = BLUS;
	_colors[2] = VIOLETS;
	_colors[3] = AQUAMARINES;
	_nextColor = 0;
	_port = 0;
}

Server & Server::operator=(const Server & server)
{
	_config = server._config;
	_colors[0] = server._colors[0];
	_colors[1] = server._colors[1];
	_colors[2] = server._colors[2];
	_colors[3] = server._colors[3];
	_nextColor = server._nextColor;
	return *this;
}

Server::~Server() {}

const t_config & Server::getConfig() const
	{ return _config; }

void Server::setConfig(t_config config)
	{_config = config; }

int Server::getSd() const
	{ return _sd; }

void Server::setSd(int sd)
	{ _sd = sd; }

int Server::getServNbr() const
	{ return _srvrNbr; }

void Server::setSrvrNbr(size_t indx)
	{ _srvrNbr = indx; }

unsigned Server::getPort() const
	{ return _port; }

void Server::setPort(unsigned port)
	{_port = port; }
