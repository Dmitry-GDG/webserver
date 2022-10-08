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
			subStr.clear();  
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

void splitStringStr(std::string str, std::string seperator, std::vector<std::string> & strings)  
{
	strings.clear();
	std::string subStr;
	size_t pos = 0;
	size_t startIndex = 0;

	for (pos = str.find(seperator, pos++); pos != std::string::npos; pos = str.find(seperator, pos + 1)) //????
	{
		subStr.clear();
		subStr.append(str, startIndex, pos - startIndex);
		strings.push_back(subStr);
		startIndex = pos + seperator.size();
	}


	// while (pos != std::string::npos)
	// {

	// }

	// std::string str1 = str.substr (0, pos);
	// std::string str2 = (pos < str.size() - 4) ? str.substr (pos + 4) : "";

	// std::string subStr; 
	// size_t startIndex = 0;  
	// for (size_t endIndex = 0; endIndex <= str.size(); endIndex++)  
	// {
	// 	if (endIndex == str.size() || str[endIndex] == seperator)  
	// 	{
	// 		subStr.clear();  
	// 		subStr.append(str, startIndex, endIndex - startIndex);  
	// 		strings.push_back(subStr);  
	// 		startIndex = endIndex + 1;  
	// 	}  
	// }



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
				subStr.clear();
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

void replaceAllStrings(std::string & msg, const std::string & search, const std::string & replace)
{
	size_t pos = 0;
	while ((pos = msg.find(search, pos)) != std::string::npos)
	{
			msg.replace(pos, search.length(), replace);
			pos += replace.length();
	}
	// if (msg != "")
	// 	std::cout << "MSG test: " << msg << std::endl;
}

void printMsgToLogFile(int srvNb, int clntSd, std::string msg1, std::string msg2)
{
	std::string msg = timestamp();
	if (srvNb >= 0)
		msg += "server[" + std::to_string(srvNb) + "]: ";
	msg += msg1;
	clntSd >= 0 ? msg += std::to_string(clntSd) : msg += " ";
	replaceAllStrings(msg2, "\n", "\n\t");
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
	replaceAllStrings(msg2, "\n", "\n\t");
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
	location.path.clear();
	location.root.clear();
	location.index.clear();
	location.autoindex.clear();
	location.upload.clear();
	location.limit_size.clear();
	location.redirs.clear();
	location.methods.clear();
	location.cgi.clear();
	location.error_pages.clear();
}

void configClear(t_config & oneServerConfig)
{
	oneServerConfig.serverName.clear();
	oneServerConfig.listen.clear();
	oneServerConfig.ip.clear();
	oneServerConfig.port.clear();
	oneServerConfig.limitSize.clear();
	oneServerConfig.root.clear();
	oneServerConfig.index.clear();
	oneServerConfig.upload.clear();
	oneServerConfig.autoindex = "false";
	oneServerConfig.redirs.clear();
	oneServerConfig.methods.clear();
	oneServerConfig.locations.clear();
	oneServerConfig.error_pages.clear();
	oneServerConfig.cgi.clear();
}

void connectionInputDataClear(t_connection & connection)
{
	connection.inputData.method.clear();
	connection.inputData.address.clear();
	connection.inputData.addressParamsStr.clear();
	connection.inputData.httpVersion.clear();
	connection.inputData.dataType = NOT_DEFINED_INPUT_DATA_TYPE;
	// connection.inputData.headerFields.clear();
	connection.inputData.headerFieldsVec.clear();
	connection.inputData.bodyFieldsVec.clear();
	// connection.inputStr.clear();
}

void connectionResponseDataClear(t_connection & connection)
{
	connection.responseData.statusCode = "200";
	connection.responseData.type = NOT_DEFINED_RESPONSE_DATA_TYPE;
	connection.responseData.lenSent = 0;
	connection.responseData.lenAnswer = 0;
	connection.responseData.fileToSendInBinary.clear();
	connection.responseData.connectionAnswer.clear();
}

void connectionClear(t_connection & connection)
{
	connection.srvNbr = 0;
	connection.fromIp.clear();
	connection.fromPort = 0;
	connection.clntSd = 0;
	connection.position = 0;
	connection.lenBody = 0;
	connection.requestProcessingStep = NOT_DEFINED_REQUEST_PROCESSING_STEP;
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


std::string exec(const char* cmd)
{
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

void readFileToStrInBinary(const char * pathChar, std::string & outp)
{
	std::string line;
	std::vector<std::string> outpVector;

	outp.clear();
	outpVector.clear();
	std::ifstream in(pathChar, std::ios::binary);
	if (in.is_open())
	{
		while (getline(in, line))
			outpVector.push_back(line);
	}
	in.close();
	std::vector<std::string>::iterator iter = outpVector.begin();
	if  (iter != outpVector.end())
	{
		while (iter != outpVector.end())
		{
			outp += *iter;
			iter++;
			if (iter != outpVector.end())
				outp += "\n";
		}
	}
}
