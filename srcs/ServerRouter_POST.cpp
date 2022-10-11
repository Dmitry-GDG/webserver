#include "ServerRouter.hpp"

void ServerRouter::_preparePostAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = POST;
	Server server = _getServer(connection->srvNbr);
	std::string path = "./";
	if (server.getConfig().root != "")
		path += server.getConfig().root + "/";
	path += _extractLocalAddress(connection->inputData.address);
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_POST _preparePostAnswer path \npath: " << NC << path << "\n----------------------\n";
	// #endif
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_POST _preparePostAnswer body \nbody:\n" << NC << connection->inputStrBody << "\n----------------------\n";
	// #endif

	_parseInputBodyStr(connection);
	_choosePostContentType(connection);
	// if 


	// std::string contentTypeAndLength = "";
	// if (!_acceptFile(contentTypeAndLength, connection, path))
	// 	; // Подумать, что вернуть, если не примется файл

	// connection->responseData.connectionAnswer += connection->responseData.statusCode \
	// + " " + connection->responseStatusCodesAll[connection->responseData.statusCode] \
	// + DELIMETER + "Connection: Close" + DDELIMETER;

}

bool ServerRouter::_acceptFile(std::string & contentTypeAndLength, t_connection * connection, std::string const & path)
{
	(void) path;
	(void) connection;
	(void) contentTypeAndLength;
	// Проверить, возможно, такой файл уже существует -> еррор
	// std::vector<std::string> splitInputDataAddr;
	// splitString(connection->inputData.address, ':', splitInputDataAddr);
	// std::string pathInsideServer = splitInputDataAddr[1];

	// std::string saveName = 


	// std::ofstream outFile;
	// struct stat file;
	// bool isExists = true;

	// // Заполнение информации о файле и проверка на его существование
	// if (stat(_parsedURI.upload.c_str(), &file))
	// {
	// 	if (errno == ENOENT)
	// 	{
	// 	isExists = false;
	// 	}
	// 	else
	// 	{
	// 	throw std::logic_error("500");
	// 	}
	// }

	// if (isExists)
	// {
	// 	if (S_ISDIR(file.st_mode)) // Это папка
	// 	{
	// 	// Создаем новый файл
	// 	outFile.open(_parsedURI.upload);
	// 	outFile << _inputBody;
	// 	outFile.close();
	// 	_resBodyType = "";
	// 	}
	// 	else // Это файл
	// 	{
	// 	throw std::logic_error("400");
	// 	}
	// }
	// else
	// {
	// 	// Создаем новый файл
	// 	outFile.open(_parsedURI.upload);
	// 	if (!outFile.is_open())
	// 	throw std::logic_error("400");
	// 	outFile << _inputBody;
	// 	outFile.close();
	// 	_resBodyType = "";
	// }
	return true;
}

void ServerRouter::_parseInputBodyStr(t_connection * connection)
{
	splitStringStr(connection->inputStrBody, DELIMETER, connection->inputData.inputStrBodyVec);
	// #ifdef DEBUGMODE
	// 	printVectorEnter(connection->inputData.inputStrBodyVec, "DEBUGMODE SR_POST _parseInputBodyStr connection->inputData.inputStrBodyVec:");
	// #endif
	connection->inputData.bodyFieldsVec.clear();
	for (std::vector<std::string>::iterator iter = connection->inputData.inputStrBodyVec.begin(); iter < connection->inputData.inputStrBodyVec.end(); iter++)
	{
		std::vector<std::string> strings;
		splitStringColon((*iter), ':', strings);
		connection->inputData.bodyFieldsVec.push_back(std::make_pair(strings[0], strings[1]));
	}
	// #ifdef DEBUGMODE
	// 	printVectorPair(connection->inputData.bodyFieldsVec, "DEBUGMODE SR_POST _parseInputBodyStr connection->inputData.bodyFieldsVec:");
	// #endif
}

void ServerRouter::_choosePostContentType(t_connection * connection)
{
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_POST _choosePostContentType connection->inputData.postContentType 0 \npostContentType: " << NC << connection->inputData.postContentType << "\n----------------------\n";
	// #endif
	for (std::vector<std::pair<std::string, std::string> >::iterator iter = connection->inputData.headerFieldsVec.begin(); iter < connection->inputData.headerFieldsVec.end(); iter++)
	{
		if ((*iter).first.find("Content-Type") != std::string::npos)
		{
			if ((*iter).second.find("application/x-www-form-urlencoded") != std::string::npos)
				connection->inputData.postContentType = URLENCODED;
			else if ((*iter).second.find("multipart/form-data") != std::string::npos)
				connection->inputData.postContentType = FORM_DATA;
			else if ((*iter).second.find("multipart/mixed") != std::string::npos)
				connection->inputData.postContentType = MIXED;
		}
	}
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_POST _choosePostContentType connection->inputData.postContentType \npostContentType: " << NC << connection->inputData.postContentType << "\n----------------------\n";
	// #endif
}
