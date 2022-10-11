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
	// for (std::vector<std::string>::iterator iter = data.begin(); iter < data.end(); iter++)
	// {
	// 	std::cout << *iter;
	// 	if (iter + 1 < data.end())
	// 		std::cout << "\t";
	// }
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
			std::cout << "location Nr " << i << std::endl;
			if (config.locations[i].path.size() > 0)
				std::cout << "   path:\t" << config.locations[i].path << std::endl;
			if (config.locations[i].root.size() > 0)
				std::cout << "   root:\t" << config.locations[i].root << std::endl;
			if (config.locations[i].index.size() > 0)
				std::cout << "   index:\t" << config.locations[i].index << std::endl;
			if (config.locations[i].autoindex.size() > 0)
				std::cout << "   autoindex:\t" << config.locations[i].autoindex << std::endl;
			if (config.locations[i].upload.size() > 0)
				std::cout << "   upload:\t" << config.locations[i].upload << std::endl;
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
	std::string arr[] = {"NOT_DEFINED_REQUEST_PROCESSING_STEP", "READING_HEADER", "READING_HEADER_DONE", "READING_BODY", "READING_BODY_DONE", "WRITING", "WRITING_DONE"};
	std::vector<std::string> sts(std::begin(arr), std::end(arr));
	if (connection.allowedMethods.size() > 0)
	{
		for (size_t i = 0; i < connection.allowedMethods.size(); i++)
			std::cout << "allowedMethods[" << i << "]:\t" << connection.allowedMethods[i] << std::endl;
	}
	// if (connection.responseStatusCodesAll.size() > 0)
	// {
	// 	std::cout << "Response status codes:" << std::endl;
	// 	for(std::map<std::string, std::string>::iterator iter = connection.responseStatusCodesAll.begin(); iter != connection.responseStatusCodesAll.end(); iter++)
	// 		std::cout << (*iter).first << ":" << (*iter).second << "\t";
	// 	std::cout << std::endl;
	// }
	// if (connection.contentTypesAll.size() > 0)
	// {
	// 	std::cout << "Content types:" << std::endl;
	// 	for(std::map<std::string, std::string>::iterator iter = connection.contentTypesAll.begin(); iter != connection.contentTypesAll.end(); iter++)
	// 		std::cout << (*iter).first << ":" << (*iter).second << "\t";
	// 	std::cout << std::endl;
	// }
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
	// for (std::map<std::string, std::string>::iterator iterM = connection.inputData.headerFields.begin(); iterM != connection.inputData.headerFields.end(); iterM++)
	// {
	// 	std::cout << "inputdata.headerFields[" << std::to_string(i) << "]:\t" << (*iterM).first << ":\t" << (*iterM).second << std::endl;
	// 	i++;
	// }
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

// void printConnection(t_connection * connection, std::string msg, int sign)
// {
// 	if (msg != "")
// 		std::cout << " " << msg << " " << std::endl;
// 	else
// 		std::cout << " printConnection " << std::endl;
// 	std::cout << "server Nbr:\t" << connection->srvNbr << std::endl;
// 	std::cout << "Client from:\t" << connection->fromIp << ":" << connection->fromPort << std::endl;
// 	std::cout << "Client Sd:\t" << connection->clntSd << std::endl;
// 	if (sign == 1)
// 	{
// 		std::vector<std::pair<int, std::string> > vec;
// 		vec.push_back(std::make_pair(1, "POLLIN"));
// 		vec.push_back(std::make_pair(4, "POLLOUT"));
// 		std::cout << "Client events:\t";
// 		for (size_t i = 0; i < vec.size(); i++)
// 		{
// 			if (vec[i].first == connection->pfd->events)
// 				std::cout << vec[i].second << std::endl;
// 		}
// 		std::cout << "Client revents:\t";
// 		for (size_t i = 0; i < vec.size(); i++)
// 		{
// 			if (vec[i].first == connection->pfd->revents)
// 				std::cout << vec[i].second << std::endl;
// 		}
// 	}
// 	std::cout << "Position:\t" << connection->position << std::endl;
// 	std::string arr[] = {"READ", "READ_DONE", "WRITE", "WRITE_DONE"};
// 	std::vector<std::string> sts(std::begin(arr), std::end(arr));
// 	std::cout << "Status:\t" << sts[connection->status] << std::endl;
// 	if (connection->methods.size() > 0)
// 	{
// 		for (size_t i = 0; i < connection->methods.size(); i++)
// 			std::cout << "method[" << i << "]:\t" << connection->methods[i] << std::endl;
// 	}
// 	std::cout << "Inputdata method:\t" << connection->inputdata.method << std::endl;
// 	std::cout << "Inputdata address:\t" << connection->inputdata.address << std::endl;
// 	std::cout << "Inputdata httpVersion:\t" << connection->inputdata.httpVersion << std::endl;
// 	std::string arr2[] = {"HTTP", "DATA_START", "DATA_CONTIN"};
// 	std::vector<std::string> dtt(std::begin(arr2), std::end(arr2));
// 	std::cout << "Inputdata dataType:\t" << dtt[connection->inputdata.dataType] << std::endl;
// 	int i = 0;
// 	for (std::map<std::string, std::string>::iterator iterM = connection->inputdata.headerFields.begin(); iterM != connection->inputdata.headerFields.end(); iterM++)
// 	{
// 		std::cout << "inputdata.headerFields[" << std::to_string(i) << "]:\t" << (*iterM).first << ":\t" << (*iterM).second << std::endl;
// 		i++;
// 	}
// 	if (connection->inputStr.size() > 0)
// 		std::cout << "Input str:\n" << connection->inputStr << std::endl;

// 	std::cout << "----------------------" << std::endl;
// }

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
	// if (data.getConfigs().size() > 0)
	// 	printAllServersConfig(data.getConfigs(), "");
	if (data.getServers().size() > 0)
		printAllServersVector(data.getServers(), "");
	if (data.getConnections().size() > 0)
	{
		std::vector<t_connection> tmp = data.getConnections();
		for (std::vector<t_connection>::iterator iter = tmp.begin(); iter < tmp.end(); iter++)
			printConnection((*iter), "", 0);
	}
	// if (data.getResponseStatusCodes().size() > 0)
	// {
	// 	std::cout << "Response status codes:" << std::endl;
	// 	std::map<std::string, std::string> tmp;
	// 	tmp.clear();
	// 	tmp = data.getResponseStatusCodes();
	// 	for(std::map<std::string, std::string>::iterator iter = tmp.begin(); iter != tmp.end(); iter++)
	// 		std::cout << (*iter).first << ":" << (*iter).second << "\t";
	// 	std::cout << std::endl;
	// }
	// if (data.getContentTypes().size() > 0)
	// {
	// 	std::cout << "Content types:" << std::endl;
	// 	std::map<std::string, std::string> tmp;
	// 	tmp.clear();
	// 	tmp = data.getContentTypes();
	// 	for(std::map<std::string, std::string>::iterator iter = tmp.begin(); iter != tmp.end(); iter++)
	// 		std::cout << (*iter).first << ":" << (*iter).second << "\t";
	// 	std::cout << std::endl;
	// }
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
