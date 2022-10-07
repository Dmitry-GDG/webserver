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
	#ifdef DEBUGMODE
		std::cout << "**** DEBUGMODE SR_POST _preparePostAnswer ****\npath: " << path << "\n--------\n";
	#endif


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
