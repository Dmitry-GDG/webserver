#include "main.hpp"

void printAllServersConfig(std::vector<t_config> configs, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printAllServersConfig ****" << std::endl;
	for (size_t i = 0; i < configs.size(); i++)
	{
		std::cout << "Server Nr " << i << std::endl;
		printServerConfig(configs[i], "");
		std::cout << "-------------------------" << std::endl;
	}
}

void printVector(std::vector<std::string> data, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printVector ****" << std::endl;
	for (std::vector<std::string>::iterator iter = data.begin(); iter < data.end(); iter++)
	{
		std::cout << *iter;
		if (iter + 1 < data.end())
			std::cout << "\t";
	}
	std::cout << std::endl;
	if (msg != "")
		std::cout << "----------------" << std::endl;
}

void printVVector(std::vector<std::vector<std::string> > data, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printVVector ****" << std::endl;
	for (std::vector<std::vector<std::string> >::iterator iter = data.begin(); iter < data.end(); iter++)
		printVector(*iter, "");
	std::cout << std::endl;
	std::cout << "----------------" << std::endl;
}

void printAllServersVector(std::vector<Server> servers, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printAllServersVector ****" << std::endl;
	int i = 0;
	for (std::vector<Server>::iterator iter = servers.begin(); iter < servers.end(); iter++)
	{
		std::cout << "Server Nr " << i << std::endl;
		printServerConfig((*iter).getConfig(), "");
		std::cout << "-------------------------" << std::endl;
		i++;
	}
}

void printPollfds(pollfd *pfds, std::string msg, size_t pSize)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printPollfds ****" << std::endl;
	for (size_t i = 0; i < pSize; i++)
		std::cout << "_pollfds[" << i << "]: sd=_pollfds.fd = " << pfds[i].fd << ", _pollfds.events = " << pfds[i].events << ", _pollfds.revents = " << pfds[i].revents << std::endl;
	std::cout << "----------------" << std::endl;
}

void printServerConfig(t_config config, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	if (config.serverName.size() > 0)
		std::cout << "server name:\t" << config.serverName << std::endl;
	if (config.listen.size() > 0)
		std::cout << "listen:\t" << config.listen << std::endl;
	if (config.ip.size() > 0)
		std::cout << "ip:\t" << config.ip << std::endl;
	if (config.port.size() > 0)
		std::cout << "port:\t" << config.port << std::endl;
	if (config.limitSize.size() > 0)
		std::cout << "limitSize:\t" << config.limitSize << std::endl;
	if (config.root.size() > 0)
		std::cout << "root:\t" << config.root << std::endl;
	if (config.index.size() > 0)
		std::cout << "index:\t" << config.index << std::endl;
	if (config.upload.size() > 0)
		std::cout << "upload:\t" << config.upload << std::endl;
	if (config.autoindex.size() > 0)
		std::cout << "autoindex:\t" << config.autoindex << std::endl;
	if (config.redirs.size() > 0)
		std::cout << "redirs:\t" << config.redirs << std::endl;
	if (config.cgi.size() > 0)
	{
		size_t j = 1;
		for(std::map<std::string, std::string>::iterator iter = config.cgi.begin(); iter != config.cgi.end(); iter++)
		{
			std::cout << "cgi " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
			j++;
		}
	}
	if (config.error_pages.size() > 0)
	{
		size_t j = 1;
		for(std::map<std::string, std::string>::iterator iter = config.error_pages.begin(); iter != config.error_pages.end(); iter++)
		{
			std::cout << "error_page " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
			j++;
		}
	}
	if (config.methods.size() > 0)
		{
			for (size_t j = 0; j < config.methods.size(); j++)
					std::cout << "method " << j << ":\t" << config.methods[j] << std::endl;
		}
	if (config.locations.size() > 0)
	{
		for (size_t i = 0; i < config.locations.size(); i++)
		{
			std::cout << "  location Nr " << i << std::endl;
			if (config.locations[i].path.size() > 0)
				std::cout << "path:\t" << config.locations[i].path << std::endl;
			if (config.locations[i].root.size() > 0)
				std::cout << "root:\t" << config.locations[i].root << std::endl;
			if (config.locations[i].index.size() > 0)
				std::cout << "index:\t" << config.locations[i].index << std::endl;
			if (config.locations[i].autoindex.size() > 0)
				std::cout << "autoindex:\t" << config.locations[i].autoindex << std::endl;
			if (config.locations[i].upload.size() > 0)
				std::cout << "upload:\t" << config.locations[i].upload << std::endl;
			if (config.locations[i].limit_size.size() > 0)
				std::cout << "limit_size:\t" << config.locations[i].limit_size << std::endl;
			if (config.locations[i].redirs.size() > 0)
				std::cout << "redirs:\t" << config.locations[i].redirs << std::endl;
			if (config.locations[i].methods.size() > 0)
			{
				for (size_t j = 0; j < config.locations[i].methods.size(); j++)
					std::cout << "method " << j << ":\t" << config.locations[i].methods[j] << std::endl;
			}
			if (config.locations[i].cgi.size() > 0)
			{
				size_t j = 1;
				for(std::map<std::string, std::string>::iterator iter = config.locations[i].cgi.begin(); iter != config.locations[i].cgi.end(); iter++)
				{
					std::cout << "cgi " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
					j++;
				}
			}
			if (config.locations[i].error_pages.size() > 0)
			{
				size_t j = 1;
				for(std::map<std::string, std::string>::iterator iter = config.locations[i].error_pages.begin(); iter != config.locations[i].error_pages.end(); iter++)
				{
					std::cout << "error_page " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
					j++;
				}
			}
		}
	}
}

void printConnection(t_connection * connection, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printConnection ****" << std::endl;
	std::cout << "server Nbr:\t" << connection->srvNbr << std::endl;
	std::cout << "Client Sd:\t" << connection->clntSd << std::endl;
	std::cout << "Client from:\t" << connection->fromIp << ":" << connection->fromPort << std::endl;
	std::cout << "Position:\t" << connection->position << std::endl;
	std::cout << "Status:\t" << connection->status << std::endl;
	if (connection->methods.size() > 0)
	{
		for (size_t i = 0; i < connection->methods.size(); i++)
			std::cout << "method[" << i << "]: " << connection->methods[i] << std::endl;
	}
	std::cout << "Inputdata method:\t" << connection->inputdata.method << std::endl;
	std::cout << "Inputdata address:\t" << connection->inputdata.address << std::endl;
	std::cout << "Inputdata httpVersion:\t" << connection->inputdata.httpVersion << std::endl;
	std::cout << "Inputdata dataType:\t" << connection->inputdata.dataType << std::endl;
	for (std::map<std::string, std::string>::iterator iterM = connection->inputdata.htmlFields.begin(); iterM != connection->inputdata.htmlFields.end(); iterM++)
		std::cout << (*iterM).first << ":" << (*iterM).second << std::endl;

	std::cout << "----------------" << std::endl;
}
