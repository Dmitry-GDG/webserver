# include "main.hpp"

// открываем конфиг файл, читаем и записываем построчно в приватный вектор класса
bool openConfigFileAndWriteToVectorConfigFile(std::string config_file_name, std::vector<std::string> & vectorConfigFile)
{
	std::string	stringForRead;

	std::ifstream inpt;
	inpt.open(config_file_name);
	if (!inpt.is_open())
	{
		std::cerr << "Error of read entered file" << std::endl;
		return false;
	}

	while (getline(inpt, stringForRead))
		vectorConfigFile.push_back(stringForRead);
	inpt.close();
	return true;
}

// проверка скобок
void	checkBracketAndCorrectVector(std::vector<std::vector<std::string> >	& oneServerConfigVectorSplit)
{
	int		openBrackets = 0;
	bool	foundBracket = false;
	bool	foundName = false;

	for (std::vector<std::vector<std::string> >::iterator iterVV = oneServerConfigVectorSplit.begin(); iterVV < oneServerConfigVectorSplit.end(); iterVV++) 
	{
		std::vector<std::string>::iterator iterV, tmp;
		for (iterV = (*iterVV).begin(); iterV < (*iterVV).end(); iterV++)
		{
			if ((*iterV) == "{")
			{
				openBrackets++;
				foundBracket = true;
				foundName = true;
			}
			if ((*iterV) == "}")
			{
				if(foundBracket)
					openBrackets--;
				else
				{
					tmp = iterV - 1;
					(*iterVV).erase(iterV);
					iterV = tmp;
					continue;
				}
			}
			if (!foundBracket && foundName)
			{
				tmp = iterV - 1;
				(*iterVV).erase(iterV);
				iterV = tmp;
				continue;
			}
			foundName = true;
			if (foundBracket && !openBrackets)
			{
				int vsize = iterV - (*iterVV).begin();
				(*iterVV).resize(vsize);
				vsize = iterVV - oneServerConfigVectorSplit.begin();
				oneServerConfigVectorSplit.resize(vsize);
				return;
			}
		}
	}
	oneServerConfigVectorSplit.clear();
}

// вектор парсится на структуру
void	parseVectorOnStruct(std::vector<std::vector<std::string> >	& oneServerConfigVectorSplit, std::vector<t_config> & configs)
{
	t_config					oneServerConfig;
	std::vector<std::string>	splitStr;
	t_location					location;
	std::vector<t_location>		locations;
	std::vector<std::string>	tmp;
	std::vector<std::string>::iterator	iter;
	
	configClear(oneServerConfig);

	// If "server_name" was placed before open brace
	if (*((oneServerConfigVectorSplit[0]).begin()) != "{")
		oneServerConfig.serverName = *((oneServerConfigVectorSplit[0]).begin());

	// Find all locations and delete them from vector oneServerConfigVectorSplit
	for (size_t i = 0; i < oneServerConfigVectorSplit.size(); i++)
	{
		if (oneServerConfigVectorSplit[i][0] == "location")
		{
			locationClear(location);
			location.path = oneServerConfigVectorSplit[i][1];
			oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
			for (;i < oneServerConfigVectorSplit.size(); i++)
			{
				// std::cout << "oneServerConfigVectorSplit[i][0]" << oneServerConfigVectorSplit[i][0] <<std::endl;
				if (oneServerConfigVectorSplit[i][0] == "root")
				{
					location.root = oneServerConfigVectorSplit[i][1];
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "index")
				{
					location.index = oneServerConfigVectorSplit[i][1];
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "autoindex")
				{
					location.autoindex = oneServerConfigVectorSplit[i][1];
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "upload")
				{
					location.upload = oneServerConfigVectorSplit[i][1];
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "limit_size")
				{
					location.limit_size = oneServerConfigVectorSplit[i][1];
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "methods" || oneServerConfigVectorSplit[i][0] == "method" || oneServerConfigVectorSplit[i][0] == "allow_methods" || oneServerConfigVectorSplit[i][0] == "allow_method")
				{
					for (size_t j = 1; j < oneServerConfigVectorSplit[i].size(); j++)
						location.methods.push_back(oneServerConfigVectorSplit[i][j]);
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "redirs" || oneServerConfigVectorSplit[i][0] == "redir" || oneServerConfigVectorSplit[i][0] == "redirection")
				{
					location.redirs = oneServerConfigVectorSplit[i][1];
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "cgi")
				{
					location.cgi.insert(std::pair<std::string, std::string>(oneServerConfigVectorSplit[i][1], oneServerConfigVectorSplit[i][2]) );
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "error" || oneServerConfigVectorSplit[i][0] == "error_page" || oneServerConfigVectorSplit[i][0] == "errors" || oneServerConfigVectorSplit[i][0] == "error_pages")
				{
					location.error_pages.insert(std::pair<std::string, std::string>(oneServerConfigVectorSplit[i][1], oneServerConfigVectorSplit[i][2]) );
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				}
				else if (oneServerConfigVectorSplit[i][0] == "}")
				{
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
					locations.push_back(location);
					i--;
					break ;
				}
				else
					oneServerConfigVectorSplit.erase(oneServerConfigVectorSplit.begin() + i);
				i--;
			}
			oneServerConfig.locations = locations;
		}
	}
	// # ifdef DEBUGMODE
	// 	printVVector(oneServerConfigVectorSplit, "DEBUG parseVectorOnStruct after Delete All locations");
	// # endif

	// After removing all locations fill server structure
	for (size_t j = 0; j < oneServerConfigVectorSplit.size(); j++)
	{
		if (oneServerConfigVectorSplit[j].size() > 1)
		{
			for (size_t i = 0; i < oneServerConfigVectorSplit[j].size(); i++)
			{
				if (oneServerConfigVectorSplit[j][0] == "listen")
				{
					oneServerConfig.listen = oneServerConfigVectorSplit[j][1];
					tmp.clear();
					splitString(oneServerConfigVectorSplit[j][1], ':', tmp);
					oneServerConfig.ip = tmp[0];
					if (tmp.size() > 1)
						oneServerConfig.port = tmp[1];
				}
				if (oneServerConfigVectorSplit[j][0] == "ip")
					oneServerConfig.ip = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "port")
					oneServerConfig.port = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "server_name" && oneServerConfig.serverName.size() == 0)
					oneServerConfig.serverName = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "limit_size")
					oneServerConfig.limitSize = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "root")
					oneServerConfig.root = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "index")
					oneServerConfig.index = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "autoindex")
					oneServerConfig.autoindex = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "cgi")
					oneServerConfig.cgi.insert(std::pair<std::string, std::string>(oneServerConfigVectorSplit[j][1], oneServerConfigVectorSplit[j][2]) );
				if (oneServerConfigVectorSplit[j][0] == "redir" || oneServerConfigVectorSplit[j][0] == "redirs" || oneServerConfigVectorSplit[j][0] == "redirection")
					oneServerConfig.redirs = oneServerConfigVectorSplit[j][1];
				if (oneServerConfigVectorSplit[j][0] == "error_page" || oneServerConfigVectorSplit[j][0] == "error_pages" || oneServerConfigVectorSplit[j][0] == "error" || oneServerConfigVectorSplit[j][0] == "errors")
					oneServerConfig.error_pages.insert(std::pair<std::string, std::string>(oneServerConfigVectorSplit[j][1], oneServerConfigVectorSplit[j][2]) );
				if (oneServerConfigVectorSplit[j][0] == "methods" || oneServerConfigVectorSplit[j][0] == "method" || oneServerConfigVectorSplit[j][0] == "allow_methods" || oneServerConfigVectorSplit[j][0] == "allow_method")
				{
					for (size_t k = 1; k < oneServerConfigVectorSplit[j].size(); k++)
					{
						if (std::find(oneServerConfig.methods.begin(), oneServerConfig.methods.end(), oneServerConfigVectorSplit[j][k])  == oneServerConfig.methods.end())
							oneServerConfig.methods.push_back(oneServerConfigVectorSplit[j][k]);
					}
				}
			}
		}
	}
	// # ifdef DEBUGMODE
	// 	printServerConfig(oneServerConfig, "DEBUG parseVectorOnStruct");
	// # endif
	
	if (oneServerConfig.ip.size() > 0 && oneServerConfig.port.size() > 0)
		configs.push_back(oneServerConfig);
}

void delCommentsAndWhiteSpacesAndEmptyLines0(std::vector<std::string> & oneServerConfigVector)
{
	// # ifdef DEBUGMODE
	// 	printVector(oneServerConfigVector, "DEBUG delCommentsAndWhiteSpaces 0");
	// # endif

	// _delete Comments
	for (std::vector<std::string>::iterator iterV = oneServerConfigVector.begin(); iterV < oneServerConfigVector.end(); iterV++)
	{
		for (std::string::iterator iterS = (*iterV).begin(); iterS < (*iterV).end(); iterS++)
		{
			if ((*iterS) == '#')
				(*iterV).resize(iterS - (*iterV).begin());
		}
	}
	// # ifdef DEBUGMODE
	// 	printVector(oneServerConfigVector, "DEBUG delCommentsAndWhiteSpaces after delete Comments");
	// # endif

	// _delete White Spaces
	for (std::vector<std::string>::iterator iterV = oneServerConfigVector.begin(); iterV < oneServerConfigVector.end(); iterV++)
	{
		for (std::string::iterator iterS = (*iterV).begin(); iterS < (*iterV).end(); iterS++)
		{
			if ((*iterS) == '\t')
				*iterS = ' ';
		}
	}
	for (std::vector<std::string>::iterator iterV = oneServerConfigVector.begin(); iterV < oneServerConfigVector.end(); iterV++)
	{
		std::string::iterator it = std::unique((*iterV).begin(), (*iterV).end(), isBothSpace);
    	(*iterV).erase(it, (*iterV).end());
		// _delete Spaces at the begining of lines
		if ((*iterV)[0] == ' ')
			(*iterV).erase((*iterV).begin());
	}
	// # ifdef DEBUGMODE
	// 	printVector(oneServerConfigVector, "DEBUG delCommentsAndWhiteSpaces after delete Comments and WhiteSpaces");
	// # endif

	// _delete empty lines
	for (std::vector<std::string>::iterator iterV = oneServerConfigVector.begin(); iterV < oneServerConfigVector.end(); iterV++)
	{
		if ((*iterV).size() < 2 && (*iterV)[0] != '}' && (*iterV)[0] != '{')
		{
			oneServerConfigVector.erase(iterV);
			iterV--;
		}
	}
	// # ifdef DEBUGMODE
	// 	printVector(oneServerConfigVector, "DEBUG delCommentsAndWhiteSpaces after delete Comments and WhiteSpaces and EmptyLines");
	// # endif

	// _delete semicolons
	for (std::vector<std::string>::iterator iterV = oneServerConfigVector.begin(); iterV < oneServerConfigVector.end(); iterV++)
	{
		while ((*iterV).size() > 1 && ((*iterV)[(*iterV).size() - 1] == ' ' || (*iterV)[(*iterV).size() - 1] == ';'))
			(*iterV).resize((*iterV).size() - 1);
	}
	// # ifdef DEBUGMODE
	// 	printVector(oneServerConfigVector, "DEBUG delCommentsAndWhiteSpaces after delete Comments and WhiteSpaces and EmptyLines and Semicolons");
	// # endif
}

// работу начинает парсер
void parserVectorConfigFile(std::vector<std::string> & vectorConfigFile, std::vector<t_config> & configs)
{
	std::string					tmpStr, strConfigFile, substr = "server";
	// int 						start, pos;
	std::string::iterator		strBegin;
	std::vector<std::string>	oneServerConfigVector;
	std::vector<std::string>	splitStr;
	std::vector<std::vector<std::string> >	oneServerConfigVectorSplit;
	std::vector<std::vector<std::string> >	allServersConfigVectorSplit;

	// # ifdef DEBUGMODE
	// 	printVector(vectorConfigFile, "DEBUG parserVectorConfigFile 0");
	// # endif

	delCommentsAndWhiteSpacesAndEmptyLines0(vectorConfigFile);
	// # ifdef DEBUGMODE
	// 	printVector(vectorConfigFile, "DEBUG parserVectorConfigFile after delCommentsAndWhiteSpacesAndEmptyLines");
	// # endif

	// Split all config strings and make vector from them
	allServersConfigVectorSplit.clear();
	for (std::vector<std::string>::iterator iter = vectorConfigFile.begin(); iter < vectorConfigFile.end(); iter++)
	{
		splitStr.clear();
		std::string strToSplit = *iter;
		splitString(strToSplit, ' ', splitStr);
		// # ifdef DEBUGMODE
		// 	printVector(splitStr, "DEBUG parserVectorConfigFile splitStr");
		// # endif
		allServersConfigVectorSplit.push_back(splitStr);
	}
	// # ifdef DEBUGMODE
	// 	printVVector(allServersConfigVectorSplit, "DEBUG parserVectorConfigFile after split all config strings");
	// # endif

	std::vector<std::vector<std::string> >::iterator iterA = allServersConfigVectorSplit.begin();
	for (; iterA < allServersConfigVectorSplit.end(); iterA++)
	{
		while (iterA < allServersConfigVectorSplit.end() && *((*iterA).begin()) != substr)
			iterA++;
		oneServerConfigVectorSplit.clear();
		if ((*iterA).size() > 1)
		{
			(*iterA).erase((*iterA).begin());
			oneServerConfigVectorSplit.push_back(*iterA);
		}
		while (iterA + 1 < allServersConfigVectorSplit.end() && *((*(iterA + 1)).begin()) != substr)
		{
			iterA++;
			oneServerConfigVectorSplit.push_back(*iterA);
		}
		// # ifdef DEBUGMODE
		// 	printVVector(oneServerConfigVectorSplit, "DEBUG parserVectorConfigFile before checkBracketAndCorrectVector");
		// # endif
		checkBracketAndCorrectVector(oneServerConfigVectorSplit);
		// # ifdef DEBUGMODE
		// 	printVVector(oneServerConfigVectorSplit, "DEBUG parserVectorConfigFile after checkBracketAndCorrectVector");
		// # endif
		parseVectorOnStruct(oneServerConfigVectorSplit, configs);
	}
}
