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
	printMsgToLogFile(-1, -1, "Webserver stopped by error. Buy!\n----------------------", "");
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

	// std::cout << "splitStringStr0\n";
	if (seperator != "")
	{
		for (pos = str.find(seperator, pos++); pos != std::string::npos; pos = str.find(seperator, pos + 1)) //????
		{
			// std::cout << "splitStringStr1\n";
			subStr.clear();
			subStr.append(str, startIndex, pos - startIndex);
			strings.push_back(subStr);
			startIndex = pos + seperator.size();
			if (pos + seperator.size() != std::string::npos)
				pos += (seperator.size() - 1);
		}
	}
	// std::cout << "splitStringStr2\n";
	subStr.clear();
	subStr.append(str, startIndex, std::string::npos);
	strings.push_back(subStr);


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

void splitStringColon(std::string str, char seperator, std::vector<std::string> & strings)  
{
	strings.clear();
	for (size_t endIndex = 0; endIndex <= str.size(); endIndex++)
	{
		if (endIndex == str.size() || str[endIndex] == seperator)
		{
			std::string subStr = "";
			subStr.append(str, 0, endIndex);
			strings.push_back(subStr);
			subStr.clear();
			if (endIndex != str.size())
				subStr.append(str, endIndex + 1, std::string::npos);
			else
				subStr = "";
			strings.push_back(subStr);
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
	location.autoindex = false;
	// location.upload.clear();
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
	oneServerConfig.limitClientBodySize = 0;
	oneServerConfig.root.clear();
	oneServerConfig.index.clear();
	oneServerConfig.upload = "downloads";
	oneServerConfig.autoindex = false;
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
	connection.inputData.postContentType = NOT_DEFINED_POST_CONTENT_TYPE;
	connection.inputData.boundary.clear();
	connection.inputData.postFileName.clear();
	connection.inputData.postFileData.clear();
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
	// connection.responseData.pathExcludeInErr.clear();
	// connection.responseData.pathIncludeBeginInErr.clear();
}

void connectionClear(t_connection & connection)
{
	connection.srvNbr = 0;
	connection.fromIp.clear();
	connection.fromPort = 0;
	connection.clntSd = 0;
	connection.position = 0;
	connection.сontentLength = 0;
	connection.lenGet = 0;
	connection.requestProcessingStep = NOT_DEFINED_REQUEST_PROCESSING_STEP;
	connection.allowedMethods.clear();
	connection.responseStatusCodesAll.clear();
	connection.contentTypesAll.clear();
	connection.referer.clear();
	connection.pathToStatusCode.clear();
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
		in.close();
	}
	// in.close();
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

size_t findLastSlashInAddress(std::string addr)
{
	size_t pos = 0;
	size_t posLast = 0;

	for ( pos = addr.find("/", pos++); pos != std::string::npos; pos = addr.find("/", pos + 1))
		posLast = pos;
	return posLast;
}

std::string getFileName(std::vector<std::string> dataVec)
{
	for (std::vector<std::string>::iterator iter = dataVec.begin(); iter < dataVec.end(); iter++)
	{
		// std::cout << " utils getFileName *iter  : " << *iter << std::endl;
		if ((*iter).find("filename") != std::string::npos)
		{
			std::vector<std::string> outpVec;
			splitStringStr((*iter), "=", outpVec);
			// std::cout << " utils getFileName outpVec[1]  : " << outpVec[1] << std::endl;
			if (outpVec[1].size() && outpVec[1][0] == '"')
				outpVec[1].erase(outpVec[1].begin());
			if (outpVec[1].size() && outpVec[1][outpVec[1].size() - 1] == '"')
				outpVec[1].erase(outpVec[1].end() - 1);
			std::cout << BLUE << " utils getFileName  filename: " << NC << outpVec[1] << std::endl;
			return (outpVec[1]);
		}
	}
	return "";
}

// https://stackoverflow.com/questions/154536/encode-decode-urls-in-c

// std::string urlDecode(std::string str)
// {
//     std::string ret;
//     char ch;
//     int i, ii, len = str.length();

//     for (i=0; i < len; i++)
// 	{
//         if(str[i] != '%')
// 		{
//             if(str[i] == '+')
//                 ret += ' ';
//             else
//                 ret += str[i];
//         }
// 		else
// 		{
//             sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
//             ch = static_cast<char>(ii);
//             ret += ch;
//             i = i + 2;
//         }
//     }
//     return ret;
// }

std::string urlDecode(std::string const & address)
{
	std::string outp;
	unsigned int decodedI;
	size_t maxLen = address.find('?');

	for (size_t i = 0; i < address.length() && (i < maxLen || maxLen == std::string::npos); i++)
	{
		if (address[i] != '%')
		{
			if (address[i] == '+')
				outp += ' ';
			else
				outp += address[i];
		}
		else
		{
			sscanf(address.substr(i + 1, 2).c_str(), "%x", & decodedI);
			outp += static_cast<char>(decodedI);
			i += 2;
		}
	}
	return outp;
}

std::string urlEncode(std::string str)
{
    std::string new_str = "";
    char c;
    int ic;
    const char* chars = str.c_str();
    char bufHex[10];
    int len = strlen(chars);

    for(int i=0;i<len;i++){
        c = chars[i];
        ic = c;
        // uncomment this if you want to encode spaces with +
        /*if (c==' ') new_str += '+';   
        else */if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') new_str += c;
        else {
            sprintf(bufHex,"%X",c);
            if(ic < 16) 
                new_str += "%0"; 
            else
                new_str += "%";
            new_str += bufHex;
        }
    }
    return new_str;
 }

std::string getTime(time_t time)
{
	struct timeval tv;
	struct timezone tz;
	struct tm tm;
	int sec_per_day = 86400;
	int sec_per_hour = 3600;
	int sec_per_min = 60;
	gettimeofday(NULL, &tz);
	tv.tv_sec = time;

	time_t tmp = tv.tv_sec;
	int flag = 1;
	int year = 0;
	while (tmp > 31536000)
	{
		if (flag == 3)
		{
			tmp -= 31622400;
			flag = 0;
		}
		else
		{
			tmp -= 31536000;
			flag++;
		}
		year++;
	}
	tm.tm_mon = 0;
	while (tmp > 2419200)
	{
		if (tm.tm_mon == 0) // january
		{
			tmp -= 2678400;
			tm.tm_mon++;
		}
		else if (tm.tm_mon == 1) // february
		{
			if (flag == 4)
				tmp -= 2505600;
			else
				tmp -= 2419200;
			tm.tm_mon++;
		}
		else if (tm.tm_mon == 2) // march
		{
			tmp -= 2678400;
			tm.tm_mon++;
		}
		else if (tm.tm_mon == 3) // april
		{
			tmp -= 2592000;
			tm.tm_mon++;
		}
		else if(tm.tm_mon == 4) // may
		{
			tmp -= 2678400;
			tm.tm_mon++;
		}
		else if (tm.tm_mon == 5) { tmp -= 2592000; tm.tm_mon++; }
		else if (tm.tm_mon == 6) { tmp -= 2678400; tm.tm_mon++; }
		else if (tm.tm_mon == 7) { tmp -= 2678400; tm.tm_mon++; }
		else if (tm.tm_mon == 8) { tmp -= 2592000; tm.tm_mon++; }
		else if (tm.tm_mon == 9) { tmp -= 2678400; tm.tm_mon++; }
		else if (tm.tm_mon == 10) { tmp -= 2592000; tm.tm_mon++; }
		else if (tm.tm_mon == 11) { tmp -= 2678400; tm.tm_mon++; }
	}
	tm.tm_mday = 1;
	tm.tm_wday = 1;
	while (tmp > 86400)
	{
		if (tm.tm_wday > 6)
			tm.tm_wday = 0;
		tmp -= 86400;
		tm.tm_mday++;
		tm.tm_wday++;
		if (tm.tm_wday > 6)
			tm.tm_wday = 0;
	}
	tm.tm_year = 70 + year; // year

	long hms = tv.tv_sec % sec_per_day;
	hms += tz.tz_dsttime * sec_per_hour;
	hms -= tz.tz_minuteswest * sec_per_min;
	hms = (hms + sec_per_day) % sec_per_day;

	tm.tm_hour = hms / sec_per_hour; // hourа
	tm.tm_min = (hms % sec_per_hour) / sec_per_min; // min
	tm.tm_sec = (hms % sec_per_hour) % sec_per_min; // sec
	tm.tm_zone = (char *)"GMT";

	char *buf = new char[100];
	tm.tm_isdst = -1; // не используем(переход летнее время)
	tm.tm_gmtoff = 0; // не используем
	tm.tm_yday = 0; // не используем
	strftime (buf, 100, "Date: %a, %d %b %Y %X %Z", &tm);
	std::string outp(buf);
	free (buf);
	return (outp);
}

std::string dayOfWeekStamp()
{
	std::string arr[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	std::vector<std::string> days(std::begin(arr), std::end(arr));
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return(days[ltm->tm_wday]);
}

std::string timeStampHeader()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	return("Date: " + dayOfWeekStamp() +  ", " + datastamp() + " " + unsignedToString99(ltm->tm_hour) +  ":" + unsignedToString99(ltm->tm_min) + ":" +  unsignedToString99(ltm->tm_sec) + " " + ltm->tm_zone);
}

std::string execPy(const char* cmd)
{
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try
	{
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
		{
            result += buffer;
        }
    }
	catch (...)
	{
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}
