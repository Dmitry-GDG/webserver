#ifndef MAIN_HPP
# define MAIN_HPP

# define NAME "webserv"
# define WEBSERV_NAME "Cool Webserver"
# define WEBSERV_VERSION "0.01"
# define WEBSERV_AUTHORS "trurgot, kmarg, jerrok                           "
# define HOSTNAME_LENGTH 30
# define SD_MAXCOUNT 1024
# define BUF_SIZE 200000
# define LIMIT_CLIENT_BODY_SIZE 3000000
# define DELIMETER "\r\n"
# define DDELIMETER "\r\n\r\n"
# define TIMEOUT 10 // in seconds

# define NC "\033[0m" //no colour
# define REV "\033[7m"
# define BLACKS "\033[2;30m"
# define BLUE "\033[0;34m"
# define BLUS "\033[2;34m"
# define GREEN "\033[0;32m"
# define GREENS "\033[2;32m"
# define RED "\033[0;31m"
# define REDS "\033[2;31m"
# define VIOLET "\033[0;35m"
# define VIOLETS "\033[2;35m"
# define AQUAMARINES "\033[2;36m"
# define YELLOS "\033[2;33m"

# include <algorithm>
# include <arpa/inet.h>
# include <cstdio>
# include <dirent.h>
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
void	splitStringStr(std::string str, std::string seperator, std::vector<std::string> & strings);
void	splitStringColon(std::string str, char seperator, std::vector<std::string> & strings);
void	locationClear(t_location & location);
void	configClear(t_config & oneServerConfig);
void	connectionClear(t_connection & connection);
void	connectionInputDataClear(t_connection & connection);
void	connectionResponseDataClear(t_connection & connection);
void	printMsg(int srvNb, int clntSd, std::string msg1, std::string msg2);
void	printMsgErr(int srvNb, int clntSd, std::string msg1, std::string msg2);
void	printMsgToLogFile(int srvNb, int clntSd, std::string msg1, std::string msg2);
void	readFileToStrInBinary(const char * path, std::string & outp);
size_t	findLastSlashInAddress(std::string addr);

void	printServerConfig(t_config config, std::string msg);
void	printWebServer(ServerRouter, std::string msg);
void	printAllServersConfig(std::vector<t_config> configs, std::string msg);
void	printAllServersVector(std::vector<Server> servers, std::string msg);
void 	printVector(std::vector<std::string> data, std::string msg);
void	printVectorEnter(std::vector<std::string> data, std::string msg);
void 	printVVector(std::vector<std::vector<std::string> > data, std::string msg);
void	printVectorPair(std::vector<std::pair<std::string, std::string> > vec, std::string msg);
void	printPollfds(const pollfd *pfds, std::string msg, size_t pSize);
void	printConnection(t_connection connection, std::string msg, int sign);
void	printAllConnections(std::vector<t_connection> connections, std::string msg);
void	printBoundary(t_connection connection, std::string msg);
void	printMapStrStr(std::map<std::string, std::string> dataMap, std::string msg);
void	printCharInt(std::string str, std::string msg);

bool	openConfigFileAndWriteToVectorConfigFile(std::string config_file_name, std::vector<std::string> & vectorConfigFile);
void 	parserVectorConfigFile(std::vector<std::string> & vectorConfigFile, std::vector<t_config> & configs);
std::string unsignedToString99(unsigned x);
std::string timestamp();
std::string timeStampHeader();
std::string getTime(time_t time);
std::string datastamp();
std::string dayOfWeekStamp();
uint16_t	atou16(const char *s);
std::string	execPy(const char *cmd);
void	exitErr(std::string errmsg);
void	ctrl_c_handler(int signum);
std::string	correctSlashInAddress(std::string str);
void	replaceAllStrings(std::string & msg, const std::string & search, const std::string & replace);
std::string	getFileName(std::vector<std::string> dataVec);
std::string urlDecode(std::string const & address);
std::string urlEncode(std::string str);

#endif
