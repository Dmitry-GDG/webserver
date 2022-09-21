#include "main.hpp"

void delWhiteSpacesStr(std::string & inptStr)
{
	// _replace tabs into spaces
	for (std::string::iterator iterS = inptStr.begin(); iterS < inptStr.end(); iterS++)
	{
		if ((*iterS) == '\t')
			*iterS = ' ';
	}
	// _delete White Spaces
	std::string::iterator it = std::unique(inptStr.begin(), inptStr.end(), isBothSpace);
	inptStr.erase(it, inptStr.end());
	// _delete space at the begining of line
	if (inptStr[0] == ' ')
		inptStr.erase(inptStr.begin());
}

void splitStrDelimeter(std::string str, std::vector<std::string> & outp)
{
	// std::vector<std::string> outp;
	std::string delim = DELIMETER;
	std::string outpLine;
	outp.clear();
	long	first = 0;
	long	second = 0;
	for(;;)
	{
		second = str.find(delim.c_str(), first);
		if (second != -1)
		{
			outpLine = str.substr(first, second);
			first = second + delim.size();
			outp.push_back(outpLine);
		}
		else
		{
			outpLine = str.substr(first, str.size());
			if (*(outpLine.end() - 1) == 10)
				outpLine.erase(outpLine.end() - 1);
			if (outpLine.size() > 0)
				outp.push_back(outpLine);
			break;
		}
	}
	// return outp;
}

bool parseInputData(char * buf, t_connection * connection)
{
	std::string inpt = buf;
	delWhiteSpacesStr(inpt);
	std::vector<std::string>	splitBuf;
	std::vector<std::string>	splitStr;
	std::string inptStr;

	#ifdef DEBUGMODE
		std::cout << "**** DEBUGMODE parseInputData ****\nInput: " << inpt << ", size: " << inpt.size() << "\n-------------" << std::endl;
	#endif

	connectionInputdataClear(connection);
	splitStrDelimeter(inpt, splitBuf);
	#ifdef DEBUGMODE
		printVector(splitBuf, "DEBUGMODE parseInputData splitStrDelimeter");
	#endif

	std::vector<std::string>::iterator iterVV = splitBuf.begin();
	inptStr = *iterVV;
	// #ifdef DEBUGMODE
	// 	for (std::string::iterator iter = inptStr.begin(); iter < inptStr.end(); iter++)
	// 		std::cout << *iter << "\t" << (int)(*iter) << std::endl;
	// #endif

	delWhiteSpacesStr(inptStr);
	// #ifdef DEBUGMODE
	// 	std::cout << "**** DEBUG delWhiteSpacesStr ****\n" << inptStr << "\n-----------------" << std::endl;
	// #endif
	if (inptStr.find("HTTP") != std::string::npos)
	{
		splitStr.clear();
		splitString(inptStr, ' ', splitStr);
		if (splitStr.size() < 3)
		{
			std::cerr << "Error! Incorrect request" << std::endl;
			return false;
		}

		connection->inputdata.dataType = HEADERS;

		std::vector<std::string>::iterator iter = splitStr.begin();

		if ( std::find(connection->methods.begin(), connection->methods.end(), *iter) == connection->methods.end() )
		{
			std::cerr << "Error! Unknown method" << std::endl;
			return false;
		}
		else
			connection->inputdata.method = *iter;

		connection->inputdata.address = *(iter + 1);
		connection->inputdata.httpVersion = *(iter + 2);

		iterVV++;
		for (; iterVV < splitBuf.end(); iterVV++)
		{
			inptStr = *iterVV;
			delWhiteSpacesStr(inptStr);
			splitStr.clear();
			splitString(inptStr, ':', splitStr);
			connection->inputdata.htmlFields[splitStr[0]] = splitStr[1];
		}
	}
	// #ifdef DEBUGMODE
	// 	printConnection(connection, "DEBUGMODE parseInputData printConnection");
	// #endif
	return true;
}
