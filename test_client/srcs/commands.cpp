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
		printCommandParsed(this, "commands _filesend commandParced");
	#endif
	if (_commandArgs.size() < 1)
		return _printMsgToTerminal(0, 0, "Not enough parameters. EXAMPLE: POST <filename>\n");
	_preparePostRequest();
	std::string buf;
	std::string fileName = FILES_TO_SEND_DIR + _commandArgs[1];
	std::fstream fileToSend;
    fileToSend.open(fileName);
    if (!fileToSend.is_open())
        return (_printMsgToTerminal(0, 0, _commandArgs[1] + " Invalid file name\n"));
	msg += " \n";
    while (1)
    {
        std::getline(fileToSend, buf);
        msg+= buf;
        if (fileToSend.eof())
            break;
		msg += "\n";
    }
    fileToSend.close();
//    std::cout << "MSG : " << msg << std::endl;
	msg += DELIMETER;
	_sendMsg(msg);
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
	for (size_t i = 0; i < _commandArgs.size(); i++)
	{

	}
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
