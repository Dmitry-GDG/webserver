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
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_POST _preparePostAnswer body \nbody:\n" << NC << connection->inputStrBody << "\n----------------------\n";
	#endif

	_parseInputBodyStr(connection);
	_choosePostContentType(connection);
	// if 


	// std::string contentTypeAndLength = "";
	// if (!_acceptFile(contentTypeAndLength, connection, path))
	// 	; // Подумать, что вернуть, если не примется файл

	connection->responseData.connectionAnswer += connection->responseData.statusCode \
	+ " " + connection->responseStatusCodesAll[connection->responseData.statusCode] \
	+ DELIMETER + "Server: \"" + WEBSERV_NAME + "\"" + DELIMETER + "Connection: Close";

	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE SR_POST _postUrlencoded connection->responseData.connectionAnswer: \n" << NC << connection->responseData.connectionAnswer << "\n----------------------\n";
	#endif
}

bool ServerRouter::_acceptFile(t_connection * connection, std::string & contentTypeAndLength, std::string const & path)
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
	#ifdef DEBUGMODE
		std::cout << BLUE << " DEBUGMODE SR_POST _choosePostContentType connection->inputData.postContentType 0 \npostContentType: " << NC << connection->inputData.postContentType << "\n----------------------\n";
	#endif
	for (std::vector<std::pair<std::string, std::string> >::iterator iter = connection->inputData.headerFieldsVec.begin(); iter < connection->inputData.headerFieldsVec.end(); iter++)
	{
		if ((*iter).first.find("Content-Type") != std::string::npos)
		{
			std::string contentType = (*iter).second;
			if ((*iter).second.find("application/x-www-form-urlencoded") != std::string::npos)
			{
				connection->inputData.postContentType = URLENCODED;
				_postUrlencoded(connection, contentType);
			}
			else if ((*iter).second.find("multipart/form-data") != std::string::npos)
			{
				connection->inputData.postContentType = FORM_DATA;
				_postFormData(connection, contentType);
			}
			else if ((*iter).second.find("multipart/mixed") != std::string::npos)
			{
				connection->inputData.postContentType = MIXED;
				_postMixed(connection, contentType);
			}
			else if ((*iter).second.find("application/json") != std::string::npos)
			{
				// connection->inputData.postContentType = MIXED;
				// _postMixed(connection, contentType);
			}
		}
	}
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR_POST _choosePostContentType connection->inputData.postContentType \npostContentType: " << NC << connection->inputData.postContentType << "\n----------------------\n";
	// #endif
	std::cout << RED << "TEST" << NC << std::endl;
}

void ServerRouter::_postUrlencoded(t_connection * connection, std::string contentType)
{
	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE SR_POST _postUrlencoded connection->responseData.connectionAnswer: \n" << NC << connection->responseData.connectionAnswer << "\n----------------------\n";
	#endif
	// connection->responseData.connectionAnswer +=
	(void) contentType;
	(void) connection;
}

void ServerRouter::_postFormData(t_connection * connection, std::string contentType)
{
	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE  SR_POST _postFormData connection->responseData.connectionAnswer: \n" << NC << connection->responseData.connectionAnswer << "\n----------------------\n";
	#endif
	std::string boundary;
	_findBoundary(contentType, boundary);
	connection->inputData.boundary.push_back(boundary);
	// #ifdef DEBUGMODE
	// 	printBoundary((*connection), " DEBUGMODE SR_POST _postFormData ");
	// #endif
	if (connection->inputData.boundary.size() < 1)
	{
		connection->responseData.statusCode = "400";
		return ;
	}
	connection->responseData.statusCode = "100";
	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE SR_POST _postFormData connection->inputStrBody:\n" << NC << connection->inputStrBody << "\n----------------------\n";
	#endif
	std::vector<std::string> bodyVec;
	bodyVec.clear();
	splitStringStr(connection->inputStrBody, "--" + connection->inputData.boundary[0], bodyVec);
	for (std::vector<std::string>::iterator iter = bodyVec.begin(); iter < bodyVec.end(); iter++)
	{
		if ((*iter) != "" && (*iter) != DELIMETER && (*iter) != DDELIMETER)
		{
			std::string tmp = "--";
			if ((*iter == "--") || (*iter == (tmp + DELIMETER)) || (*iter == (tmp + DDELIMETER)))
				break;
			_postGetFileName(connection, *iter);
			#ifdef DEBUGMODE
				std::cout << RED <<  " DEBUGMODE SR_POST _postFormData *iter: \n" << NC << *iter << "\n----------------------\n";
			#endif
			#ifdef DEBUGMODE
				if (connection->inputData.postFileName.size())
					std::cout << RED <<  " DEBUGMODE SR_POST _postFormData connection->inputData.postFileName: \n" << NC << connection->inputData.postFileName << "\n----------------------\n";
			#endif
			#ifdef DEBUGMODE
				if (connection->inputData.postFileData.size())
					std::cout << RED <<  " DEBUGMODE SR_POST _postFormData connection->inputData.postFileData: \n" << NC << connection->inputData.postFileData << "\n----------------------\n";
			#endif
			_postGetFilePathToSave(connection);
			// std::cout << RED << "TEST22" << NC << std::endl;
			_postSaveFile(connection);
		}
		// else
		// 	continue ;
	}
	// connection->responseData.connectionAnswer +=

}

void ServerRouter::_postMixed(t_connection * connection, std::string contentType)
{
	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE SR_POST _postMixe connection->responseData.connectionAnswer: \n" << NC << connection->responseData.connectionAnswer << "\n----------------------\n";
	#endif
	// connection->responseData.connectionAnswer +=
	(void) contentType;
	(void) connection;
}

void ServerRouter::_findBoundary(std::string contentType, std::string &boundary)
{
	std::vector<std::string> contentTypeVec;
	splitStringStr(contentType, "; ", contentTypeVec);
	for (std::vector<std::string>::iterator iter = contentTypeVec.begin(); iter < contentTypeVec.end(); iter++)
	{
		if ((*iter).find("boundary") != std::string::npos)
		{
			std::vector<std::string> boundaryVec;
			splitStringStr(*iter, "=", boundaryVec);
			boundary = boundaryVec[1];
		}
	}
}

void ServerRouter::_postGetFileName(t_connection * connection, std::string str)
{
	// #ifdef DEBUGMODE
	// 	std::cout << RED <<  " DEBUGMODE SR_POST _getFile str: \n" << NC << str << "\n----------------------\n";
	// #endif
	connection->inputData.postFileName.clear();;
	std::string endStr = "--";
	endStr += DELIMETER;
	if (str == endStr || str == "--")
	{
		connection->responseData.statusCode = "200";
		return ;
	}
	std::vector<std::string> dataVec, dataVec0, dataVec00, dataVec000;
	dataVec.clear();
	splitStringStr(str, DDELIMETER, dataVec);
	// #ifdef DEBUGMODE
	// 	printVector(dataVec, "SR_POST _getFile dataVec");
	// #endif
	// #ifdef DEBUGMODE
	// 	if (dataVec[0].size())
	// 		std::cout << "dataVec[0]: " << dataVec[0] << std::endl;
	// 	if (dataVec[1].size())
	// 		std::cout << "dataVec[1]: " << dataVec[1] << std::endl;
	// #endif
	connection->inputData.postFileData = dataVec[1];
	if (connection->inputData.postFileData == "\n" || connection->inputData.postFileData == DELIMETER || connection->inputData.postFileData == DDELIMETER)
		connection->inputData.postFileData.clear();
	if (dataVec[1] == "\n" || dataVec[1] == "" || dataVec[1] == DELIMETER || dataVec[1] == DDELIMETER)
		return;
	if (dataVec.size())
	{
		dataVec0.clear();
		splitStringStr(dataVec[0], DELIMETER, dataVec0);
		// #ifdef DEBUGMODE
		// 	printVector(dataVec0, "SR_POST _getFile dataVec0");
		// #endif
		// #ifdef DEBUGMODE
		// 	if (dataVec0[0].size())
		// 		std::cout << "dataVec0[0]: " << dataVec0[0] << std::endl;
		// 	if (dataVec0[1].size())
		// 		std::cout << "dataVec0[1]: " << dataVec0[1] << std::endl;
		// #endif
		if (dataVec0.size())
		{
			std::map<std::string, std::string> dataMap;
			dataMap.clear();
			for (std::vector<std::string>::iterator iter = dataVec0.begin(); iter < dataVec0.end(); iter++)
			{
				dataVec00.clear();
				splitStringStr((*iter), ": ", dataVec00);
				// #ifdef DEBUGMODE
				// 	printVector(dataVec00, "SR_POST _getFile dataVec00");
				// #endif
				// #ifdef DEBUGMODE
				// 	if (dataVec00[0].size())
				// 		std::cout << "dataVec00[0]: " << dataVec00[0] << std::endl;
				// 	if (dataVec00[1].size())
				// 		std::cout << "dataVec00[1]: " << dataVec00[1] << std::endl;
				// #endif
				if (dataVec00.size() && dataVec00[0].size() && dataVec00[1].size())
					dataMap.insert(std::pair<std::string, std::string>(dataVec00[0], dataVec00[1]));
			}
			if (dataMap.size())
			{
				// #ifdef DEBUGMODE
				// 	printMapStrStr(dataMap, "SR_POST _getFile dataMap");
				// #endif
				dataVec000.clear();
				for (std::map<std::string, std::string>::iterator iter = dataMap.begin(); iter != dataMap.end(); iter++)
				{
					if ((*iter).first.find("Content-Disposition") != std::string::npos)
					{
						splitStringStr((*iter).second, "; ", dataVec000);
						// #ifdef DEBUGMODE
						// 	printVector(dataVec000, "SR_POST _getFile dataVec000");
						// #endif
						// #ifdef DEBUGMODE
						// 	for (size_t i = 0; i < dataVec000.size(); i++)
						// 	// {
						// 	// 	if (dataVec000[i].size())
						// 			std::cout << "dataVec000[" << i << "]: " << dataVec000[i] << std::endl;
						// 	// }
						// #endif
						break;
					}
				}
				if (dataVec000.size())
					connection->inputData.postFileName = getFileName(dataVec000);
			}
		}
	}
	// #ifdef DEBUGMODE
	// 	std::cout << RED <<  " DEBUGMODE SR_POST _getFile connection->inputData.postFileName: \n" << NC << connection->inputData.postFileName << "\n----------------------\n";
	// #endif
}

void ServerRouter::_postGetFilePathToSave(t_connection * connection)
{
	Server server = _getServer(connection->srvNbr);
	connection->inputData.postFilePathToSave = "./";
	if (server.getConfig().root != "")
		connection->inputData.postFilePathToSave += server.getConfig().root + "/";
	connection->inputData.postFilePathToSave += server.getConfig().upload + "/";
	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE SR_POST _postGetFilePathToSave connection->inputData.postFilePathToSave: \n" << NC << connection->inputData.postFilePathToSave << "\n----------------------\n";
	#endif
}


int ServerRouter::_postCheckIsFileExist(t_connection * connection)
{
	DIR *dir;
    struct dirent *entry;
    if ((dir = opendir((connection->inputData.postFilePathToSave).c_str())) == NULL)
		mkdir((connection->inputData.postFilePathToSave).c_str(), 0777);		
	while ( (entry = readdir(dir)) != NULL) 
	{
		if (!strcmp(entry->d_name, (connection->inputData.postFileName).c_str()))
		{
			closedir(dir);
			return 1;
		}
	}
    closedir(dir);
	return 2;
}

void ServerRouter::_postSaveFile(t_connection * connection)
{
	std::string buf;

	std::vector<std::string> saveName;
	splitStringColon(connection->inputData.postFileName, '.', saveName);
	int i = _postCheckIsFileExist(connection); // ? maybe just save with another filenamename ?
	if (i != 2)
	{
		// if (i == 0)
			// return (_sendMsg("371 " + commandParced.args[1] + " FILEGET unknown error. Try later"));// придумать ошибку и вернуть её
		for (size_t j = 1; ; j++)
		{
			connection->inputData.postFileName = saveName[0] + "_" + unsignedToString99(j);
			if (saveName[1].size())
				connection->inputData.postFileName += "." + saveName[1];
			// #ifdef DEBUGMODE
			// 	std::cout << RED <<  " DEBUGMODE SR_POST _postSaveFile i = \n" << NC << i << "\n----------------------\n";
			// #endif
			i = _postCheckIsFileExist(connection);
			if (i == 2)
				break ;
			// if (i == 0)
				// return (_sendMsg("371 " + commandParced.args[0] + " FILEGET unknown error. Try later"));
		}
	}

	std::string fileName = correctSlashInAddress(connection->inputData.postFilePathToSave) + "/" + correctSlashInAddress(connection->inputData.postFileName);
	#ifdef DEBUGMODE
		std::cout << RED <<  " DEBUGMODE SR_POST _postSaveFile fileName: \n" << NC << fileName << "\n----------------------\n";
	#endif

	FILE *file;
	if (connection->requestProcessingStep == READING_DONE)
	{
		file = fopen(fileName.c_str(), "w");
		connection->requestProcessingStep = WRITING;
	}
	else
		file = fopen(fileName.c_str(), "a");

	// #ifdef DEBUGMODE
	// 	printCharInt(connection->inputData.postFileData, "DEBUGMODE SR_POST _postSaveFile connection->inputData.postFileData");
	// #endif
	// std::cout << RED << "TEST00" << NC << std::endl;
	std::vector<std::string> dataVecData;
	// std::cout << RED << "TEST0" << NC << std::endl;
	splitStringStr(connection->inputData.postFileData, DELIMETER, dataVecData);
	// std::cout << RED << "TEST" << NC << std::endl;
	std::string dataToWrite = dataVecData[0];

	fwrite(dataToWrite.c_str(), sizeof(char), dataToWrite.size(), file);
	// fwrite((connection->inputData.postFileData).c_str(), sizeof(char), (connection->inputData.postFileData).size(), file);
	// #ifdef DEBUGMODE
	// 	std::cout << RED <<  " DEBUGMODE SR_POST _makefilelist \n" << NC << "ERROR11" << "\n----------------------\n";
	// #endif
	fclose(file);
}
