#include "ServerRouter.hpp"

void ServerRouter::_prepareGetAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = GET;
	Server server = _getServer(connection->srvNbr);

	std::string contentTypeAndLengthAndData = "";
	if (!_addFileToAnswer(connection, contentTypeAndLengthAndData))
	{
		_addStatusCodePage(connection, contentTypeAndLengthAndData); // Подумать, что вернуть, если не откроется файл
	}

	connection->responseData.connectionAnswer += connection->responseData.statusCode + " " \
	+ connection->responseStatusCodesAll[connection->responseData.statusCode] + DELIMETER \
	+ timeStampHeader() + DELIMETER \
	+ "Server: \"" + WEBSERV_NAME + "\"" + DELIMETER \
	+ contentTypeAndLengthAndData;
}

bool ServerRouter::_addFileToAnswer(t_connection * connection, std::string & contentTypeAndLengthAndData)
{
	if (connection->inputData.address == "setCookies")
	{
		std::string setCookies = "<html><head><title>Set cookies</title><meta http-equiv=\"set-cookie\" content=\"name=mycookie; lang=en; expires=Saturday, 31 Dec 2022 15:25:00 GMT\" /></head><body><h2>Cookies set successfully</h2></body></html>";
		contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
		contentTypeAndLengthAndData += DELIMETER;
		contentTypeAndLengthAndData += "Content-Length: " + std::to_string(setCookies.length());
		contentTypeAndLengthAndData += DDELIMETER + setCookies + DDELIMETER;
		return true;
	}

	std::string msg;
	Server server = _getServer(connection->srvNbr);

	std::string pathToServer = "./";

	size_t i;
	if (server.getConfig().root != "")
		pathToServer += server.getConfig().root + "/";
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		if (connection->inputData.address == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				pathToServer += server.getConfig().locations[i].root + "/";
		}
	}
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_GET _addFileToAnswer pathToServer \n0pathToServer: " << NC << pathToServer << "\n----------------------" << std::endl;
	// #endif
	std::string path = pathToServer + connection->inputData.address;
	_findReferer(connection);
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_GET _addFileToAnswer refer \nReferer: " << NC << connection->referer << "\n----------------------" << std::endl;
	// #endif

	const char * pathChar = path.c_str();
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_GET _addFileToAnswer path \n1path: " << NC << path << "\n----------------------" << std::endl;
	// #endif

	struct stat buf;
	
	lstat(pathChar, & buf);
	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL)
	{
		std::vector<std::string> pathWithoutRefererVec;

		if (connection->referer != "")
		{
			splitStringStr(connection->referer, server.getConfig().port, pathWithoutRefererVec); 

			std::string pathWithoutRefererTmp = pathWithoutRefererVec[1];
			pathWithoutRefererTmp.erase(pathWithoutRefererTmp.begin());
			size_t posLast = findLastSlashInAddress(pathWithoutRefererTmp);
			std::string pathWithoutReferer = pathWithoutRefererTmp.substr(0, posLast);
			#ifdef DEBUGMODE
				std::cout << BLUE << " DEBUGMODE SR_GET _addFileToAnswer pathWithoutReferer \npathWithoutReferer: " << NC << pathWithoutReferer << "\n----------------------" << std::endl;
			#endif
			#ifdef DEBUGMODE
				std::cout << BLUE << " DEBUGMODE SR_GET _addFileToAnswer connection->inputData.address \nconnection->inputData.address: " << NC << connection->inputData.address << "\n----------------------" << std::endl;
			#endif
			pathWithoutRefererVec.clear();
			// #ifdef DEBUGMODE
			// 	std::cout << GREEN << " DEBUGMODE SR_GET _addFileToAnswer just_point \nconnection->inputData.address: " << NC << "just_point\n----------------------" << std::endl;
			// #endif
			splitStringStr(connection->inputData.address, pathWithoutReferer, pathWithoutRefererVec);
			#ifdef DEBUGMODE
				std::cout << BLUE << " DEBUGMODE SR_GET _addFileToAnswer pathWithoutRefererVec \npathWithoutRefererVec[0]: " << NC << pathWithoutRefererVec[0] << GREEN << "\tpathWithoutRefererVec[1]: " << NC << pathWithoutRefererVec[1] << "\n----------------------" << std::endl;
			#endif
		
			connection->responseData.statusCode = "404";
			_findPathToStatusCodePage(connection);
			posLast = findLastSlashInAddress(connection->pathToStatusCode);
			std::string pathToErrFolder = connection->pathToStatusCode.substr(0, posLast);
			std::string pathNew = (pathWithoutRefererVec[0] == "") ? (pathToErrFolder + "/" + pathWithoutRefererVec[1]) : (pathToErrFolder + "/" + pathWithoutRefererVec[0]);

			#ifdef DEBUGMODE
				std::cout << BLUE << " DEBUGMODE SR_GET _addFileToAnswer connection->pathToStatusCode \nconnection->pathToStatusCode: " << NC << connection->pathToStatusCode << "\n----------------------" << std::endl;
			#endif
			#ifdef DEBUGMODE
				std::cout << BLUE << " DEBUGMODE SR_GET _addFileToAnswer pathToErrFolder \npathToErrFolder: " << NC << pathToErrFolder << "\n----------------------" << std::endl;
			#endif
			#ifdef DEBUGMODE
				std::cout << BLUE << " DEBUGMODE SR_GET _addFileToAnswer pathNew \npathNew: " << NC << pathNew << "\n----------------------" << std::endl;
			#endif
			const char * pathChar2 = pathNew.c_str();
			struct stat buf2;
			lstat(pathChar2, & buf2);
			FILE * fileOpen2 = fopen(pathChar2, "rb"); //r - read only, b - in binary
			if (fileOpen2 == NULL)
			{
				if (_isPathAutoindex(connection))
				{
					msg = "autoindex from path \"" + connection->inputData.address + "\", sd ";
					printMsg(connection->srvNbr, connection->clntSd, msg, "");
					printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
					connection->responseData.statusCode = "200";
					std::string htmlStr = _createAutoindex(connection);
					connection->responseData.fileToSendInBinary.clear();
					connection->responseData.fileToSendInBinary = htmlStr;
					contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
					contentTypeAndLengthAndData +=  DELIMETER;
					contentTypeAndLengthAndData +=  "Content-Length: " + std::to_string(htmlStr.size());
					contentTypeAndLengthAndData +=  DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
					return true;
				}
				msg = "Error! Can not open the file " + pathNew + ", sd ";
				printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
				connection->responseData.statusCode = "404";
				return false;
			}
			else if (S_ISREG(buf2.st_mode))  //it's path to file
			{
				msg = "the file " + path + " was sucsessfully opened, sd ";
				printMsg(connection->srvNbr, connection->clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
				connection->responseData.statusCode = "200";

				fseek(fileOpen2, 0L, SEEK_END); // перемотать на конец файла
				size_t fileLength2 = ftell(fileOpen2);
				// std::cout << "fileLength2: " << fileLength2 << std::endl;

				std::string pathTmp = path;
				pathTmp.erase(pathTmp.begin());
				size_t dot = pathTmp.find('.');
				std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
				// std::cout << RED << "ext = " << ext << NC << std::endl;

				std::string contType;
				if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
					contType = connection->contentTypesAll[ext];
				else
					contType = "text/html";

				fclose(fileOpen2);

				std::string nname = pathTmp.substr(0, dot);
				size_t nnpos = findLastSlashInAddress(nname);
				nname = nname.substr(nnpos + 1);

				if (ext == "py" || ext == "rb")
				{
					std::string pyStr = execPy(pathChar);
					contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
					contentTypeAndLengthAndData += DELIMETER;
					contentTypeAndLengthAndData += "Content-Length: " + std::to_string(pyStr.length());
					contentTypeAndLengthAndData += DDELIMETER + pyStr + DDELIMETER;
				}
				else 
				{
					readFileToStrInBinary(pathChar2, connection->responseData.fileToSendInBinary);
					contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength2) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
				}
			}
		}
		else
		{
			if (_isPathAutoindex(connection))
			{
				msg = "autoindex from path \"" + connection->inputData.address + "\", sd ";
				printMsg(connection->srvNbr, connection->clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
				connection->responseData.statusCode = "200";
				std::string htmlStr = _createAutoindex(connection);
				connection->responseData.fileToSendInBinary.clear();
				connection->responseData.fileToSendInBinary = htmlStr;
				contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
				contentTypeAndLengthAndData +=  DELIMETER;
				contentTypeAndLengthAndData +=  "Content-Length: " + std::to_string(htmlStr.size());
				contentTypeAndLengthAndData +=  DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
				return true;
			}
			msg = "Error! Can not open the file " + path + ", sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "404";
			return false;
		}
	}
	else if (S_ISREG(buf.st_mode)) //it's path to file
	{
		msg = "the file " + path + " was sucsessfully opened, sd ";
		printMsg(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "200";

		fseek(fileOpen, 0L, SEEK_END); // перемотать на конец файла
		size_t fileLength = ftell(fileOpen);
		// std::cout << "fileLength: " << fileLength << std::endl;

		std::string pathTmp = path;
		pathTmp.erase(pathTmp.begin());
		size_t dot = pathTmp.find('.');
		std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
		// std::cout << RED << "ext = " << ext << NC << std::endl;

		std::string contType;
		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
			contType = connection->contentTypesAll[ext];
		else
			contType = "text/html";
		fclose(fileOpen);

		std::string nname = pathTmp.substr(0, dot);
		size_t nnpos = findLastSlashInAddress(nname);
		nname = nname.substr(nnpos + 1);

		if (ext == "py" || ext == "rb")
		{
			std::string pyStr = execPy(pathChar);
			contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
			contentTypeAndLengthAndData += DELIMETER;
			contentTypeAndLengthAndData += "Content-Length: " + std::to_string(pyStr.length());
			contentTypeAndLengthAndData += DDELIMETER + pyStr + DDELIMETER;
		}
		else
		{
			readFileToStrInBinary(pathChar, connection->responseData.fileToSendInBinary);
			contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
		}
	}
	else if (S_ISDIR(buf.st_mode)) //it's path to dir
	{
		fclose(fileOpen);
		size_t i;
		for (i = 0; i < server.getConfig().locations.size(); i++)
		{
			if (connection->inputData.address == server.getConfig().locations[i].path)
			{
				if (server.getConfig().locations[i].index != "")
					path += server.getConfig().locations[i].index;
				else
					path += "index.html";
				break;
			}
		}
		if (i == server.getConfig().locations.size())
		{
			if (server.getConfig().index != "")
				path += server.getConfig().index;
			else
				path += "index.html";
		}
		// std::cout << BLUE << " SR_GET _addFileToAnswer  path: " << NC << path << std::endl;

		const char * pathChar2 = path.c_str();
		struct stat buf2;
		lstat(pathChar2, & buf2);
		FILE * fileOpen2 = fopen(pathChar2, "rb"); //r - read only, b - in binary
		if (fileOpen2 == NULL)
		{
			if (_isPathAutoindex(connection))
			{
				msg = "autoindex from path \"" + connection->inputData.address + "\", sd ";
				printMsg(connection->srvNbr, connection->clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
				connection->responseData.statusCode = "200";
				std::string htmlStr = _createAutoindex(connection);
				connection->responseData.fileToSendInBinary.clear();
				connection->responseData.fileToSendInBinary = htmlStr;
				contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
				contentTypeAndLengthAndData +=  DELIMETER;
				contentTypeAndLengthAndData +=  "Content-Length: " + std::to_string(htmlStr.size());
				contentTypeAndLengthAndData +=  DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
				return true;
			}
			msg = "Error! Can not open the file " + path + ", sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "404";
			return false;
		}
		else if (S_ISREG(buf2.st_mode))  //it's path to file
		{
			msg = "the file " + path + " was sucsessfully opened, sd ";
			printMsg(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "200";

			fseek(fileOpen2, 0L, SEEK_END); // перемотать на конец файла
			size_t fileLength2 = ftell(fileOpen2);
			// std::cout << "fileLength2: " << fileLength2 << std::endl;

			std::string pathTmp = path;
			pathTmp.erase(pathTmp.begin());
			size_t dot = pathTmp.find('.');
			std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
			// std::cout << RED << "ext = " << ext << NC << std::endl;

			std::string contType;
			if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
				contType = connection->contentTypesAll[ext];
			else
				contType = "text/html";

			fclose(fileOpen2);

			std::string nname = pathTmp.substr(0, dot);
			size_t nnpos = findLastSlashInAddress(nname);
			nname = nname.substr(nnpos + 1);

			if (ext == "py" || ext == "rb")
			{
				std::string pyStr = execPy(pathChar);
				contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
				contentTypeAndLengthAndData += DELIMETER;
				contentTypeAndLengthAndData += "Content-Length: " + std::to_string(pyStr.length());
				contentTypeAndLengthAndData += DDELIMETER + pyStr + DDELIMETER;
			}
			else
			{
				readFileToStrInBinary(pathChar2, connection->responseData.fileToSendInBinary);
				contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength2) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
			}
		}
	}
	else
	{
		if (_isPathAutoindex(connection))
		{
			msg = "autoindex from path \"" + connection->inputData.address + "\", sd ";
			printMsg(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "200";
			std::string htmlStr = _createAutoindex(connection);
			connection->responseData.fileToSendInBinary.clear();
			connection->responseData.fileToSendInBinary = htmlStr;
			contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
			contentTypeAndLengthAndData +=  DELIMETER;
			contentTypeAndLengthAndData +=  "Content-Length: " + std::to_string(htmlStr.size());
			contentTypeAndLengthAndData +=  DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
			return true;
		}
		msg = "Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
		return false;
	}
	return true;
}
