#include "ServerRouter.hpp"

bool ServerRouter::_checkDelimeterAtTheEnd(std::string str)
{
	std::string delim = DELIMETER;
	if ((str.size() > 1) && (str[str.size() - 2] == delim[delim.size() - 2]) && (str[str.size() - 1] == delim[delim.size() - 1]))
		return true;
	return false;
}

void ServerRouter::_delWhiteSpacesStr(std::string & inptStr)
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

// void ServerRouter::_splitStrDelimeter(std::string str, std::vector<std::string> & outp, std::string delim)
// {
// 	// std::string delim = DELIMETER;
// 	// std::string delim = "\r\n";
// 	std::string outpLine;
// 	outp.clear();
// 	unsigned long	first = 0;
// 	unsigned long	second = 0;
// 	for(;;)
// 	{
// 		second = str.find(delim, first);
// 		if (second != std::string::npos)
// 		{
// 			outpLine = str.substr(first, second - first);
// 			// std::cout << "outpLine: " << outpLine << std::endl;
// 			first = second + delim.size();
// 			if (*(outpLine.end() - 1) == 10 || *(outpLine.end() - 1) == 32)
// 				outpLine.erase(outpLine.end() - 1);
// 			if (outpLine.size() > 0)
// 				outp.push_back(outpLine);
// 		}
// 		else
// 		{
// 			outpLine = str.substr(first, str.size());
// 			// std::cout << "outpLine2: " << outpLine << std::endl;
// 			if (*(outpLine.end() - 1) == 10 || *(outpLine.end() - 1) == 32)
// 				outpLine.erase(outpLine.end() - 1);
// 			if (outpLine.size() > 0)
// 				outp.push_back(outpLine);
// 			break;
// 		}
// 	}
// }

// void splitStrDelimeter(std::string str, std::vector<std::string> & outp)
// {
// 	std::string delim = DELIMETER;
// 	// std::string delim = "\r\n";
// 	std::string outpLine;
// 	outp.clear();
// 	unsigned long	first = 0;
// 	unsigned long	second = 0;
// 	for(;;)
// 	{
// 		second = str.find(delim, first);
// 		if (second != std::string::npos)
// 		{
// 			outpLine = str.substr(first, second - first);
// 			// std::cout << "outpLine: " << outpLine << std::endl;
// 			first = second + delim.size();
// 			if (*(outpLine.end() - 1) == 10 || *(outpLine.end() - 1) == 32)
// 				outpLine.erase(outpLine.end() - 1);
// 			if (outpLine.size() > 0)
// 				outp.push_back(outpLine);
// 		}
// 		else
// 		{
// 			outpLine = str.substr(first, str.size());
// 			// std::cout << "outpLine2: " << outpLine << std::endl;
// 			if (*(outpLine.end() - 1) == 10 || *(outpLine.end() - 1) == 32)
// 				outpLine.erase(outpLine.end() - 1);
// 			if (outpLine.size() > 0)
// 				outp.push_back(outpLine);
// 			break;
// 		}
// 	}
// }

void ServerRouter::_parseMultiStringData(std::vector<std::string>	splitBuf, t_connection * connection)
{
	(void) splitBuf;
	(void) connection;
	std::cout << "parseMultiStringData" << std::endl;
}

bool ServerRouter::_parseInputDataHeader(t_connection * connection)
{
	std::string inpt = connection->inputStrHeader;
	std::vector<std::string>	splitBuf;
	std::vector<std::string>	splitStr;
	std::string inptStr;
	std::string msg, msg1;

	_delWhiteSpacesStr(inpt);
	// #ifdef DEBUGMODE
	// 	std::cout << " DEBUGMODE parseInputData \nInput: " << inpt << ", size: " << inpt.size() << "\n----------------------" << std::endl;
	// #endif

	// std::vector<std::string> headerBody;
	// _splitStrDelimeter(inpt, headerBody, DDELIMETER);
	// // #ifdef DEBUGMODE
	// // 	std::cout << " DEBUGMODE parseInputData splitStrDelimeter headerBody " << std::endl;
	// // 	size_t jjj = 0;
	// // 	for (std::vector<std::string>::iterator iter = headerBody.begin(); iter < headerBody.end(); iter++)
	// // 	{
	// // 		std::cout << "headerBody[" << jjj << "]: " << *iter << std::endl;
	// // 		jjj++;
	// // 	}
	// // 	std::cout << "---- end of headerBody ----" << std::endl;
	// // #endif




	connectionInputDataClear(* connection);

	splitStringStr(inpt, DELIMETER, splitBuf);
	// _splitStrDelimeter(inpt, splitBuf, DELIMETER);

	// #ifdef DEBUGMODE
	// 	printVector(splitBuf, "DEBUGMODE parseInputData splitStrDelimeter");
	// #endif

	std::vector<std::string>::iterator iterVV = splitBuf.begin();
	inptStr = *iterVV;
	// #ifdef DEBUGMODE
	// 	int j = 0;
	// 	for (std::string::iterator iter = inptStr.begin(); iter < inptStr.end(); iter++)
	// 	{
	// 		std::cout << j << "\t" << *iter << "\t" << (int)(*iter) << std::endl;
	// 		j++;
	// 	}
	// 	std::cout << "----------------------" << std::endl;
	// #endif

	_delWhiteSpacesStr(inptStr);
	// #ifdef DEBUGMODE
	// 	std::cout << "DEBUGMODE parseInputData _delWhiteSpacesStr(inptStr)/n/tintpstr: " << inptstr << "\n----------------------\n";
	// #endif
	// msg = "data from sd ";
	// printMsg(connection->srvNbr, connection->clntSd, msg, ":\n" + inptStr);
	// printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, ":\n" + inptStr);

	if (inptStr.find("HTTP") != std::string::npos)
	{
		splitStr.clear();
		splitString(inptStr, ' ', splitStr);
		if (splitStr.size() < 3)
		{
			msg = "Error! Incorrect request from sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "400";
			return false;
		}

		connection->inputData.dataType = HTTP;

		std::vector<std::string>::iterator iter = splitStr.begin();

		// if ( std::find(connection->allowedMethods.begin(), connection->allowedMethods.end(), *iter) == connection->allowedMethods.end() )
		// {
		// 	msg = "Error! Unknown method from sd ";
		// 	printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		// 	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		// 	return false;
		// }
		// else
		connection->inputData.method = *iter;
		iter++;
		// connection->inputData.address = correctSlashInAddress(*iter);
		std::string addr = correctSlashInAddress(*iter);
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR_parse _parseInputDataHeader \naddr: " << NC << addr << "\n----------------------\n";
		// #endif
		size_t pos = addr.find('?');
		if (pos != std::string::npos)
			connection->inputData.addressParamsStr = addr.substr(pos + 1);
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR_parse _parseInputDataHeader \nconnection->inputData.addressParamsStr: " << NC << connection->inputData.addressParamsStr << "\n----------------------\n";
		// #endif

		connection->inputData.address = urlDecode(addr);
		// connection->inputData.address = _addressDecode(addr);

		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR_parse _parseInputDataHeader \nconnection->inputData.address: " << NC << connection->inputData.address << "\n----------------------\n";
		// #endif
		iter++;
		connection->inputData.httpVersion = *iter;

		iterVV++;
		// std::cout << "iterVV: " << *iterVV << std::endl;
		for (; iterVV < splitBuf.end(); iterVV++)
		{
			inptStr = *iterVV;
			_delWhiteSpacesStr(inptStr);
			splitStr.clear();
			splitStringColon(inptStr, ':', splitStr);
			// #ifdef DEBUGMODE
			// 	std::cout << "DEBUGMODE parseInputData nputdata.headerFields\t" << splitStr[0] << "\t" << splitStr[1] << std::endl;
			// #endif
			// connection->inputData.headerFields[splitStr[0]] = splitStr[1];
			if (splitStr.size() == 1)
				connection->inputData.headerFieldsVec.push_back(std::make_pair(splitStr[0], ""));
			else if (splitStr.size() == 2)
				connection->inputData.headerFieldsVec.push_back(std::make_pair(splitStr[0], splitStr[1]));
		}
		// msg = "data from sd ";
		// msg1 = connection->inputData.method + " " + connection->inputData.address;
		// if (connection->inputData.addressParamsStr != "")
		// 	msg1 += "?" + connection->inputData.addressParamsStr;
		// msg1 += " " + connection->inputData.httpVersion + "\n";
		// // for (std::map<std::string, std::string>::iterator iter = connection->inputData.headerFields.begin(); iter != connection->inputData.headerFields.end(); iter++)
		// // 	msg1 += (*iter).first + ": " + (*iter).second + "\n";
		// for (std::vector<std::pair<std::string, std::string> >::iterator iter = connection->inputData.headerFieldsVec.begin(); iter != connection->inputData.headerFieldsVec.end(); iter++)
		// {	msg1 += (*iter).first;
		// 	if ((*iter).second != "")
		// 		msg1 += ": " + (*iter).second + "\n";
		// }
		// printMsg(connection->srvNbr, connection->clntSd, msg, ":\n" + msg1);
		// printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, ":\n" + msg1);
	}
	else
		_parseMultiStringData(splitBuf, connection);
	// #ifdef DEBUGMODE
	// 	printConnection(* connection, "DEBUGMODE parseInputData printConnection", 0);
	// #endif
	return true;
}

bool ServerRouter::_findConnectionСontentLength(t_connection * connection)
{
	// for (std::map<std::string, std::string>::iterator iterM = connection->inputData.headerFields.begin(); iterM != connection->inputData.headerFields.end(); iterM++)
	// {
	// 	if ((*iterM).first == "Content-Length")
	// 		connection->сontentLength = stoll ((*iterM).second);
	// }
	for (std::vector<std::pair<std::string, std::string> >::iterator iterM = connection->inputData.headerFieldsVec.begin(); iterM != connection->inputData.headerFieldsVec.end(); iterM++)
	{
		if ((*iterM).first == "Content-Length")
		{
			connection->сontentLength = stoll ((*iterM).second);
			// #ifdef DEBUGMODE
			// 	std::cout << " DEBUGMODE SR_PID _findConnectionсontentLength \n(*iterM).second: " << (*iterM).second << "\n----------------------" << std::endl;
			// 	std::cout << " DEBUGMODE SR_PID _findConnectionсontentLength \nconnection->сontentLength: " << connection->сontentLength << "\n----------------------" << std::endl;
			// #endif
			return true;
		}
	}
	return false;
}

void ServerRouter::_parseParamsStr(t_connection * connection)
{
	if (connection->inputData.addressParamsStr != "")
	{
		std::vector<std::string> addressParamsStrVec, splitStr;
		splitStringStr(connection->inputData.addressParamsStr, "&", addressParamsStrVec);
		for (std::vector<std::string>::iterator iter = addressParamsStrVec.begin(); iter != addressParamsStrVec.end(); iter++)
		{
			splitStringColon(*iter, '=', splitStr);
			if (splitStr.size() == 1)
				connection->inputData.headerFieldsVec.push_back(std::make_pair(splitStr[0], ""));
			else if (splitStr.size() == 2)
				connection->inputData.headerFieldsVec.push_back(std::make_pair(splitStr[0], splitStr[1]));
		}
	}
}
