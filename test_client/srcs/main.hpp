#ifndef MAIN_HPP
# define MAIN_HPP

# define NAME "client_webserv"
# define CLIENT_NAME "Webserv Client"
# define CLIENT_VERSION "0.01"
# define CLIENT_AUTHORS "trurgot               "

# define BUFLEN 10000
# define HOSTNAME_LENGTH 30
# define DELIMETER "\r\n"
# define DDELIMETER "\r\n\r\n"
# define DELIMETER_LENGHT 2
# define HELP_CLIENT_MSG "The client receives no commands. Client can response the following specific commands: CLIENTFILESLIST (CFL), CLIENTHELP (CHELP), QUIT. Client can send the webserv a file (files): CFL -> POST <filename>[ <falename>[ <filename]]"
# define FILES_TO_SEND_DIR "./files/"

# include <algorithm>
# include <arpa/inet.h>
# include <dirent.h>
# include <fstream>
# include <iostream>
# include <map>
# include <netdb.h>
# include <stdio.h>
# include <string>
# include <string.h>
# include <sys/poll.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <thread>
# include <unistd.h>
# include <vector>

// #include "CommandParced.hpp"
// #include "Client.hpp"

class Client;

# define NC "\033[0m"
# define REV "\033[7m"
# define BLACKS "\033[2;30m"
# define BLUS "\033[2;34m"
# define GREENS "\033[2;32m"
# define REDS "\033[2;31m"
# define VIOLET "\033[0;35m"
# define VIOLETS "\033[2;35m"
# define AQUAMARINES "\033[2;36m"
# define YELLOS "\033[2;33m"

void  		stringToUpper(std::string & str);
std::string unsignedToString99(unsigned x);
std::string timestamp();
std::string replaceAllDelimeters(std::string &msg);

void	printCommandParsed(Client * client, std::string msg);
void	readFileToStrInBinary(const char * pathChar, std::string & outp);

#endif
