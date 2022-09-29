#include "main.hpp"
#include "ServerRouter.hpp"


// static std::pair<std::string, std::string> responseStatusCodesArr[] =
// {
// 	// std::vector<std::pair<std::string, std::string> > vr = {{"100", "Continue"}, {"101", "Switching Protocols"}, {"102", "Processing"}, {"103", "Early Hints"}, {"200", "OK"}, {"201", "Created"}, {"202", "Accepted"}, {"203", "Non-Authoritative Information"}, {"204", "No Content"}, {"205", "Reset Content"}, {"206", "Partial Content"}, {"207", "Multi-Status"}, {"208", "Already Reported"}, {"226", "IM Used"}, {"300", "Multiple Choices"}, {"301", "Moved Permanently"}, {"302", "Found"}, {"303", "See Other"}, {"304", "Not Modified"}, {"305", "Use Proxy"}, {"307", "Temporary Redirect"}, {"308", "Permanent Redirect"}, {"400", "Bad Request"}, {"401", "Unauthorized"}, {"402", "Payment Required"}, {"403", "Forbidden"}, {"404", "Not Found"}, {"405", "Method Not Allowed"}, {"406", "Not Acceptable"}, {"407", "Proxy Authentication Required"}, {"408", "Request Timeout"}, {"409", "Conflict"}, {"410", "Gone"}, {"411", "Length Required"}, {"412", "Precondition Failed"}, {"413", "Payload Too Large"}, {"414", "URI Too Long"}, {"415", "Unsupported Media Type"}, {"416", "Range Not Satisfiable"}, {"417", "Expectation Failed"}, {"418", "I'm a teapot"}, {"421", "Misdirected Request"}, {"422", "Unprocessable Content"}, {"423", "Locked"}, {"424", "Failed Dependency"}, {"425", "Too Early"}, {"426", "Upgrade Required"}, {"428", "Precondition Required"}, {"429", "Too Many Requests"}, {"431", "Request Header Fields Too Large"}, {"451", "Unavailable For Legal Reasons"}, {"500", "Internal Server Error"}, {"501", "Not Implemented"}, {"502", "Bad Gateway"}, {"503", "Service Unavailable"}, {"504", "Gateway Timeout"}, {"505", "HTTP Version Not Supported"}, {"506", "Variant Also Negotiates"}, {"507", "Insufficient Storage"}, {"508", "Loop Detected"}, {"510", "Not Extended"}, {"511", "Network Authentication Required"}};
//     std::make_pair("100", "Continue"), std::make_pair("101", "Switching Protocols") //, и т.д.
// };
 
// #define ARRSIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
 
// std::map<std::string, std::string> responseStatusCodes(responseStatusCodesArr, responseStatusCodesArr + ARRSIZE(responseStatusCodesArr));
 
// #undef  ARRSIZE



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

void printPollfds(const pollfd *pfds, std::string msg, size_t pSize)
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

void printConnection(t_connection connection, std::string msg, int sign)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printConnection ****" << std::endl;
	std::cout << "server Nbr:\t" << connection.srvNbr << std::endl;
	std::cout << "Client from:\t" << connection.fromIp << ":" << connection.fromPort << std::endl;
	std::cout << "Client Sd:\t" << connection.clntSd << std::endl;
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
	std::cout << "Last Connection time:\t" << connection.lastActivityTime << " sec" << std::endl;
	std::string arr[] = {"READ", "READ_DONE", "WRITE", "WRITE_DONE"};
	std::vector<std::string> sts(std::begin(arr), std::end(arr));
	std::cout << "Status:\t\t" << sts[connection.status] << std::endl;
	if (connection.allowedMethods.size() > 0)
	{
		for (size_t i = 0; i < connection.allowedMethods.size(); i++)
			std::cout << "allowedMethods[" << i << "]:\t" << connection.allowedMethods[i] << std::endl;
	}
	if (connection.responseStatusCodesAll.size() > 0)
	{
		std::cout << "Response status codes:" << std::endl;
		for(std::map<std::string, std::string>::iterator iter = connection.responseStatusCodesAll.begin(); iter != connection.responseStatusCodesAll.end(); iter++)
			std::cout << (*iter).first << ":" << (*iter).second << "\t";
		std::cout << std::endl;
	}
	if (connection.contentTypesAll.size() > 0)
	{
		std::cout << "Content types:" << std::endl;
		for(std::map<std::string, std::string>::iterator iter = connection.contentTypesAll.begin(); iter != connection.contentTypesAll.end(); iter++)
			std::cout << (*iter).first << ":" << (*iter).second << "\t";
		std::cout << std::endl;
	}
	std::cout << "Inputdata method:\t" << connection.inputData.method << std::endl;
	std::cout << "Inputdata address:\t" << connection.inputData.address << std::endl;
	std::cout << "Inputdata httpVersion:\t" << connection.inputData.httpVersion << std::endl;
	std::string arr2[] = {"HTTP", "DATA_START", "DATA_CONTIN"};
	std::vector<std::string> dtt(std::begin(arr2), std::end(arr2));
	std::cout << "Inputdata dataType:\t" << dtt[connection.inputData.dataType] << std::endl;
	int i = 0;
	for (std::map<std::string, std::string>::iterator iterM = connection.inputData.htmlFields.begin(); iterM != connection.inputData.htmlFields.end(); iterM++)
	{
		std::cout << "inputdata.htmlFields[" << std::to_string(i) << "]:\t" << (*iterM).first << ":\t" << (*iterM).second << std::endl;
		i++;
	}
	if (connection.inputData.inputStr.size() > 0)
		std::cout << "Input str:\n" << connection.inputData.inputStr << std::endl;

	std::cout << "----------------" << std::endl;
}

// void printConnection(t_connection * connection, std::string msg, int sign)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	else
// 		std::cout << "**** printConnection ****" << std::endl;
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
// 	std::cout << "Status:\t\t" << sts[connection->status] << std::endl;
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
// 	for (std::map<std::string, std::string>::iterator iterM = connection->inputdata.htmlFields.begin(); iterM != connection->inputdata.htmlFields.end(); iterM++)
// 	{
// 		std::cout << "inputdata.htmlFields[" << std::to_string(i) << "]:\t" << (*iterM).first << ":\t" << (*iterM).second << std::endl;
// 		i++;
// 	}
// 	if (connection->inputStr.size() > 0)
// 		std::cout << "Input str:\n" << connection->inputStr << std::endl;

// 	std::cout << "----------------" << std::endl;
// }

void printWebServer(ServerRouter data, std::string msg)
{
	if (msg != "")
		std::cout << "**** " << msg << " ****" << std::endl;
	else
		std::cout << "**** printWebServer ****" << std::endl;
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
	if (data.getResponseStatusCodes().size() > 0)
	{
		std::cout << "Response status codes:" << std::endl;
		std::map<std::string, std::string> tmp;
		tmp.clear();
		tmp = data.getResponseStatusCodes();
		for(std::map<std::string, std::string>::iterator iter = tmp.begin(); iter != tmp.end(); iter++)
			std::cout << (*iter).first << ":" << (*iter).second << "\t";
		std::cout << std::endl;
	}
	if (data.getContentTypes().size() > 0)
	{
		std::cout << "Content types:" << std::endl;
		std::map<std::string, std::string> tmp;
		tmp.clear();
		tmp = data.getContentTypes();
		for(std::map<std::string, std::string>::iterator iter = tmp.begin(); iter != tmp.end(); iter++)
			std::cout << (*iter).first << ":" << (*iter).second << "\t";
		std::cout << std::endl;
	}
	std::cout << "Quantuty of poiifds:\t" << data.getPollfdsQty() << std::endl;
	printPollfds(data.getPollfds(), "",  data.getPollfdsQty());
}
