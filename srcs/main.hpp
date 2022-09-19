#ifndef MAIN_HPP
# define MAIN_HPP

# define NAME "webserver"
# define WEBSERV_NAME "Cool Webserver"
# define WEBSERV_VERSION "0.01"
# define WEBSERV_AUTHORS "trurgot, botilia               "
# define HOSTNAME_LENGTH 30
# define SD_MAXCOUNT 1024

# define NC "\033[0m"
# define REV "\033[7m"
// # define BLACK "\033[0;30m"
# define BLACKS "\033[2;30m"
// # define BLUE "\033[0;34m"
# define BLUS "\033[2;34m"
// # define GREEN "\033[0;32m"
# define GREENS "\033[2;32m"
// # define RED "\033[0;31m"
// # define REDB "\033[41m"
# define REDS "\033[2;31m"
// # define VIOLET "\033[0;35m"
# define VIOLETS "\033[2;35m"
# define AQUAMARINES "\033[2;36m"
// # define YELLOW "\033[33m"
# define YELLOS "\033[2;33m"

# include <algorithm>
# include <arpa/inet.h>
// # include <cstring>
# include <fcntl.h>
# include <fstream>
# include <iostream>
# include <map>
# include <netinet/in.h>
# include <set>
# include <string>
# include <strings.h>
# include <sys/event.h>
# include <sys/poll.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <unistd.h>
# include <vector>

# include "structs.hpp"
# include "Server.hpp"

// # include "http_response_status_codes.hpp"
// # include "messages.hpp"
// # include "webservconfig/lexer.hpp"
// # include "webservconfig/parser.hpp"

class Server;

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
void	printServerConfig(t_config config, std::string msg);
void	printAllServersConfig(std::vector<t_config> configs, std::string msg);
void	printAllServersVector(std::vector<Server> servers, std::string msg);
void 	printVector(std::vector<std::string> data, std::string msg);
void 	printVVector(std::vector<std::vector<std::string> > data, std::string msg);
// void	printPollfds(std::vector<pollfd> data, std::string msg);
void	printPollfds(pollfd *pfds, std::string msg, size_t pSize);
void	printMsg(size_t who, size_t whom, std::string msg1, std::string msg2);
void	locationClear(t_location & location);
void	configClear(t_config & oneServerConfig);

bool	openConfigFileAndWriteToVectorConfigFile(std::string config_file_name, std::vector<std::string> & vectorConfigFile);
void 	parserVectorConfigFile(std::vector<std::string> & vectorConfigFile, std::vector<t_config> & configs);
std::string unsignedToString99(unsigned x);
std::string timestamp();
uint16_t	atou16(const char *s);
void	exitErr(std::string errmsg);

#endif
