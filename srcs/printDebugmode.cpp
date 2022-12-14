#include "main.hpp"
#include "ServerRouter.hpp"

void printAllServersConfig(std::vector<t_config> configs, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printAllServersConfig " << NC << std::endl;
	for (size_t i = 0; i < configs.size(); i++)
	{
		std::cout << "Server Nr " << i << std::endl;
		printServerConfig(configs[i], "");
		std::cout << "----------------------" << std::endl;
	}
	std::cout << "----------------------" << std::endl;
}

void printVector(std::vector<std::string> data, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printVector " << NC << std::endl;
	std::vector<std::string>::iterator iter = data.begin();
	if (iter < data.end())
	{
		while (iter < data.end())
		{
			std::cout << *iter;
			iter++;
			if (iter < data.end())
				std::cout << "\t";
		}
	}
	std::cout << std::endl;
	if (msg != "")
		std::cout << "----------------------" << std::endl;
}

void printVectorEnter(std::vector<std::string> data, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printVector " << NC << std::endl;
	std::vector<std::string>::iterator iter = data.begin();
	if (iter < data.end())
	{
		while (iter < data.end())
		{
			std::cout << *iter;
			iter++;
			if (iter < data.end())
				std::cout << "\n";
		}
	}
	std::cout << std::endl;
	if (msg != "")
		std::cout << "----------------------" << std::endl;
}

void printVVector(std::vector<std::vector<std::string> > data, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printVVector " << NC << std::endl;
	for (std::vector<std::vector<std::string> >::iterator iter = data.begin(); iter < data.end(); iter++)
		printVector(*iter, "");
	std::cout << std::endl;
	std::cout << "----------------------" << std::endl;
}

void printAllServersVector(std::vector<Server> servers, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printAllServersVector " << NC << std::endl;
	int i = 0;
	for (std::vector<Server>::iterator iter = servers.begin(); iter < servers.end(); iter++)
	{
		std::cout << "Server Nr " << i << std::endl;
		printServerConfig((*iter).getConfig(), "");
		std::cout << "----------------------" << std::endl;
		i++;
	}
	std::cout << "----------------------" << std::endl;
}

void printPollfds(const pollfd *pfds, std::string msg, size_t pSize)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printPollfds " << NC << std::endl;
	for (size_t i = 0; i < pSize; i++)
		std::cout << "_pollfds[" << i << "]: sd=_pollfds.fd = " << pfds[i].fd << ", _pollfds.events = " << pfds[i].events << ", _pollfds.revents = " << pfds[i].revents << std::endl;
	std::cout << "----------------------" << std::endl;
}

void printServerConfig(t_config config, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	if (config.serverName.size() > 0)
		std::cout << "server name:\t" << config.serverName << std::endl;
	if (config.listen.size() > 0)
		std::cout << "listen:\t" << config.listen << std::endl;
	if (config.ip.size() > 0)
		std::cout << "ip:\t" << config.ip << std::endl;
	if (config.port.size() > 0)
		std::cout << "port:\t" << config.port << std::endl;
	// if (config.limitClientBodySize > 0)
	std::cout << "limit client body size:\t" << config.limitClientBodySize << std::endl;
	if (config.root.size() > 0)
		std::cout << "root:\t" << config.root << std::endl;
	if (config.index.size() > 0)
		std::cout << "index:\t" << config.index << std::endl;
	if (config.upload.size() > 0)
		std::cout << "upload:\t" << config.upload << std::endl;
	// if (config.autoindex.size() > 0)
	std::cout << "autoindex:\t";
	if (config.autoindex)
		std::cout << "true" << std::endl;
	else
		std::cout << "false" << std::endl;
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
			std::cout << "location Nr " << i << std::endl;
			if (config.locations[i].path.size() > 0)
				std::cout << "   path:\t" << config.locations[i].path << std::endl;
			if (config.locations[i].root.size() > 0)
				std::cout << "   root:\t" << config.locations[i].root << std::endl;
			if (config.locations[i].index.size() > 0)
				std::cout << "   index:\t" << config.locations[i].index << std::endl;
			// if (config.locations[i].autoindex.size() > 0)
			std::cout << "   autoindex:\t";
			if (config.locations[i].autoindex)
				std::cout << "true" << std::endl;
			else
				std::cout << "false" << std::endl;
			// if (config.locations[i].upload.size() > 0)
			// 	std::cout << "   upload:\t" << config.locations[i].upload << std::endl;
			if (config.locations[i].limit_size.size() > 0)
				std::cout << "   limit_size:\t" << config.locations[i].limit_size << std::endl;
			if (config.locations[i].redirs.size() > 0)
				std::cout << "   redirs:\t" << config.locations[i].redirs << std::endl;
			if (config.locations[i].methods.size() > 0)
			{
				for (size_t j = 0; j < config.locations[i].methods.size(); j++)
					std::cout << "   method " << j << ":\t" << config.locations[i].methods[j] << std::endl;
			}
			if (config.locations[i].cgi.size() > 0)
			{
				size_t j = 1;
				for(std::map<std::string, std::string>::iterator iter = config.locations[i].cgi.begin(); iter != config.locations[i].cgi.end(); iter++)
				{
					std::cout << "   cgi " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
					j++;
				}
			}
			if (config.locations[i].error_pages.size() > 0)
			{
				size_t j = 1;
				for(std::map<std::string, std::string>::iterator iter = config.locations[i].error_pages.begin(); iter != config.locations[i].error_pages.end(); iter++)
				{
					std::cout << "   error_page " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
					j++;
				}
			}
		}
	}
	std::cout << "----------------------" << std::endl;
}

void printConnection(t_connection connection, std::string msg, int sign)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printConnection " << NC << std::endl;
	std::cout << "server Nbr:\t" << connection.srvNbr << std::endl;
	std::cout << "Client from:\t" << connection.fromIp << ":" << connection.fromPort << std::endl;
	std::cout << "Client Sd:\t" << connection.clntSd << std::endl;
	std::cout << "Last Connection time:\t" << connection.lastActivityTime << " sec" << std::endl;
	if (connection.allowedMethods.size() > 0)
	{
		for (size_t i = 0; i < connection.allowedMethods.size(); i++)
			std::cout << "allowedMethods[" << i << "]:\t" << connection.allowedMethods[i] << std::endl;
	}
	std::string arr[] = {"NOT_DEFINED_REQUEST_PROCESSING_STEP", "READING_HEADER", "READING_HEADER_DONE", "READING_BODY", "READING_BODY_DONE", "WRITING", "WRITING_DONE"};
	std::vector<std::string> sts(std::begin(arr), std::end(arr));
	std::cout << "Request Processing Step:\t" << RED << sts[connection.requestProcessingStep] << NC << std::endl;
	if (sign == 1)
	{
		std::vector<std::pair<int, std::string> > vec;
		vec.push_back(std::make_pair(1, "POLLIN"));
		vec.push_back(std::make_pair(4, "POLLOUT"));
		std::cout << "Client events:\t";
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (vec[i].first == connection.pfd->events)
				std::cout << vec[i].second << std::endl;
		}
		std::cout << "Client revents:\t";
		for (size_t i = 0; i < vec.size(); i++)
		{
			if (vec[i].first == connection.pfd->revents)
				std::cout << vec[i].second << std::endl;
		}
	}
	std::cout << "Position:\t" << connection.position << std::endl;
	std::string arr2[] = {"NOT_DEFINED_INPUT_DATA", "HTTP"};
	std::vector<std::string> dtt(std::begin(arr2), std::end(arr2));
	std::cout << "Inputdata dataType:\t" << dtt[connection.inputData.dataType] << std::endl;
	std::cout << "Inputdata method:\t" << connection.inputData.method << std::endl;
	std::cout << "Inputdata address:\t" << connection.inputData.address << std::endl;
	std::cout << "Inputdata address params:\t" << connection.inputData.addressParamsStr << std::endl;
	std::cout << "Inputdata httpVersion:\t" << connection.inputData.httpVersion << std::endl;
	int i = 0;
	for (std::vector<std::pair<std::string, std::string> >::iterator iterM = connection.inputData.headerFieldsVec.begin(); iterM != connection.inputData.headerFieldsVec.end(); iterM++)
	{
		std::cout << "inputdata.headerFieldsVec[" << std::to_string(i) << "]:\t" << (*iterM).first;
		if ((*iterM).second != "")
			std::cout << ":\t" << (*iterM).second;
		std::cout << std::endl;
		i++;
	}
	if (connection.inputStr.size() > 0)
		std::cout << "+++++++\n" << GREEN << "Input str:\n" << NC << connection.inputStr << std::endl;
	if (connection.inputStrHeader.size() > 0)
		std::cout << "+++++++\n" << GREEN << "Input str Header:\n" << NC << connection.inputStrHeader << std::endl;
	if (connection.inputStrBody.size() > 0)
		std::cout << "+++++++\n" << GREEN << "Input str Body:\n" << NC << connection.inputStrBody << std::endl;

	std::cout << "+++++++\n----------------------" << std::endl;
}

void printAllConnections(std::vector<t_connection> connections, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printAllConnections " << NC << std::endl;
	if (connections.size() > 0)
	{
		for (std::vector<t_connection>::iterator iter = connections.begin(); iter < connections.end(); iter++)
			printConnection((*iter), "", 0);
	}
	std::cout << "----------------------" << std::endl;
}

void printWebServer(ServerRouter data, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printWebServer " << NC << std::endl;
	std::cout << "Host name: " << data.getHostname() << std::endl;
	if (data.getServers().size() > 0)
		printAllServersVector(data.getServers(), "");
	if (data.getConnections().size() > 0)
	{
		std::vector<t_connection> tmp = data.getConnections();
		for (std::vector<t_connection>::iterator iter = tmp.begin(); iter < tmp.end(); iter++)
			printConnection((*iter), "", 0);
	}
	std::cout << "Quantuty of poiifds:\t" << data.getPollfdsQty() << std::endl;
	printPollfds(data.getPollfds(), "",  data.getPollfdsQty());
	std::cout << "----------------------" << std::endl;
}

void printVectorPair(std::vector<std::pair<std::string, std::string> > vec, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printVectorPair " << NC << std::endl;
	for (std::vector<std::pair<std::string, std::string> >::iterator iterM = vec.begin(); iterM != vec.end(); iterM++)
	{
		std::cout << (*iterM).first;
		if ((*iterM).second != "")
			std::cout << ":\t" << (*iterM).second;
		std::cout << std::endl;
	}
}

void printBoundary(t_connection connection, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printBoundary " << NC << std::endl;
	if (connection.inputData.boundary.size() > 0)
	{
		size_t i = 0;
		for (std::vector<std::string>::iterator iter = connection.inputData.boundary.begin(); iter < connection.inputData.boundary.end(); iter++, i++)
			std::cout << "boundary[" << i << "]: " << connection.inputData.boundary[i] << std::endl;
	}
	std::cout << "----------------------" << std::endl;
}

void printMapStrStr(std::map<std::string, std::string> dataMap, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printMapStrStr " << NC << std::endl;
	if (dataMap.size() > 0)
	{
		size_t i = 0;
		for (std::map<std::string, std::string>::iterator iter = dataMap.begin(); iter != dataMap.end(); iter++, i++)
			std::cout << "dataMap[" << i << "]:\t" << (*iter).first << "\t" << (*iter).second << std::endl;
	}
	std::cout << "----------------------" << std::endl;
}

void	printCharInt(std::string str, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printCharInt " << NC << std::endl;
	for (size_t i = 0; i < str.size(); i++)
	{
		std::cout << str[i] << "\t" << int(str[i]) << std::endl;
	}
	std::cout << "----------------------" << std::endl;
}
