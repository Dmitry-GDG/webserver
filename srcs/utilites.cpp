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
	std::cerr << "\r" << REV << errmsg << std::endl;
	printMsgToLogFile(-1, -1, errmsg, "");
	printMsgToLogFile(-1, -1, "Webserver stopped by error. Buy!\n ----------------------", "");
	exit (EXIT_FAILURE);
}

void ctrl_c_handler(int signum)
{
	(void) signum;
	printMsgToLogFile(-1, -1, "Webserver stopped by Ctrl+C. Buy!\n ----------------------", "");
	std::cout << "\r" << "Buy!" << std::endl;
	exit (EXIT_SUCCESS);
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
	std::string subStr; 
	size_t startIndex = 0;  
	for (size_t endIndex = 0; endIndex <= str.size(); endIndex++)  
	{
		if (endIndex == str.size() || str[endIndex] == seperator)  
		{
			subStr = "";  
			subStr.append(str, startIndex, endIndex - startIndex);  
			strings.push_back(subStr);  
			startIndex = endIndex + 1;  
		}  
	}



	// size_t currIndex = 0;
	// size_t i = 0;
	// size_t startIndex = 0, endIndex = 0;  
	// while (i <= str.size())  
	// {
	// 	if (str[i] == seperator || i == str.size())  
	// 	{
	// 		endIndex = i;  
	// 		std::string subStr = "";  
	// 		subStr.append(str, startIndex, endIndex - startIndex);  
	// 		strings.push_back(subStr);  
	// 		// currIndex += 1;  
	// 		startIndex = endIndex + 1;  
	// 	}  
	// 	i++;
	// }
}

void splitStringColon (std::string str, char seperator, std::vector<std::string> & strings)  
{
	strings.clear();
	for (size_t endIndex = 0; endIndex <= str.size(); endIndex++)
	{
		if (endIndex == str.size() || str[endIndex] == seperator)
		{
			std::string subStr = "";
			subStr.append(str, 0, endIndex);
			strings.push_back(subStr);
			if (endIndex != str.size())
			{
				subStr = "";
				subStr.append(str, endIndex + 1, std::string::npos);
				strings.push_back(subStr);
			}
			return ;
		}
	}
}

std::string timestamp()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return("[" + unsignedToString99(ltm->tm_hour) +  ":" + unsignedToString99(ltm->tm_min) + ":" +  unsignedToString99(ltm->tm_sec) + "]: ");
}

void printMsgToLogFile(int srvNb, int clntSd, std::string msg1, std::string msg2)
{
	std::string msg = timestamp();
	if (srvNb >= 0)
		msg += "server[" + std::to_string(srvNb) + "]: ";
	msg += msg1;
	clntSd >= 0 ? msg += std::to_string(clntSd) : msg += " ";
	msg += msg2;
    std::ofstream fout("logfile.txt", std::ofstream::app);
    fout << msg << std::endl; 
    fout.close();
}

void printMsg(int srvNb, int clntSd, std::string msg1, std::string msg2)
{
	std::string msg = NC + timestamp() + YELLOS;
	if (srvNb >= 0)
		msg += "server[" + std::to_string(srvNb) + "]: ";
	msg += msg1;
	clntSd >= 0 ? msg += NC + std::to_string(clntSd) + YELLOS : msg += " ";
	msg += msg2 + NC;
	std::cout << msg << std::endl;
}

void printMsgErr(int srvNb, int clntSd, std::string msg1, std::string msg2)
{
	std::string msg = NC + timestamp() + YELLOS;
	if (srvNb >= 0)
		msg += "server[" + std::to_string(srvNb) + "]: ";
	msg += msg1;
	clntSd >= 0 ? msg += NC + std::to_string(clntSd) + YELLOS : msg += " ";
	msg += msg2 + NC;
	std::cerr << msg << std::endl;
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

void connectionInputDataClear(t_connection & connection)
{
	connection.inputData.method = "";
	connection.inputData.address = "";
	connection.inputData.httpVersion = "";
	connection.inputData.dataType = DATA_START;
	connection.inputData.htmlFields.clear();
	connection.inputData.inputStr.clear();
}

void connectionResponseDataClear(t_connection & connection)
{
	connection.responseData.responseStatusCode = "200";
	connection.responseData.method = NOT_DEFINED;
}

void connectionClear(t_connection & connection)
{
	connection.srvNbr = 0;
	connection.fromIp = "";
	connection.fromPort = 0;
	connection.clntSd = 0;
	connection.position = 0;
	connection.status = READ;
	connection.allowedMethods.clear();
	connection.responseStatusCodesAll.clear();
	connection.contentTypesAll.clear();
	connectionInputDataClear(connection);
	connectionResponseDataClear(connection);
}

std::string datastamp()
{
	std::string arr[] = {"Jan.", "Feb.", "Mar.", "Apr.", "May", "June", "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec."};
	std::vector<std::string> months(std::begin(arr), std::end(arr));
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return(std::to_string(ltm->tm_mday) +  " " + months[ltm->tm_mon] + " " +  std::to_string(ltm->tm_year + 1900));
}
