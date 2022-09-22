#include "main.hpp"
#include "Server.hpp"

class Server;

// class ParserException : public std::exception 
// {
// 	const char* what() const throw()
// 	{
// 		return  "Error of parsing, try other configuration file!";
// 	}
// };


void exitErr(std::string errmsg)
{
	if (errmsg != "Interrupted system call")
	{
		std::cerr << "\r" << REV << errmsg << std::endl;
		printMsgToLogFile(timestamp() + errmsg + "\n" + timestamp() + "Webserver stopped by error. Buy!\n ----------------------");
		exit (EXIT_FAILURE);
	}
	std::cout << "\r" << REV << "Buy!" << std::endl;
	printMsgToLogFile(timestamp() + errmsg + "\n" + timestamp() + "Webserver stopped. Buy!\n ----------------------");
	exit (EXIT_SUCCESS);
}

void ctrl_c_handler(int signum)
{
	(void) signum;
	printMsgToLogFile(timestamp() + "Webserver stopped by Ctrl+C. Buy!\n ----------------------");
	exit (EXIT_FAILURE);
}

std::string unsignedToString99(unsigned x)
{
    std::string s;
	if (x > 99)
	{
		while (x > 99)
			x %= 100;
	}
	s += x / 10 + '0';
	s += x % 10 + '0';
    return s;
}

uint16_t atou16(const char *s)
{
	uint16_t v = 0;

    while (*s)
		v = (v << 1) + (v << 3) + (*(s++) - '0');
    return v;
}

bool isBothSpace(char const &lhs, char const &rhs)
{
    return (lhs == ' ' || lhs == '\t') && (rhs == ' ' || rhs == '\t');
}

void splitString (std::string str, char seperator, std::vector<std::string> & strings)  
{
	strings.clear();
	size_t currIndex = 0, i = 0;  
	size_t startIndex = 0, endIndex = 0;  
	while (i <= str.size())  
	{
		if (str[i] == seperator || i == str.size())  
		{
			endIndex = i;  
			std::string subStr = "";  
			subStr.append(str, startIndex, endIndex - startIndex);  
			strings.push_back(subStr);  
			currIndex += 1;  
			startIndex = endIndex + 1;  
		}  
		i++;
	}
}

// void printServerConfig(t_config config, std::string msg)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	if (config.serverName.size() > 0)
// 		std::cout << "server name:\t" << config.serverName << std::endl;
// 	if (config.listen.size() > 0)
// 		std::cout << "listen:\t" << config.listen << std::endl;
// 	if (config.ip.size() > 0)
// 		std::cout << "ip:\t" << config.ip << std::endl;
// 	if (config.port.size() > 0)
// 		std::cout << "port:\t" << config.port << std::endl;
// 	if (config.limitSize.size() > 0)
// 		std::cout << "limitSize:\t" << config.limitSize << std::endl;
// 	if (config.root.size() > 0)
// 		std::cout << "root:\t" << config.root << std::endl;
// 	if (config.index.size() > 0)
// 		std::cout << "index:\t" << config.index << std::endl;
// 	if (config.upload.size() > 0)
// 		std::cout << "upload:\t" << config.upload << std::endl;
// 	if (config.autoindex.size() > 0)
// 		std::cout << "autoindex:\t" << config.autoindex << std::endl;
// 	if (config.redirs.size() > 0)
// 		std::cout << "redirs:\t" << config.redirs << std::endl;
// 	if (config.cgi.size() > 0)
// 	{
// 		size_t j = 1;
// 		for(std::map<std::string, std::string>::iterator iter = config.cgi.begin(); iter != config.cgi.end(); iter++)
// 		{
// 			std::cout << "cgi " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
// 			j++;
// 		}
// 	}
// 	if (config.error_pages.size() > 0)
// 	{
// 		size_t j = 1;
// 		for(std::map<std::string, std::string>::iterator iter = config.error_pages.begin(); iter != config.error_pages.end(); iter++)
// 		{
// 			std::cout << "error_page " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
// 			j++;
// 		}
// 	}
// 	if (config.methods.size() > 0)
// 		{
// 			for (size_t j = 0; j < config.methods.size(); j++)
// 					std::cout << "method " << j << ":\t" << config.methods[j] << std::endl;
// 		}
// 	if (config.locations.size() > 0)
// 	{
// 		for (size_t i = 0; i < config.locations.size(); i++)
// 		{
// 			std::cout << "  location Nr " << i << std::endl;
// 			if (config.locations[i].path.size() > 0)
// 				std::cout << "path:\t" << config.locations[i].path << std::endl;
// 			if (config.locations[i].root.size() > 0)
// 				std::cout << "root:\t" << config.locations[i].root << std::endl;
// 			if (config.locations[i].index.size() > 0)
// 				std::cout << "index:\t" << config.locations[i].index << std::endl;
// 			if (config.locations[i].autoindex.size() > 0)
// 				std::cout << "autoindex:\t" << config.locations[i].autoindex << std::endl;
// 			if (config.locations[i].upload.size() > 0)
// 				std::cout << "upload:\t" << config.locations[i].upload << std::endl;
// 			if (config.locations[i].limit_size.size() > 0)
// 				std::cout << "limit_size:\t" << config.locations[i].limit_size << std::endl;
// 			if (config.locations[i].redirs.size() > 0)
// 				std::cout << "redirs:\t" << config.locations[i].redirs << std::endl;
// 			if (config.locations[i].methods.size() > 0)
// 			{
// 				for (size_t j = 0; j < config.locations[i].methods.size(); j++)
// 					std::cout << "method " << j << ":\t" << config.locations[i].methods[j] << std::endl;
// 			}
// 			if (config.locations[i].cgi.size() > 0)
// 			{
// 				size_t j = 1;
// 				for(std::map<std::string, std::string>::iterator iter = config.locations[i].cgi.begin(); iter != config.locations[i].cgi.end(); iter++)
// 				{
// 					std::cout << "cgi " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
// 					j++;
// 				}
// 			}
// 			if (config.locations[i].error_pages.size() > 0)
// 			{
// 				size_t j = 1;
// 				for(std::map<std::string, std::string>::iterator iter = config.locations[i].error_pages.begin(); iter != config.locations[i].error_pages.end(); iter++)
// 				{
// 					std::cout << "error_page " << j << ":\t" << (*iter).first << "  " << (*iter).second << std::endl;
// 					j++;
// 				}
// 			}
// 		}
// 	}
// }

// void printAllServersConfig(std::vector<t_config> configs, std::string msg)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	else
// 		std::cout << "**** printAllServersConfig ****" << std::endl;
// 	for (size_t i = 0; i < configs.size(); i++)
// 	{
// 		std::cout << "Server Nr " << i << std::endl;
// 		printServerConfig(configs[i], "");
// 		std::cout << "-------------------------" << std::endl;
// 	}
// }

// void printAllServersVector(std::vector<Server> servers, std::string msg)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	else
// 		std::cout << "**** printAllServersVector ****" << std::endl;
// 	int i = 0;
// 	for (std::vector<Server>::iterator iter = servers.begin(); iter < servers.end(); iter++)
// 	{
// 		std::cout << "Server Nr " << i << std::endl;
// 		printServerConfig((*iter).getConfig(), "");
// 		std::cout << "-------------------------" << std::endl;
// 		i++;
// 	}
// }

// void printVector(std::vector<std::string> data, std::string msg)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	else
// 		std::cout << "**** printVector ****" << std::endl;
// 	for (std::vector<std::string>::iterator iter = data.begin(); iter < data.end(); iter++)
// 	{
// 		std::cout << *iter;
// 		if (iter + 1 < data.end())
// 			std::cout << "\t";
// 	}
// 	std::cout << std::endl;
// 	if (msg != "")
// 		std::cout << "----------------" << std::endl;
// }

// void printVVector(std::vector<std::vector<std::string> > data, std::string msg)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	else
// 		std::cout << "**** printVVector ****" << std::endl;
// 	for (std::vector<std::vector<std::string> >::iterator iter = data.begin(); iter < data.end(); iter++)
// 		printVector(*iter, "");
// 	std::cout << std::endl;
// 	std::cout << "----------------" << std::endl;
// }

// void printPollfds(pollfd *pfds, std::string msg, size_t pSize)
// {
// 	if (msg != "")
// 		std::cout << "**** " << msg << " ****" << std::endl;
// 	else
// 		std::cout << "**** printPollfds ****" << std::endl;
// 	for (size_t i = 0; i < pSize; i++)
// 		std::cout << "_pollfds[" << i << "]: sd=_pollfds.fd = " << pfds[i].fd << ", _pollfds.events = " << pfds[i].events << ", _pollfds.revents = " << pfds[i].revents << std::endl;
// 	std::cout << "----------------" << std::endl;
// }

void printMsg(size_t who, size_t whom, std::string msg1, std::string msg2)
{
	std::cout << NC << timestamp() << YELLOS << "server[" << who << "]: " << msg1 << NC << whom << YELLOS << msg2 <<  NC << std::endl;
}

void locationClear(t_location & location)
{
	location.path = "";
	location.root = "";
	location.index = "";
	location.autoindex = "";
	location.upload = "";
	location.limit_size = "";
	location.redirs = "";
	location.methods.clear();
	location.cgi.clear();
	location.error_pages.clear();
}

void configClear(t_config & oneServerConfig)
{
	oneServerConfig.serverName = "";
	oneServerConfig.listen = "";
	oneServerConfig.ip = "";
	oneServerConfig.port = "";
	oneServerConfig.limitSize = "";
	oneServerConfig.root = "";
	oneServerConfig.index = "";
	oneServerConfig.upload = "";
	oneServerConfig.autoindex = "false";
	oneServerConfig.redirs = "";
	oneServerConfig.methods.clear();
	oneServerConfig.locations.clear();
	oneServerConfig.error_pages.clear();
	oneServerConfig.cgi.clear();
}

void connectionInputdataClear(t_connection * connection)
{
	connection->inputdata.method = "";
	connection->inputdata.address = "";
	connection->inputdata.httpVersion = "";
	connection->inputdata.dataType = HEADERS;
	connection->inputdata.htmlFields.clear();
}

void connectionClear(t_connection & connection)
{
	connection.srvNbr = 0;
	connection.clntSd = 0;
	connection.position = 0;
	connection.status = READ;
	connection.fromIp = "";
	connection.fromPort = 0;
	connection.methods.clear();
	connection.inputdata.method = "";
	connection.inputdata.address = "";
	connection.inputdata.httpVersion = "";
	connection.inputdata.dataType = HEADERS;
	connection.inputdata.htmlFields.clear();
}

std::string timestamp()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return("[" + unsignedToString99(ltm->tm_hour) +  ":" + unsignedToString99(ltm->tm_min) + ":" +  unsignedToString99(ltm->tm_sec) + "]: ");
}

std::string datastamp()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return(std::to_string(ltm->tm_mday) +  "." + std::to_string(ltm->tm_mon) + "." +  std::to_string(ltm->tm_year + 1900) + "\n");
}

void printMsgToLogFile(std::string msg)
{
    std::ofstream fout("logfile.txt", std::ofstream::app);
    fout << msg << std::endl; 
    fout.close();
}
