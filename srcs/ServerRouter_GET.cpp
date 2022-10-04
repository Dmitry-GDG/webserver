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
		; // Подумать, что вернуть, если не откроется файл

	connection->responseData.connectionAnswer += connection->responseData.statusCode \
	+ " " + connection->responseStatusCodesAll[connection->responseData.statusCode] \
	+ DELIMETER + "Server: \"" + WEBSERV_NAME + "\"" + DELIMETER \
	+ contentTypeAndLengthAndData;



}

bool ServerRouter::_addFileToAnswer(std::string & contentTypeAndLengthAndData, t_connection * connection)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	// std::string path = server.getConfig().listen + connection->inputdata.address;
	// std::string path = "./" + connection->inputData.address;

	std::string path = "./";

	size_t i;
	if (server.getConfig().root != "")
		path += server.getConfig().root + "/";
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		if (connection->inputData.address == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				path += server.getConfig().locations[i].root + "/";
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

	path += connection->inputData.address;

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
	struct stat buf;
	
	lstat(pathChar, & buf);
	FILE * fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL)
	{
		msg = "Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
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
			msg = "Error! Can not open the file " + path + ", sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "404";
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
		msg = "Error! Can not open the file " + path + ", sd ";
		printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		connection->responseData.statusCode = "404";
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
