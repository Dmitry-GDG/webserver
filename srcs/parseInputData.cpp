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

std::vector<std::string> splitStrDelimeter(std::string str)
{
	std::vector<std::string> outp;
	outp.clear();
	size_t	first = 0;
	size_t	second = str.find(DELIMETER, first);
	while (second < str.size())
	{
		outp.push_back(str.substr(first, second));
		first = second + 2;
		second = str.find(DELIMETER, first);
	}
	return outp;
}

bool parseInputData(char * buf, t_connection * connection)
{
	std::string inpt = buf;
	delWhiteSpacesStr(inpt);
	std::vector<std::string>	splitBuf;
	std::vector<std::string>	splitStr;

	#ifdef DEBUGMODE
		std::cout << "**** DEBUGMODE parseInputData ****\nInput: " << inpt << ", size: " << inpt.size() << "\n-------------" << std::endl;
	#endif

	splitBuf = splitStrDelimeter(inpt);
	#ifdef DEBUGMODE
		std::cout << "**** DEBUGMODE parseInputData splitStrDelimeter ****\n";
		for (std::vector<std::string>::iterator iter = splitBuf.begin(); iter < splitBuf.end(); iter++)
		{
			std::cout << *iter << "\t";
			for (std::string::iterator iterS = (*iter).begin(); iterS < (*iter).end(); iterS++)
				std::cout << *iterS << "\t" << (int)(*iterS) << std::endl;
		}
		std::cout << "-------------" << std::endl;
	#endif

	// std::string inptStr = inpt.substr(0, inpt.find('\n', 0));
	std::string inptStr = inpt.substr(0, inpt.find(DELIMETER, 0));
	if(inptStr.size() > 0)
		inptStr.resize(inptStr.size() - 1);
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
	}
	return true;
}
