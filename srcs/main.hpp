#ifndef MAIN_HPP
# define MAIN_HPP

# define NAME "webserv"
# define WEBSERV_NAME "Cool Webserver"
# define WEBSERV_VERSION "0.01"
# define WEBSERV_AUTHORS "trurgot, botilia\t\t"
# define HOSTNAME_LENGTH 30
# define SD_MAXCOUNT 1024
# define BUF_SIZE 3000
# define DELIMETER "\r\n"
# define DDELIMETER "\r\n\r\n"
// # define DELIMETER "\\r\\n"
# define TIMEOUT 10 // in seconds

# define NC "\033[0m"
# define REV "\033[7m"
// # define BLACK "\033[0;30m"
# define BLACKS "\033[2;30m"
// # define BLUE "\033[0;34m"
# define BLUS "\033[2;34m"
# define GREEN "\033[0;32m"
# define GREENS "\033[2;32m"
# define RED "\033[0;31m"
// # define REDB "\033[41m"
# define REDS "\033[2;31m"
# define VIOLET "\033[0;35m"
# define VIOLETS "\033[2;35m"
# define AQUAMARINES "\033[2;36m"
// # define YELLOW "\033[33m"
# define YELLOS "\033[2;33m"

# include <algorithm>
# include <arpa/inet.h>
# include <fcntl.h>
# include <fstream>
# include <iostream>
# include <map>
# include <netinet/in.h>
# include <set>
# include <sstream>
# include <string>
# include <strings.h>
# include <sys/event.h>
# include <sys/poll.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
# include <vector>

# include "structs.hpp"
# include "Server.hpp"
# include "ServerRouter.hpp"

class Server;
class ServerRouter;

class ParserException : public std::exception 
{
	const char* what() const throw()
	{
		return  "Error of parsing, try other configuration file!";
	}
};

typedef unsigned char	u_char;

bool	isBothSpace(char const &lhs, char const &rhs);
void	splitString(std::string str, char seperator, std::vector<std::string> & strings);
void	splitStringColon(std::string str, char seperator, std::vector<std::string> & strings);
void	locationClear(t_location & location);
void	configClear(t_config & oneServerConfig);
void	connectionClear(t_connection & connection);
void	connectionInputDataClear(t_connection & connection);
void	connectionResponseDataClear(t_connection & connection);
void	printMsg(int srvNb, int clntSd, std::string msg1, std::string msg2);
void	printMsgErr(int srvNb, int clntSd, std::string msg1, std::string msg2);
void	printMsgToLogFile(int srvNb, int clntSd, std::string msg1, std::string msg2);

void	printServerConfig(t_config config, std::string msg);
void	printWebServer(ServerRouter, std::string msg);
void	printAllServersConfig(std::vector<t_config> configs, std::string msg);
void	printAllServersVector(std::vector<Server> servers, std::string msg);
void 	printVector(std::vector<std::string> data, std::string msg);
void 	printVVector(std::vector<std::vector<std::string> > data, std::string msg);
void	printPollfds(const pollfd *pfds, std::string msg, size_t pSize);
void	printConnection(t_connection connection, std::string msg, int sign);

bool	openConfigFileAndWriteToVectorConfigFile(std::string config_file_name, std::vector<std::string> & vectorConfigFile);
void 	parserVectorConfigFile(std::vector<std::string> & vectorConfigFile, std::vector<t_config> & configs);
std::string unsignedToString99(unsigned x);
std::string timestamp();
std::string datastamp();
uint16_t	atou16(const char *s);
std::string	exec(const char *cmd);
void	exitErr(std::string errmsg);
void	ctrl_c_handler(int signum);
std::string	correctSlashInAddress(std::string str);
// bool	parseInputData(char * buf, t_connection * connection);
// void	delWhiteSpacesStr(std::string & inptStr);
// bool	checkDelimeterAtTheEnd(std::string str);

#endif
