#include "ServerRouter.hpp"

void ServerRouter::_prepareGetAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = GET;
	Server server = _getServer(connection->srvNbr);
	// bool correctAddr = false;
	// if (connection->inputData.address != "")
	// {
	// 	for (size_t i = 0; i < server.getConfig().locations.size(); i++)
	// 	{
	// 		if (connection->inputData.address == server.getConfig().locations[i].path)
	// 			correctAddr = true;
	// 	}
	// }
	// if (!correctAddr)
	// {
	// 	msg = "Error! Unknown location addr from sd ";
	// 	printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
	// 	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
	// 	connection->responseData.statusCode = "404";
	// }

	std::string contentTypeAndLengthAndData = "";
	// _addFileToAnswer(contentTypeAndLengthAndData, connection)
	if (!_addFileToAnswer(contentTypeAndLengthAndData, connection))
	{
		// std::cout << RED << "_prepareGetAnswer\n" << NC;
		_addFile404(contentTypeAndLengthAndData, connection); // Подумать, что вернуть, если не откроется файл
		// connection->responseData.connectionAnswer += "404 " \
		// + connection->responseStatusCodesAll["404"] + DELIMETER \
		// + "Location: resources/8080/404/404.html" + DELIMETER \
  		// + "Connection: Close\r\n\r\n";

	}

	connection->responseData.connectionAnswer += connection->responseData.statusCode \
	+ " " + connection->responseStatusCodesAll[connection->responseData.statusCode] \
	+ DELIMETER + "Server: \"" + WEBSERV_NAME + "\"" + DELIMETER \
	+ contentTypeAndLengthAndData;



}

void ServerRouter::_addFile404(std::string & contentTypeAndLengthAndData, t_connection * connection)
{
	std::string msg;
	bool found404 = false;
	Server server = _getServer(connection->srvNbr);
	// std::string path = server.getConfig().listen + connection->inputdata.address;
	// std::string path = "./" + connection->inputData.address;

	// std::string path = "./";
	std::string path = "";
	// std::string pathNew = path + "404.html";

	// size_t pos = 0;
	// size_t posLast = 0;
	// // while (pos != std::string::npos)
	// // {
	// // 	pos = connection->inputData.address.find("\\");
	// // }

	// for ( pos = connection->inputData.address.find("/", pos++); pos != std::string::npos; pos = connection->inputData.address.find("/", pos + 1))
	// 	posLast = pos;

	size_t posLast = findLastSlashInAddress(connection->inputData.address);
	// std::cout << RED << "posLast: " << posLast << NC << "\n";
	std::string addressFolder = connection->inputData.address.substr(0, posLast);

	if (server.getConfig().root != "")
		path += server.getConfig().root + "/";
	size_t i;
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		// std::cout << RED << addressFolder << "\t" << server.getConfig().locations[i].path << NC << "\n";
		if (addressFolder == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				path += server.getConfig().locations[i].root + "/";
			if (server.getConfig().locations[i].error_pages.size() > 0)
			{
				for (std::map<std::string, std::string>::const_iterator iter = server.getConfig().locations[i].error_pages.begin(); iter != server.getConfig().locations[i].error_pages.end(); iter++)
				{
					// std::cout << RED << "1(*iter).first == " << (*iter).first << "\t1(*iter).second == " << (*iter).second << NC << "\n";
					if ((*iter).first == "404" && (*iter).second != "")
					{
						path += (*iter).second;
						found404 = true;
					}
				}

			}
		}
	}
	if (!found404)
	{
		if (server.getConfig().error_pages.size() > 0)
		{
			for (std::map<std::string, std::string>::const_iterator iter = server.getConfig().error_pages.begin(); iter != server.getConfig().error_pages.end(); iter++)
			{
				// std::cout << RED << "(*iter).first == " << (*iter).first << "\t(*iter).second == " << (*iter).second << NC << "\n";
				if ((*iter).first == "404" && (*iter).second != "")
				{
					path += (*iter).second;
					found404 = true;
				}
			}

		}
	}
	std::cout << RED << "path == " << path << NC << "\n";
	std::cout << RED << "connection->inputData.address == " << connection->inputData.address << NC << "\n";
	std::cout << RED << "addressFolder == " << addressFolder << NC << "\n";
	// connection->responseData.pathExcludeInErr = addressFolder;

	posLast = findLastSlashInAddress(path);
	// std::cout << RED << "posLast: " << posLast << NC << "\n";
	// connection->responseData.pathIncludeBeginInErr = path.substr(0, posLast);

	// std::string seperator = 
	//  splitStringStr(path, std::string seperator, connection->responseData.pathIncludeInErr);

	struct stat buf;
	std::string contType;
	path = "./" + path;
	const char * pathChar = path.c_str();

	FILE * fileOpen;
	
	lstat(pathChar, & buf);
	fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL || S_ISDIR(buf.st_mode))
	{
		// std::cout << RED << "fileOpen == " << fileOpen << NC << "\n";
		// msg = "Error! Can not open the file " + path + ", sd ";
		// printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		// printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		// connection->responseData.statusCode = "404";
		// std::ofstream fileToWrite;
		// std::ofstream ofile(pathNew);
		// fileToWrite.open(pathNew, std::ios::out | std::ios::in);
		std::string new404 = "<!DOCTYPE html><html lang=en><meta charset=utf-8><title>Error 404 (Not Found)!!</title><style>*{margin:0;padding:0}html,code{font:15px/22px arial,sans-serif}html{background:#fff;color:#222;padding:15px}body{margin:7% auto 0;max-width:390px;min-height:180px;padding:30px 0 15px}* > body{padding-right:205px}p{margin:11px 0 22px;overflow:hidden}ins{color:#777;text-decoration:none}a img{border:0}@media screen and (max-width:772px){body{background:none;margin-top:0;max-width:none;padding-right:0}}#logo{background:url(//www.google.com/images/branding/googlelogo/1x/googlelogo_color_150x54dp.png) no-repeat;margin-left:-5px}@media only screen and (min-resolution:192dpi){#logo{background:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) no-repeat 0% 0%/100% 100%;-moz-border-image:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) 0}}@media only screen and (-webkit-min-device-pixel-ratio:2){#logo{background:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) no-repeat;-webkit-background-size:100% 100%}}#logo{display:inline-block;height:54px;width:150px}</style><p><b>404.</b> <ins>That’s an error.</ins><p>The requested URL <code>" + connection->inputData.address + "</code> was not found on this server.  <ins>That’s all we know.</ins>";
		// fileToWrite << new404;
		// fileToWrite.close();
		// contType = "text/html";
		// t_config config = server.getConfig();

		// for (i = 0; i < config.locations.size(); i++)
		// {
		// 	if (connection->inputData.address == config.locations[i].path)
		// 		config.error_pages.insert(std::make_pair("404", "404.html"));
		// }
		// server.setConfig(config);
		// pathChar = pathNew.c_str();
		// lstat(pathChar, & buf);
		// fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
		// readFileToStrInBinary(pathChar, connection->responseData.fileToSendInBinary);
		connection->responseData.fileToSendInBinary.clear();
		connection->responseData.fileToSendInBinary = new404;

		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(new404.size()) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
	}

	else if (S_ISREG(buf.st_mode)) //it's path to file
	{
		fseek(fileOpen, 0L, SEEK_END); // перемотать на конец файла
		size_t fileLength = ftell(fileOpen);
		// std::cout << "fileLength: " << fileLength << std::endl;

		std::string pathTmp = path;
		pathTmp.erase(pathTmp.begin());
		size_t dot = pathTmp.find('.');
		std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
		std::cout << RED << "ext = " << ext << NC << std::endl;

		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
			contType = connection->contentTypesAll[ext];
		else
			contType = "text/html";
		fclose(fileOpen);
		readFileToStrInBinary(pathChar, connection->responseData.fileToSendInBinary);
		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
	}


}

bool ServerRouter::_addFileToAnswer(std::string & contentTypeAndLengthAndData, t_connection * connection)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	// std::string path = server.getConfig().listen + connection->inputdata.address;
	// std::string path = "./" + connection->inputData.address;

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
	// 		if (server.getConfig().locations[i].index != "")
	// 			path += server.getConfig().locations[i].index;
	// 		else
	// 			path += "index.html";
	// 		break;
		}
	}
	// if (i == server.getConfig().locations.size())
	// {
	// 	if (server.getConfig().index != "")
	// 		path += server.getConfig().index;
	// 	else
	// 		path += "index.html";
	// }

	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_GET _addFileToAnswer pathToServer \n0pathToServer: " << NC << pathToServer << "\n------------" << std::endl;
	#endif
	std::string path = pathToServer + connection->inputData.address;
	_findReferer(connection);
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_GET _addFileToAnswer refer \nReferer: " << NC << connection->referer << "\n------------" << std::endl;
	#endif
	// std::string fileName = 

	// size_t i;
	// for (i = 0; i < server.getConfig().locations.size(); i++)
	// {
	// 	if (connection->inputdata.address == server.getConfig().locations[i].path)
	// 	{
	// 		if (server.getConfig().locations[i].root != "")
	// 			path += server.getConfig().locations[i].root;
	// 		if (server.getConfig().locations[i].index != "")
	// 			path += server.getConfig().locations[i].index;
	// 		break;
	// 	}
	// }
	// if (i == server.getConfig().locations.size())
	// {
	// 	if (server.getConfig().root != "")
	// 		path += server.getConfig().root;
	// 	if (server.getConfig().index != "")
	// 		path += server.getConfig().index;
	// }

	const char * pathChar = path.c_str();
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_GET _addFileToAnswer path \n1path: " << NC << path << "\n------------" << std::endl;
	// #endif

	struct stat buf;
	
	lstat(pathChar, & buf);
	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL)
	{
		//ИСПОЛЬЗОВАТЬ referer
		// if (_ifErrPages(connection))
		// {
		// 	std::vector<std::string> errPathParts;
	 	// 	splitStringStr(path, connection->responseData.pathExcludeInErr, errPathParts);
		// 	path = "./" + connection->responseData.pathIncludeBeginInErr + "/" + errPathParts[1];
		// 	std::cout << GREEN << "PATH in ERR: " << NC << path << std::endl;
		// }
		// else
		// {
			msg = "1Error! Can not open the file " + path + ", sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "404";
		// }
		return false;
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
		std::cout << RED << "ext = " << ext << NC << std::endl;

		std::string contType;
		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
			contType = connection->contentTypesAll[ext];
		else
			contType = "text/html";
		fclose(fileOpen);

		readFileToStrInBinary(pathChar, connection->responseData.fileToSendInBinary);

		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
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
		std::cout << RED << path << NC << std::endl;

		const char * pathChar2 = path.c_str();
		struct stat buf2;
		lstat(pathChar2, & buf2);
		FILE * fileOpen2 = fopen(pathChar2, "rb"); //r - read only, b - in binary
		if (fileOpen2 == NULL)
		{
			msg = "2Error! Can not open the file " + path + ", sd ";
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
			std::cout << RED << "ext = " << ext << NC << std::endl;

			std::string contType;
			if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
				contType = connection->contentTypesAll[ext];
			else
				contType = "text/html";

			fclose(fileOpen2);

			readFileToStrInBinary(pathChar2, connection->responseData.fileToSendInBinary);

			contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength2) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
		}
	}
	else
	{
		msg = "3Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
		return false;
	}
	return true;
}

// bool ServerRouter::_addFileToAnswer(std::string & contentTypeAndLengthAndData, t_connection * connection)
// {
// 	std::string msg;
// 	Server server = _getServer(connection->srvNbr);
// 	// std::string path = server.getConfig().listen + connection->inputdata.address;
// 	// std::string path = "./" + connection->inputData.address;

// 	std::string path = "./";

// 	size_t i;
// 	if (server.getConfig().root != "")
// 		path += server.getConfig().root + "/";
// 	for (i = 0; i < server.getConfig().locations.size(); i++)
// 	{
// 		if (connection->inputData.address == server.getConfig().locations[i].path)
// 		{
// 			if (server.getConfig().locations[i].root != "")
// 				path += server.getConfig().locations[i].root + "/";
// 	// 		if (server.getConfig().locations[i].index != "")
// 	// 			path += server.getConfig().locations[i].index;
// 	// 		else
// 	// 			path += "index.html";
// 	// 		break;
// 		}
// 	}
// 	// if (i == server.getConfig().locations.size())
// 	// {
// 	// 	if (server.getConfig().index != "")
// 	// 		path += server.getConfig().index;
// 	// 	else
// 	// 		path += "index.html";
// 	// }

// 	path += connection->inputData.address;

// 	// size_t i;
// 	// for (i = 0; i < server.getConfig().locations.size(); i++)
// 	// {
// 	// 	if (connection->inputdata.address == server.getConfig().locations[i].path)
// 	// 	{
// 	// 		if (server.getConfig().locations[i].root != "")
// 	// 			path += server.getConfig().locations[i].root;
// 	// 		if (server.getConfig().locations[i].index != "")
// 	// 			path += server.getConfig().locations[i].index;
// 	// 		break;
// 	// 	}
// 	// }
// 	// if (i == server.getConfig().locations.size())
// 	// {
// 	// 	if (server.getConfig().root != "")
// 	// 		path += server.getConfig().root;
// 	// 	if (server.getConfig().index != "")
// 	// 		path += server.getConfig().index;
// 	// }

// 	const char * pathChar = path.c_str();
// 	struct stat buf;
	
// 	lstat(pathChar, & buf);
// 	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
// 	if (fileOpen == NULL)
// 	{
// 		msg = "Error! Can not open the file " + path + ", sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "404";
// 	}
// 	else if (S_ISREG(buf.st_mode)) //it's path to file
// 	{
// 		msg = "the file " + path + " was sucsessfully opened, sd ";
// 		printMsg(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "200";

// 		fseek(fileOpen, 0L, SEEK_END); // перемотать на конец файла
// 		size_t fileLength = ftell(fileOpen);
// 		std::cout << "fileLength: " << fileLength << std::endl;

// 		std::string pathTmp = path;
// 		pathTmp.erase(pathTmp.begin());
// 		size_t dot = pathTmp.find('.');
// 		std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
// 		// std::string ext = path.substr(path.find('.'), 6);
// 		std::cout << RED << "ext = " << ext << NC << std::endl;
// 		std::string contType;
// 		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
// 			contType = connection->contentTypesAll[ext];
// 		else
// 			contType = "text/html";

// 		// exec(file --brief --mime-type path);

// 		char * buffer = (char *)malloc(sizeof(char) * fileLength);
// 		if (!buffer)
// 		{
// 			fclose(fileOpen);
// 			// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
// 			return false;
// 		}
// 		// std::cout << "fileLength: " << fileLength << std::endl;
// 		// std::cout << "buffer.size(): " << sizeof(buffer) << std::endl;
// 		fseek(fileOpen, 0L, SEEK_SET); // перейти на начало файла
// 		if (fileLength != fread(buffer, 1, fileLength, fileOpen))
// 		{
// 			free(buffer);
// 			fclose(fileOpen);
// 			// fputs( "Read data size is not equal to actual file size.", stderr );
// 			return false;
// 		}

// 		connection->responseData.fileToSendInBinary = buffer;
// 		// std::cout << "fileToSendInBinary.size(): " << connection->responseData.fileToSendInBinary.size() << std::endl;
// 		// std::cout << "buffer: " << buffer << std::endl;
// 		fclose(fileOpen);
// 		free(buffer);

// 		std::string line, S;
// 		S = "";
// 		std::ifstream in(pathChar, std::ios::binary);
// 		if (in.is_open())
// 		{
// 			while (getline(in, line))
// 			{
// 				// std::cout << line << std::endl;
// 				S += line + "\n";
// 			}
// 		}
// 		in.close();
// 		// std::cout << "S:\n" << S << "\n";
// 		connection->responseData.fileToSendInBinary = S;

// 		// readFileToStrInBinary(pathChar, connection->responseData.fileToSendInBinary);

// 		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
// 	}
// 	else if (S_ISDIR(buf.st_mode)) //it's path to dir
// 	{
// 		//  + connection->inputData.address
// 		fclose(fileOpen);
// 		size_t i;
// 		for (i = 0; i < server.getConfig().locations.size(); i++)
// 		{
// 			if (connection->inputData.address == server.getConfig().locations[i].path)
// 			{
// 				// if (server.getConfig().locations[i].root != "")
// 				// 	path += server.getConfig().locations[i].root + "//";
// 				if (server.getConfig().locations[i].index != "")
// 					path += server.getConfig().locations[i].index;
// 				else
// 					path += "index.html";
// 				break;
// 			}
// 		}
// 		if (i == server.getConfig().locations.size())
// 		{
// 			if (server.getConfig().index != "")
// 				path += server.getConfig().index;
// 			else
// 				path += "index.html";
// 		}
// 		std::cout << RED << path << NC << std::endl;

// 		const char * pathChar2 = path.c_str();
// 		struct stat buf2;
// 		lstat(pathChar2, & buf2);
// 		FILE * fileOpen2 = fopen(pathChar2, "rb"); //r - read only, b - in binary
// 		if (fileOpen2 == NULL)
// 		{
// 			msg = "Error! Can not open the file " + path + ", sd ";
// 			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 			connection->responseData.statusCode = "404";
// 		}
// 		if (S_ISREG(buf2.st_mode))  //it's path to file
// 		{
// 			std::cout << RED << "HI!!!" << NC << std::endl;
// 			msg = "the file " + path + " was sucsessfully opened, sd ";
// 			printMsg(connection->srvNbr, connection->clntSd, msg, "");
// 			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 			connection->responseData.statusCode = "200";

// 			fseek(fileOpen2, 0L, SEEK_END); // перемотать на конец файла
// 			size_t fileLength2 = ftell(fileOpen2);
// 			std::cout << "fileLength2: " << fileLength2 << std::endl;

// 			size_t dot = path.find(".");
// 			std::string ext = path.substr(dot + 1, path.length() - dot);
// 			std::string contType;
// 			if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
// 				contType = connection->contentTypesAll[ext];
// 			else
// 				contType = "text/html";

// 			char * buffer2 = (char *)malloc(fileLength2);
// 			if (!buffer2)
// 			{
// 				fclose(fileOpen2);
// 				// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
// 				return false;
// 			}
// 			fseek(fileOpen2, 0L, SEEK_SET); // перейти на начало файла
// 			if (fileLength2 != fread(buffer2, 1, fileLength2, fileOpen2))
// 			{
// 				free(buffer2);
// 				fclose(fileOpen2);
// 				// fputs( "Read data size is not equal to actual file size.", stderr );
// 				return false;
// 			}

// 			connection->responseData.fileToSendInBinary = buffer2;
// 			fclose(fileOpen2);
// 			free(buffer2);

// 			readFileToStrInBinary(pathChar2, connection->responseData.fileToSendInBinary);

// 			contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength2) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
// 		}
// 	}
// 	else
// 	{
// 		msg = "Error! Can not open the file " + path + ", sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "404";
// 	}
// 	return true;
// }

// bool ServerRouter::_addFileToAnswer(std::string & contentTypeAndLengthAndData, t_connection * connection)
// {
// 	std::string msg;
// 	Server server = _getServer(connection->srvNbr);
// 	// std::string path = server.getConfig().listen + connection->inputdata.address;
// 	// std::string path = "./" + connection->inputData.address;

// 	std::string path = "./";

// 	size_t i;
// 	if (server.getConfig().root != "")
// 		path += server.getConfig().root + "/";
// 	for (i = 0; i < server.getConfig().locations.size(); i++)
// 	{
// 		if (connection->inputData.address == server.getConfig().locations[i].path)
// 		{
// 			if (server.getConfig().locations[i].root != "")
// 				path += server.getConfig().locations[i].root + "/";
// 	// 		if (server.getConfig().locations[i].index != "")
// 	// 			path += server.getConfig().locations[i].index;
// 	// 		else
// 	// 			path += "index.html";
// 	// 		break;
// 		}
// 	}
// 	// if (i == server.getConfig().locations.size())
// 	// {
// 	// 	if (server.getConfig().index != "")
// 	// 		path += server.getConfig().index;
// 	// 	else
// 	// 		path += "index.html";
// 	// }

// 	path += connection->inputData.address;

// 	// size_t i;
// 	// for (i = 0; i < server.getConfig().locations.size(); i++)
// 	// {
// 	// 	if (connection->inputdata.address == server.getConfig().locations[i].path)
// 	// 	{
// 	// 		if (server.getConfig().locations[i].root != "")
// 	// 			path += server.getConfig().locations[i].root;
// 	// 		if (server.getConfig().locations[i].index != "")
// 	// 			path += server.getConfig().locations[i].index;
// 	// 		break;
// 	// 	}
// 	// }
// 	// if (i == server.getConfig().locations.size())
// 	// {
// 	// 	if (server.getConfig().root != "")
// 	// 		path += server.getConfig().root;
// 	// 	if (server.getConfig().index != "")
// 	// 		path += server.getConfig().index;
// 	// }

// 	const char * pathChar = path.c_str();
// 	struct stat buf;
	
// 	lstat(pathChar, & buf);
// 	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
// 	if (fileOpen == NULL)
// 	{
// 		msg = "Error! Can not open the file " + path + ", sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "404";
// 	}
// 	else if (S_ISREG(buf.st_mode)) //it's path to file
// 	{
// 		msg = "the file " + path + " was sucsessfully opened, sd ";
// 		printMsg(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "200";

// 		fseek(fileOpen, 0L, SEEK_END); // перемотать на конец файла
// 		size_t fileLength = ftell(fileOpen);
// 		std::cout << "fileLength: " << fileLength << std::endl;

// 		std::string pathTmp = path;
// 		pathTmp.erase(pathTmp.begin());
// 		size_t dot = pathTmp.find('.');
// 		std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
// 		// std::string ext = path.substr(path.find('.'), 6);
// 		std::cout << RED << "ext = " << ext << NC << std::endl;
// 		std::string contType;
// 		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
// 			contType = connection->contentTypesAll[ext];
// 		else
// 			contType = "text/html";

// 		// exec(file --brief --mime-type path);

// 		char * buffer = (char *)malloc(sizeof(char) * fileLength);
// 		if (!buffer)
// 		{
// 			fclose(fileOpen);
// 			// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
// 			return false;
// 		}
// 		// std::cout << "fileLength: " << fileLength << std::endl;
// 		// std::cout << "buffer.size(): " << sizeof(buffer) << std::endl;
// 		fseek(fileOpen, 0L, SEEK_SET); // перейти на начало файла
// 		if (fileLength != fread(buffer, 1, fileLength, fileOpen))
// 		{
// 			free(buffer);
// 			fclose(fileOpen);
// 			// fputs( "Read data size is not equal to actual file size.", stderr );
// 			return false;
// 		}

// 		connection->responseData.fileToSendInBinary = buffer;
// 		// std::cout << "fileToSendInBinary.size(): " << connection->responseData.fileToSendInBinary.size() << std::endl;
// 		// std::cout << "buffer: " << buffer << std::endl;
// 		fclose(fileOpen);
// 		free(buffer);

// 		// // char * S = (char *)malloc(sizeof(char) * fileLength);     //Символьный массив в 255 символов
//     	// char S[1598740] = {};
// 		// /*НАЧАЛО РАБОТЫ С ФАЙЛОМ*/
// 		// // if (!S)
// 		// // {
// 		// // 	// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
// 		// // 	return false;
// 		// // }
// 		// std::ifstream in(pathChar, std::ios::binary);
// 		// in.read((char*)&S, sizeof(S));          //перенос байтов из файла в "х"
// 		// // in.read((char*)S, sizeof(S));          //в случае с массивом можно и так
// 		// in.close();
// 		// /*КОНЕЦ РАБОТЫ С ФАЙЛОМ*/
// 		// std::cout << S << '\n';
// 		// // free (S);
// 		// connection->responseData.fileToSendInBinary = S;
// 		// std::cout << "std::to_string(fileLength): " << std::to_string(fileLength) << "\n";
// 		// std::cout << "pathChar: " << pathChar <<"\n";
// 		std::string line, S;
// 		S = "";
// 		std::ifstream in(pathChar, std::ios::binary);
// 		if (in.is_open())
// 		{
// 			while (getline(in, line))
// 			{
// 				// std::cout << line << std::endl;
// 				S += line + "\n";
// 			}
// 		}
// 		in.close();
// 		// std::cout << "S:\n" << S << "\n";
// 		connection->responseData.fileToSendInBinary = S;

// 		// std::string ff = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
// 		// ff += "\t\t<title>The Periodic Table</title>\n\t</head>\n\t<body>\n";
// 		// ff += "\t\t<h2>Dmitri Mendeleev\'s periodic table of the elements</h2>\n\t\t<br>\n\t\t<table style = 'border: 1px blue solid;'>\n";
// 		// ff += "\t\t</table>\n\t</body>\n</html>";

// 		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
// 		// contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(ff.size()) + DDELIMETER + ff + DDELIMETER;
// 		// std::stringstream bufFile;
// 		// bufFile << fileOpen.rdbuf();
// 	}
// 	else if (S_ISDIR(buf.st_mode)) //it's path to dir
// 	{
// 		//  + connection->inputData.address
// 		fclose(fileOpen);
// 		size_t i;
// 		for (i = 0; i < server.getConfig().locations.size(); i++)
// 		{
// 			if (connection->inputData.address == server.getConfig().locations[i].path)
// 			{
// 				// if (server.getConfig().locations[i].root != "")
// 				// 	path += server.getConfig().locations[i].root + "//";
// 				if (server.getConfig().locations[i].index != "")
// 					path += server.getConfig().locations[i].index;
// 				else
// 					path += "index.html";
// 				break;
// 			}
// 		}
// 		if (i == server.getConfig().locations.size())
// 		{
// 			if (server.getConfig().index != "")
// 				path += server.getConfig().index;
// 			else
// 				path += "index.html";
// 		}
// 		std::cout << RED << path << NC << std::endl;

// 		const char * pathChar2 = path.c_str();
// 		struct stat buf2;
// 		lstat(pathChar2, & buf2);
// 		FILE * fileOpen2 = fopen(pathChar2, "rb"); //r - read only, b - in binary
// 		if (fileOpen2 == NULL)
// 		{
// 			msg = "Error! Can not open the file " + path + ", sd ";
// 			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 			connection->responseData.statusCode = "404";
// 		}
// 		if (S_ISREG(buf2.st_mode))  //it's path to file
// 		{
// 			std::cout << RED << "HI!!!" << NC << std::endl;
// 			msg = "the file " + path + " was sucsessfully opened, sd ";
// 			printMsg(connection->srvNbr, connection->clntSd, msg, "");
// 			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 			connection->responseData.statusCode = "200";

// 			fseek(fileOpen2, 0L, SEEK_END); // перемотать на конец файла
// 			size_t fileLength2 = ftell(fileOpen2);
// 			std::cout << "fileLength2: " << fileLength2 << std::endl;

// 			size_t dot = path.find(".");
// 			std::string ext = path.substr(dot + 1, path.length() - dot);
// 			std::string contType;
// 			if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
// 				contType = connection->contentTypesAll[ext];
// 			else
// 				contType = "text/html";

// 			char * buffer2 = (char *)malloc(fileLength2);
// 			if (!buffer2)
// 			{
// 				fclose(fileOpen2);
// 				// fputs( "Could not allocate memory for file buffer. File could be empty or too large.", stderr );
// 				return false;
// 			}
// 			fseek(fileOpen2, 0L, SEEK_SET); // перейти на начало файла
// 			if (fileLength2 != fread(buffer2, 1, fileLength2, fileOpen2))
// 			{
// 				free(buffer2);
// 				fclose(fileOpen2);
// 				// fputs( "Read data size is not equal to actual file size.", stderr );
// 				return false;
// 			}

// 			connection->responseData.fileToSendInBinary = buffer2;
// 			fclose(fileOpen2);
// 			free(buffer2);

// 			// std::string ff = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
// 			// ff += "\t\t<title>The Periodic Table</title>\n\t</head>\n\t<body>\n";
// 			// ff += "\t\t<h2>Dmitri Mendeleev\'s periodic table of the elements</h2>\n\t\t<br>\n\t\t<table style = 'border: 1px blue solid;'>\n";
// 			// ff += "\t\t</table>\n\t</body>\n</html>";

// 			contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength2) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
// 			// contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(ff.size()) + DDELIMETER + ff + DDELIMETER;
// 			// std::stringstream bufFile;
// 			// bufFile << fileOpen2.rdbuf();
// 		}
// 	}
// 	else
// 	{
// 		msg = "Error! Can not open the file " + path + ", sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "404";
// 	}
// 	return true;
// }

// void ServerRouter::_addFileToAnswer(std::string & contentTypeAndLengthAndData, t_connection * connection)
// {
// 	std::string msg;
// 	Server server = _getServer(connection->srvNbr);
// 	// std::string path = server.getConfig().listen + connection->inputdata.address;
// 	std::string path = "." + connection->inputData.address;

// 	size_t i;
// 	if (server.getConfig().root != "")
// 		path += server.getConfig().root;
// 	for (i = 0; i < server.getConfig().locations.size(); i++)
// 	{
// 		if (connection->inputData.address == server.getConfig().locations[i].path)
// 		{
// 			if (server.getConfig().locations[i].root != "")
// 				path += server.getConfig().locations[i].root;
// 			if (server.getConfig().locations[i].index != "")
// 				path += server.getConfig().locations[i].index;
// 			else
// 				path += "index.html";
// 			break;
// 		}
// 	}
// 	if (i == server.getConfig().locations.size())
// 	{
// 		if (server.getConfig().index != "")
// 			path += server.getConfig().index;
// 		else
// 			path += "index.html";
// 	}

// 	// size_t i;
// 	// for (i = 0; i < server.getConfig().locations.size(); i++)
// 	// {
// 	// 	if (connection->inputdata.address == server.getConfig().locations[i].path)
// 	// 	{
// 	// 		if (server.getConfig().locations[i].root != "")
// 	// 			path += server.getConfig().locations[i].root;
// 	// 		if (server.getConfig().locations[i].index != "")
// 	// 			path += server.getConfig().locations[i].index;
// 	// 		break;
// 	// 	}
// 	// }
// 	// if (i == server.getConfig().locations.size())
// 	// {
// 	// 	if (server.getConfig().root != "")
// 	// 		path += server.getConfig().root;
// 	// 	if (server.getConfig().index != "")
// 	// 		path += server.getConfig().index;
// 	// }

// 	const char * pathChar = path.c_str();
// 	struct stat buf;
	
// 	lstat(pathChar, & buf);
// 	FILE * file = fopen(pathChar, "rb"); //r - read only, b - in binary
// 	if (!S_ISREG(buf.st_mode) || file == NULL)
// 	{
// 		msg = "Error! Can not open the file " + path + ", sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "404";
// 	}
// 	else
// 	{
// 		msg = "the file " + path + " was sucsessfully opened, sd ";
// 		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
// 		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
// 		connection->responseData.statusCode = "200";

// 		fseek(file, 0L, SEEK_END);
// 		int fileLength = ftell(file);

// 		int dot = path.find(".");
// 		std::string ext = path.substr(dot + 1, path.length() - dot);
// 		std::string contType;
// 		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
// 			contType = connection->contentTypesAll[ext];
// 		else
// 			contType = "text/html";

// 		std::string ff = "<!DOCTYPE html>\n<html lang=\"en\">\n\t<head>\n\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
// 		ff += "\t\t<title>The Periodic Table</title>\n\t</head>\n\t<body>\n";
// 		ff += "\t\t<h2>Dmitri Mendeleev\'s periodic table of the elements</h2>\n\t\t<br>\n\t\t<table style = 'border: 1px blue solid;'>\n";
// 		ff += "\t\t</table>\n\t</body>\n</html>";

// 		// contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER;
// 		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(ff.size()) + DDELIMETER + ff + DDELIMETER;
// 		// std::stringstream bufFile;
// 		// bufFile << file.rdbuf();
// 		(void)fileLength;
// 		fclose(file);
// 	}
// }
