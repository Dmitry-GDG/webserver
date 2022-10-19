#include "Client.hpp"

void Client::_welcome()
{
	std::string msg = "";
	msg += YELLOS;
	msg += "Welcome to the ";
	msg += CLIENT_NAME;
	msg += " v.";
	msg += CLIENT_VERSION;
	msg += "by ";
	msg += CLIENT_AUTHORS;
	msg += ". ";
	msg += HELP_CLIENT_MSG;
	msg += "\n";
	_sendMsg(msg);
}

void Client::_chelp()
{
	std::string msgToSend = "CHELPB ";
	msgToSend += _command;
	msgToSend += " ";
	msgToSend += HELP_CLIENT_MSG;
	_sendMsg(msgToSend);
}

void Client::_quit()
{
	exit (0);
}

void Client::_makefilelist()
{
    DIR *dir;
    struct dirent *entry;
	std::ofstream file_list;                 //создаем поток
    if ((dir = opendir(FILES_TO_SEND_DIR)) == NULL) {
        return;
    }
    std::ofstream ofile("files.txt");
	file_list.open("files.txt", std::ios::out | std::ios::in);  // открываем файл для перезаписи
    if ( (entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".DS_Store"))
		{
			file_list << entry->d_name;
			file_list << "\n";
		}
		while ( (entry = readdir(dir)) != NULL) 
		{
			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".DS_Store"))
			{
				file_list << entry->d_name;
				file_list << "\n";
			}
		}
	}
	file_list.close();
    ofile.close();
    closedir(dir);
}

void Client::_printfileslist()
{
	_makefilelist();
	std::string msg = "Files list: ";
	std::string buf;
	std::fstream fileToSend;
	fileToSend.open("files.txt");
    if (!fileToSend.is_open())
        return ;
    while (1)
    {
        std::getline(fileToSend, buf);
		if (buf != "." && buf != ".." && buf != ".DS_Store")
		{
        	msg += buf;
			msg += " ";
		}
        if (fileToSend.eof())
            break;
    }
	msg += "\n";
    fileToSend.close();
	_printMsgToTerminal(0, 0, msg);
}

void Client::_post(std::string msg)
{
	_makefilelist();
	_splitCommandsPost(msg);
	#ifdef DEBUGMODE
		printCommandParsed(this, "DEBUGMODE commands _filesend commandParced");
	#endif
	if (_commandArgs.size() < 1)
		return _printMsgToTerminal(0, 0, "Not enough parameters. EXAMPLE: POST <filename>\n");
	_preparePostRequest();
	_sendMsg(_msgToSend);
}

void Client::_preparePostRequest()
{
	_msgToSend.clear();
	_msgToSend = "POST ";
	_msgToSend += _host;
	_msgToSend += ":";
	_msgToSend += _port;
	_msgToSend += " HTTP/1.1";
	_msgToSend += DELIMETER;
	_msgToSend += "Content-Type: ";
	std::string fileType = _getFileType(_commandArgs[0]);
	if (_isText(fileType) && _commandArgs.size() == 1)
	{
		_msgToSend += _getContentType(fileType);
		_msgToSend += DDELIMETER;
		std::string buf;
		std::string fileName = FILES_TO_SEND_DIR + _commandArgs[0];
		std::fstream fileToSend;
		fileToSend.open(fileName);
		if (!fileToSend.is_open())
		{
			_printMsgToTerminal(0, 0, _commandArgs[0] + " Invalid file name\n");
			_msgToSend = "";
			return;
		}
		// msg += " \n";
		while (1)
		{
			std::getline(fileToSend, buf);
			_msgToSend += buf;
			if (fileToSend.eof())
				break;
			_msgToSend += "\n";
		}
		fileToSend.close();
	//    std::cout << "MSG : " << msg << std::endl;
		_msgToSend += DELIMETER;
		#ifdef DEBUGMODE
			std::cout << VIOLET << " DEBUGMODE commands _preparePostRequest " << NC << "\n_msgToSend: " << _msgToSend << "\n----------------------" << std::endl;
		#endif
		return ;
	}
	else
	{
		for (size_t i = 0; i < _commandArgs.size(); i++)
		{

			// void readFileToStrInBinary(const char * pathChar, std::string & outp);

		}

	}
}

bool Client::_isText(std::string str)
{
	std::string arr[] = {"abc", "acgi", "aip", "asm", "asp", "c", "c++", "cc", "com", "conf", "cpp", "css", "cxx", "def", "el", "etx", "f", "f77", "f90", "flx", "for", "g", "h", "hh", "hlb", "htc", "htm", "html", "htmls", "htt", "htx", "idc", "jav", "java", "js", "list", "log", "lst", "lsx", "m", "mar", "p", "pas", "pl", "py", "rexx", "rt", "rtf", "rtx", "s", "sdml", "sgm", "sgml", "shtml", "ssi", "talk", "tcsh", "text", "tsv", "txt", "uil", "uni", "unis", "uri", "uris", "uue", "vcs", "wml", "wmls", "wsc", "xml", "zsh"};
	std::vector<std::string> texts(std::begin(arr), std::end(arr));
	for (std::vector<std::string>::iterator iter = texts.begin(); iter < texts.end(); iter++)
	{
		if (str == *iter)
			return true;
	}
	return false;
}

std::string Client::_getFileType(std::string str)
{
	size_t pos = str.find(".");
	std::string outp = "";
	if (pos != std::string::npos)
		outp = str.erase(0, pos + 1);
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE commands _getType " << NC << "\ntype: " << outp << "\n----------------------" << std::endl;
	#endif
	return (outp);
}

std::string Client::_getContentType(std::string str)
{
	for (std::map<std::string, std::string>::iterator iter = _contentTypes.begin(); iter != _contentTypes.end(); iter++)
	{
		if (str == (*iter).first)
			return (*iter).second;
	}
	return ("text/plain");
}

// void Client::_filesend(std::string msg)
// {
// 	_makefilelist();
// 	CommandParced commandParced(_splitCommandsFS(msg));
// 	#ifdef DEBUGMODE
// 		printCommandParsed(this, "commands _filesend commandParced");
// 	#endif
// 	if (commandParced.args.size() < 1)
// 		return _printMsgToTerminal(0, 0, "Not enough parameters. EXAMPLE: POST <filename>\n");
// 	std::string buf;
// 	std::string fileName = FILES_TO_SEND_DIR + commandParced.args[1];
// 	std::fstream fileToSend;
//     fileToSend.open(fileName);
//     if (!fileToSend.is_open())
//         return (_printMsgToTerminal(0, 0, commandParced.args[1] + " Invalid file name\n"));
// 	msg += " \n";
//     while (1)
//     {
//         std::getline(fileToSend, buf);
//         msg+= buf;
//         if (fileToSend.eof())
//             break;
// 		msg += "\n";
//     }
//     fileToSend.close();
// //    std::cout << "MSG : " << msg << std::endl;
// 	msg += DELIMETER;
// 	_sendMsg(msg);
// }

// int Client::_checkFileName(std::string sendName)
// {
// 	std::fstream fileList;
// 	std::string buf;
// 	fileList.open("files.txt");
//     if (!fileList.is_open())
//         return 0;
    
// 	// std::cout << "!!!!" << _bufGet <<  std::endl;
// 	while (1)
//     {
//         std::getline(fileList, buf);
// 		// std::cout << "buf: " << buf << std::endl;
// 		if (buf == sendName)
// 		{
// 			fileList.close();
// 			return 1;
// 		}
//         if (fileList.eof())
//             break;
//     }
//     fileList.close();
// 	return 2;
// }

// void Client::_fileget(CommandParced commandParced)
// {
// 	// std::cout << "commandParced.command: " << commandParced.command << std::endl;
// 	// for (size_t i = 0; i < commandParced.args.size(); i++)
// 	// 	std::cout << "commandParced.arg[" << i << "]: " << commandParced.args[i] << std::endl;
// 	_makefilelist();
// 	// std::fstream fileList;
// 	// std::string buf;
// 	// fileList.open("files.txt");
//     // if (!fileList.is_open())
//     //     return (_sendMsg(sd, "371 " + commandParced.args[0] + " FILEGET unknown error. Try later"));
    
// 	// // std::cout << "!!!!" << _bufGet <<  std::endl;
// 	// while (1)
//     // {
//     //     std::getline(fileList, buf);
// 	// 	if (buf == commandParced.args[1])
// 	// 		return (_sendMsg(sd, "371 " + commandParced.args[0] + " FILEGET The file " + commandParced.args[1] + " already exists"));
//     //     if (fileList.eof())
//     //         break;
//     // }
//     // fileList.close();
// 	std::string sendName = commandParced.args[1];
// 	// std::string saveName = commandParced.args[1];
// 	int i = _checkFileName(sendName);
// 	if (i != 2)
// 	{
// 		if (i == 0)
// 			return (_sendMsg("371 " + commandParced.args[1] + " FILEGET unknown error. Try later"));
// 		for (size_t j = 1; ; j++)
// 		{
// 			sendName = commandParced.args[1] + "_" + unsignedToString99(j);
// 			i = _checkFileName(sendName);
// 			if (i == 2)
// 				break ;
// 			if (i == 0)
// 				return (_sendMsg("371 " + commandParced.args[0] + " FILEGET unknown error. Try later"));
// 		}
// 	}
// 	std::string fileName = "./files/" + sendName;
// 	// std::cout << "commandParced.args[1] : " << commandParced.args[1] <<  std::endl;
// 	std::ofstream fileToWrite;
// 	std::ofstream ofile(fileName);
// 	fileToWrite.open(fileName, std::ios::out | std::ios::in);
//     // // if (!fileToWrite.is_open())
//     // //     return (_sendMsg(sd, "371 " + commandParced.args[1] + " :Invalid file name"));

//     // for (size_t i = 2; i < commandParced.args.size(); i++)
//     //     fileToWrite << commandParced.args[i];
// 	std::string tmp = _bufGet;
// 	int j = 0;
// 	std::string::iterator iter = tmp.begin();
// 	while (iter < tmp.end() && (*iter) != '\n')
// 	{
// 		iter++;
// 		j++;
// 	}
// 	tmp.erase(0, j + 1);
// 	fileToWrite << tmp;
//     fileToWrite.close();
// 	// _sendMsg(sd, msgToSend);
// 	// // _printMsgToTerminal(0, sd, msgToSend);
// 	// std::string msgToSend = "FGB ";
// 	// msgToSend += commandParced.args[0];
// 	// msgToSend += " ";
// 	// msgToSend += commandParced.args[1];
// 	// // msgToSend += " \n ";
// 	// msgToSend += "\n";
// 	_fileslist(commandParced);
// 	// _quit();
// 	// std::cout << "I dont want to exit!!!" << std::endl;
// }
