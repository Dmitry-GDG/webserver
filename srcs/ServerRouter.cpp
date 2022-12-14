#include "ServerRouter.hpp"

ServerRouter::ServerRouter(std::vector<t_config> configs, std::string configFile)
{
	_configs = configs;
	_configFile = configFile;
	_responseStatusCodesInit();
	_contentTypesInit();
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";

	#ifdef DEBUGMODE
		printAllServersConfig(_configs, "DEBUGMODE SR ServerRouter AllServersConfig:");
	#endif
	for (std::vector<t_config>::iterator iter = _configs.begin(); iter < _configs.end(); iter++)
		_servers.push_back(Server(*iter));
	_pollfdsQty = _servers.size();
	_pollfdsInit();
	std::string arr[] = {"GET", "POST", "DELETE"};
	std::vector<std::string> metods(std::begin(arr), std::end(arr));
	_allowedMethods.clear();
	_allowedMethods = metods;
	// #ifdef DEBUGMODE
	// 	printAllServersVector(_servers, "DEBUGMODE SR ServerRouter AllServersVector:");
	// #endif
}

ServerRouter::~ServerRouter() {}

bool ServerRouter::_launch(Server & server, int indx)
{
	int on = 1;
	struct sockaddr_in addr;
	int sd;
	struct pollfd pfd;
	std::string msg;

	// Create socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		msg = "Can not start a server: can not create socket.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// Set socket options
	// SOL_SOCKET - присвоение параметра на уровне библиотеки сокетов
	// SO_REUSEADDR - разрешает повторное использование локальных адресов
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
	{
		close (sd);
		msg = "Can not start a server: can not set socket options.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// преобразуем адрес IPv4 или IPv6 из символьного представления в двоичную форму с сетевым порядком следования байтов
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atou16(server.getConfig().port.c_str()));
	addr.sin_len = sizeof(addr);
	if (inet_pton(addr.sin_family, server.getConfig().ip.c_str(), &(addr.sin_addr)) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not convert listen address.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// Функция bind() задает порт или имя файла для привязки сокета
	if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not set the port to bind the socket to.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	// man listen
	// The backlog parameter defines the maximum length for the queue of pending connections. 
	// If a connection request arrives with the queue full, the client may receive an error with an indication of ECONNREFUSED.  
	// Alternatively, if the underlying protocol supports retransmission, the request may be ignored so that retries may succeed.
	if (listen(sd, SOMAXCONN) < 0)
	{
		close(sd);
		msg = "Can not start a server: can not listen queue.";
		printMsgErr(-1, -1, msg, "");
		printMsgToLogFile(-1, -1, msg, "");
		return false;
	}

	server.setSd(sd);
	server.setSrvrNbr(indx);
	pfd.fd = sd;
	pfd.events = POLLIN;
	_pollfds[indx] = pfd;

	// char _serverIp[sizeof(struct sockaddr_in)];
	inet_ntop(AF_INET, &(addr.sin_addr), _serverIp, sizeof(addr));
	server.setPort(ntohs(addr.sin_port));

	std::cout << NC << timestamp() << YELLOS << "On socket descriptor " << NC << sd \
	<< YELLOS << " the " << NC << server.getConfig().serverName \
	<< YELLOS << " server[" << server.getServNbr() << "] started successfully and is listening on " << NC \
	<< _serverIp << ":" << server.getPort() << std::endl;

	msg = "On sd " + std::to_string(sd) + " the '" + server.getConfig().serverName + "' server[" + std::to_string(server.getServNbr()) + "] started successfully and is listening on " + _serverIp + ":" + std::to_string(ntohs(addr.sin_port));
	printMsgToLogFile(-1, -1, msg, "");

	return true;
}

void ServerRouter::start()
{
	std::string msg;

	std::cout << NC << timestamp() << YELLOS << NAME << ": Welcome to the " << WEBSERV_NAME << " v."<< WEBSERV_VERSION << " by " \
	<< WEBSERV_AUTHORS << "\nIt was tested on MAC OS.\n" \
	<< "It was launched at " << NC << _hostname << YELLOS << ", using settings " << NC "'" << _configFile << "'" << YELLOS << "\nQuit the server with CONTROL-C.\n" << NC;
	
	msg = "Webserver ";
	msg += WEBSERV_NAME;
	msg += " started at " + _hostname;
	printMsgToLogFile(-1, -1, msg, "");

	int indx = 0;
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if (!_launch(*iter, indx))
			exitErr ("Check server config file and try again.");
		indx++;
	}

	// #ifdef DEBUGMODE
	// printPollfds(_pollfds, "DEBUGMODE SR start _pollfds:", _pollfdsQty);
	// #endif

	while (42)
	{
		if (!_mainLoop())
		{
			break;
		}
		_checkTimeout(); //disconnect by timeout
	}

	_closeSockets();
}

bool ServerRouter::_mainLoop()
{
	socklen_t socklen = sizeof(struct sockaddr_in);
	struct sockaddr_in addrNew;
	std::string msg, msg1;
	int err;

	int ret = poll(_pollfds, _pollfdsQty, -1);
	if (ret < 0 || ret == 0)
	{
		_closeSockets();
		if (ret < 0)
			exitErr ("Poll error.");
		exitErr ("Poll error timeout.");
	}
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].revents == 0)
		{
			continue;
		}
		if (_isSocketServer(_pollfds[i].fd))
		{
			int sd = accept(_pollfds[i].fd, (struct sockaddr *)&addrNew, &socklen);
			fcntl(sd, F_SETFL, O_NONBLOCK);
			if (sd < 0)
			{
				msg = "sd " + std::to_string(_pollfds[i].fd) + ": Accept error.";
				printMsgErr(-1, -1, msg, "");
				printMsgToLogFile(-1, -1, msg, "");
				return false;
				break ;
			}
			_pollfds[_pollfdsQty].fd = sd;
			_pollfds[_pollfdsQty].events = POLLIN;
			_pollfdsQty++;
			_saveConnection(sd, i, inet_ntoa(((struct sockaddr_in*)&addrNew)->sin_addr), ntohs(((struct sockaddr_in*)&addrNew)->sin_port));
			msg = "new connection from " + _getConnection(sd)->fromIp + ":" + std::to_string(_getConnection(sd)->fromPort) + ", sd: ";
			printMsg(i, sd, msg, "");
			printMsgToLogFile(i, sd, msg, "");
			// #ifdef DEBUGMODE
			// 	msg = "DEBUGMODE SR _mainLoop printAllConnections";
			// 	printAllConnections(_connections, msg);
			// #endif
		}
		else
		{
			int clntSd = _pollfds[i].fd;
			t_connection * connection = _getConnection(clntSd);
			connection->pfd = &(_pollfds[i]);
			char buf[BUF_SIZE + 1];
			if (_pollfds[i].revents & POLLIN) // есть данные для чтения
			{
				err = _readSd(connection);

				if (err < 0)
				{
					continue ;
				}

				// #ifdef DEBUGMODE
				// 	printConnection(* connection, "DEBUGMODE SR _mainLoop printConnection 1", 1);
				// #endif

				if (connection->requestProcessingStep == READING_DONE)
					connection->pfd->events = POLLOUT;

				// #ifdef DEBUGMODE
				// 	printConnection(* connection, "DEBUGMODE SR _mainLoop printConnection 2", 1);
				// #endif

			}
			else if (_pollfds[i].revents & POLLOUT) // запись возможна
			{
				err = _sendAnswer(connection);
				bool connectionClosed;

				connectionClosed = false;
				if (connection->requestProcessingStep == WRITING_DONE)
				{
					connection->pfd->events = POLLIN;
					connection->inputStr.clear();

					for (std::vector<std::pair<std::string, std::string> >::iterator iterF = connection->inputData.headerFieldsVec.begin(); iterF != connection->inputData.headerFieldsVec.end(); iterF++)
					{
						if ((*iterF).first.find("Connection") != std::string::npos && (*iterF).second.find("Close") != std::string::npos)
						{
							connection->pfd->revents = POLLIN;
							connectionClosed = true;
						}
					}

				}

				if (err < 0 || connectionClosed)
				{
					if (!connectionClosed)
					{
						msg = "client closed sd ";
						msg1 = "";
					}
					else
					{
						msg = "closed sd ";
						msg1 = " by Connection condition";
					}
					printMsg(connection->srvNbr, clntSd, msg, msg1);
					printMsgToLogFile(connection->srvNbr, clntSd, msg, msg1);
					_closeConnection (clntSd);
					continue ;
				}
			}
			else if (recv(clntSd, buf, BUF_SIZE, MSG_PEEK) == 0)
			{
				msg = "client closed sd ";
				printMsg(connection->srvNbr, clntSd, msg, "");
				printMsgToLogFile(connection->srvNbr, clntSd, msg, "");
				_closeConnection (clntSd);
			}
		}
	}
	return true;
}

int ServerRouter::_sendAnswer(t_connection * connection)
{
	std::string msg;
	std::string delim = DELIMETER;
	connection->responseData.connectionAnswer.clear();
	if (connection->inputData.dataType == HTTP)
	{
		connection->responseData.connectionAnswer = connection->inputData.httpVersion + " ";
		if (std::find(connection->allowedMethods.begin(), connection->allowedMethods.end(), connection->inputData.method) == connection->allowedMethods.end() )
		{
			msg = "Error! Unknown method from sd ";
			printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			connection->responseData.statusCode = "405";
			connection->responseData.connectionAnswer += connection->responseData.statusCode + " " + connection->responseStatusCodesAll[connection->responseData.statusCode] + DELIMETER;
		}
		else if (connection->inputData.method == "GET")
			_prepareGetAnswer(connection);
		else if (connection->inputData.method == "POST")
			_preparePostAnswer(connection);
		else if (connection->inputData.method == "DELETE")
			_prepareDeleteAnswer(connection);
	}

	connection->responseData.connectionAnswer += DDELIMETER; // TEMPORARY SOLUTION. It must be fixed when answer prepared

	size_t posTmp = connection->responseData.connectionAnswer.find(DDELIMETER);
	std::string answerHeaderTmp = connection->responseData.connectionAnswer.substr(0, posTmp);
	msg = "prepared answer to sd ";
	printMsg(connection->srvNbr, connection->clntSd, msg, ":\n" + answerHeaderTmp);
	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, ":\n" + answerHeaderTmp);
	if (!connection->responseData.lenAnswer)
		connection->responseData.lenAnswer = connection->responseData.connectionAnswer.length();

	int sizePacket, bytesSent;

	sizePacket = (connection->responseData.lenAnswer - connection->responseData.lenSent) > BUF_SIZE ? BUF_SIZE : (connection->responseData.lenAnswer - connection->responseData.lenSent);

	bytesSent = send(connection->clntSd, (connection->responseData.connectionAnswer.substr (connection->responseData.lenSent, sizePacket)).c_str(), sizePacket, 0);
	if (bytesSent < 0)
		return bytesSent;
	connection->responseData.lenSent += bytesSent;
	msg = "sent " + std::to_string(connection->responseData.lenSent) + "/" + std::to_string(connection->responseData.lenAnswer) + " bytes to sd ";
	printMsgErr(connection->srvNbr, connection->clntSd, msg, "");
	printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
	if (connection->responseData.lenAnswer <= connection->responseData.lenSent)
	{
		connection->requestProcessingStep = WRITING_DONE;
		connection->responseData.lenAnswer = 0;
	}
	return bytesSent; //?
}

int ServerRouter::_readSd(t_connection * connection)
{
	Server server = _getServer(connection->srvNbr);
	char buf[BUF_SIZE + 1];
	std::string msg, msg1;
	std::string tmpDDelimeter = DDELIMETER;

	int qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
	if (qtyBytes == 0) // ?? было ли всё полностью получено? не было ли разрыва?
	{
		return 0;
	}
	else if (qtyBytes > 0)
	{
		buf[qtyBytes] = '\0';
		connection->lenGet += qtyBytes;
		size_t amount;
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR _readSd buf \nbuf: " << NC << buf << "\n----------------------" << std::endl;
		// #endif

		// #ifdef DEBUGMODE
		// 	std::cout << BLUE << " DEBUGMODE SR _readSd (connection->inputStr).size()0 \n(connection->inputStr).size()0: " << NC << (connection->inputStr).size() << "\n----------------------" << std::endl;
		// #endif
		connection->inputStr.append(buf, qtyBytes);
		// #ifdef DEBUGMODE
		// 	std::cout << GREEN << " DEBUGMODE SR _readSd (connection->inputStr).size() \n(connection->inputStr).size(): " << NC << (connection->inputStr).size() << "\n----------------------" << std::endl;
		// #endif

		std::string tmp = buf;
		std::string tmpBody = "";

		if (connection->requestProcessingStep == NOT_DEFINED_REQUEST_PROCESSING_STEP)
		{
			// первый запуск
			size_t pos = tmp.find(DDELIMETER);
			connection->inputStrHeader = tmp.substr (0, pos);
			if (!_parseInputDataHeader(connection))
			{
				// ничего не пришло, выход
				while (qtyBytes > 0)
					qtyBytes = recv(connection->clntSd, buf, BUF_SIZE, 0);
				// connection->inputStr = "";
				return -1;
			}
			connection->requestProcessingStep = READING_HEADER;
			connection->responseData.statusCode = "100";
			if (pos != std::string::npos)
			{
				// есть body
				connection->inputStrBody = std::string(connection->inputStr, pos + tmpDDelimeter.size(), qtyBytes - pos - tmpDDelimeter.size());
				if (!_findConnectionСontentLength(connection))
				{
					// body закончится в этом пакете
					connection->requestProcessingStep = READING_DONE;
					connection->responseData.statusCode = "200";
				}
				else
				{
					unsigned long bodyBinarSize = server.getConfig().limitClientBodySize;
					// сравнить размер пришедших данных с максимально допустимым в конфигурации
					// #ifdef DEBUGMODE
					// 	std::cout << GREEN << " DEBUGMODE SR _readSd connection->сontentLength \nconnection->сontentLength: " << NC << connection->сontentLength << "\n----------------------" << std::endl;
					// 	std::cout << GREEN << " DEBUGMODE SR _readSd bodyBinarSize \nbodyBinarSize: " << NC << bodyBinarSize << "\n----------------------" << std::endl;
					// #endif
					if (connection->сontentLength > bodyBinarSize + 1)
					{
						return 0 ;
					}
					else
					{
						// #ifdef DEBUGMODE
						// 	std::cout << VIOLET << " DEBUGMODE SR _readSd strlen(connection->bodyBinar) \nstrlen(connection->bodyBinar): " << NC << strlen(connection->bodyBinar) << "\n----------------------" << std::endl;
						// #endif
						// #ifdef DEBUGMODE
						// 	std::cout << VIOLET << " DEBUGMODE SR _readSd sizeof(buf) \nstrlen(buf): " << NC << sizeof(buf) << "\n----------------------" << std::endl;
						// 	std::cout << VIOLET << " DEBUGMODE SR _readSd strlen(connection->bodyBinar) \nstrlen(connection->bodyBinar): " << NC << strlen(connection->bodyBinar) << "\n----------------------" << std::endl;
						// #endif
						if (connection->lenGet < (connection->сontentLength + tmpDDelimeter.size() + connection->inputStrHeader.size()))
						{
							// данные еще будут
							connection->requestProcessingStep = READING_BODY;
						}
						else
						{
							connection->requestProcessingStep = READING_DONE;
							connection->responseData.statusCode = "200";
						}
					}
				}
			}
			else
			{
				// нет body
				if (_findConnectionСontentLength(connection) && (connection->lenGet < connection->сontentLength))
				{
					connection->requestProcessingStep = READING_HEADER;
				}
				else
				{
					connection->requestProcessingStep = READING_DONE;
					connection->responseData.statusCode = "200";
				}
			}
		}
		else
		{
			// первый запуск произошел, требуется более одного пакета данных
			// #ifdef DEBUGMODE
			// 	std::cout << VIOLET << " DEBUGMODE SR _readSd strlen(buf) \nstrlen(buf): " << NC << strlen(buf) << "\n----------------------" << std::endl;
			// 	std::cout << VIOLET << " DEBUGMODE SR _readSd strlen(connection->bodyBinar) \nstrlen(connection->bodyBinar): " << NC << strlen(connection->bodyBinar) << "\n----------------------" << std::endl;
			// #endif

			if (connection->requestProcessingStep < READING_BODY)
			{
				// заголовок еще не прочитан
				size_t pos = tmp.find(DDELIMETER);
				connection->inputStrHeader += tmp.substr (0, pos);
				if (pos != std::string::npos)
				{
					// есть body
					connection->inputStrBody = std::string(connection->inputStr, pos + tmpDDelimeter.size(), qtyBytes - pos - tmpDDelimeter.size());
					// длину body надо сверять с параметром Сontent-Length
	amount = connection->сontentLength > 0 ? (connection->сontentLength + tmpDDelimeter.size() + connection->inputStrHeader.size()) : (connection->inputStr).size();
					if (connection->lenGet < (connection->сontentLength + tmpDDelimeter.size() + connection->inputStrHeader.size()))
					{
						// данные еще будут
						connection->requestProcessingStep = READING_BODY;
					}
					else
					{
						connection->requestProcessingStep = READING_DONE;
					}
				}
				else
				{
					// нет body
					if (connection->lenGet < connection->сontentLength)
					{
						connection->requestProcessingStep = READING_HEADER;
					}
					else
					{
						connection->requestProcessingStep = READING_DONE;
					}
				}
			}
			else
			{
				// body непрочитано до конца
				connection->inputStrBody.append(buf, qtyBytes);
				// длину body надо сверять с параметром Сontent-Length
				if (connection->lenGet < (connection->сontentLength + tmpDDelimeter.size() + connection->inputStrHeader.size()))
				{
					// данные еще будут
					connection->requestProcessingStep = READING_BODY;
				}
				else
				{
					connection->requestProcessingStep = READING_DONE;
				}
			}
		}

		amount = connection->сontentLength > 0 ? (connection->сontentLength + tmpDDelimeter.size() + connection->inputStrHeader.size()) : (connection->inputStr).size();
		msg = "got " + std::to_string(connection->lenGet) + "/" + std::to_string(amount) + " bytes from sd ";
		printMsg(connection->srvNbr, connection->clntSd, msg, "");
		printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
		// #ifdef DEBUGMODE
		// 	std::string arr[] = {"NOT_DEFINED_REQUEST_PROCESSING_STEP", "READING_HEADER", "READING_HEADER_DONE", "READING_BODY", "READING_BODY_DONE", "WRITING", "WRITING_DONE"};
		// 	std::vector<std::string> sts(std::begin(arr), std::end(arr));
		// 	std::cout << VIOLET << " DEBUGMODE SR _readSd connection->requestProcessingStep \nconnection->requestProcessingStep: " << NC << sts[connection->requestProcessingStep] << "\n----------------------" << std::endl;
		// #endif
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR _readSd (connection->inputStr).size() \n(connection->inputStr).size(): " << NC << (connection->inputStr).size() << "\n----------------------" << std::endl;
		// #endif
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR _readSd (connection->inputStrHeader).size() \n(connection->inputStrHeader).size(): " << NC << (connection->inputStrHeader).size() << "\n----------------------" << std::endl;
		// #endif
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR _readSd (connection->inputStrBody).size() \n(connection->inputStrBody).size(): " << NC << (connection->inputStrBody).size() << "\n----------------------" << std::endl;
		// #endif
		if (connection->requestProcessingStep == READING_DONE)
		{
			connection->responseData.statusCode = "200";
			_setConnectionLastActivity(connection->lastActivityTime);
			if (connection->inputData.addressParamsStr != "")
				_parseParamsStr(connection);
			msg = "finished reading data from sd ";
			printMsg(connection->srvNbr, connection->clntSd, msg, "");
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, "");
			// #ifdef DEBUGMODE
			// 	std::cout << VIOLET << " DEBUGMODE SR _readSd sizeof(connection->bodyBinar) \nsizeof(connection->bodyBinar): " << NC << sizeof(connection->bodyBinar) << "\n----------------------" << std::endl;
			// #endif
			msg = "data from sd ";
			msg1 = "✧✧✧✧✧header:✧✧✧✧✧\n" + connection->inputData.method + " " + connection->inputData.address;
			if (connection->inputData.addressParamsStr != "")
				msg1 += "?" + connection->inputData.addressParamsStr;
			msg1 += " " + connection->inputData.httpVersion + "\n";
			// for (std::map<std::string, std::string>::iterator iter = connection->inputData.headerFields.begin(); iter != connection->inputData.headerFields.end(); iter++)
			// 	msg1 += (*iter).first + ": " + (*iter).second + "\n";
			for (std::vector<std::pair<std::string, std::string> >::iterator iter = connection->inputData.headerFieldsVec.begin(); iter != connection->inputData.headerFieldsVec.end(); iter++)
			{	msg1 += (*iter).first;
				if ((*iter).second != "")
					msg1 += ": " + (*iter).second + "\n";
			}
			// msg1 += "✧✧✧✧✧body:✧✧✧✧✧\n";
			// msg1 += connection->inputStrBody;
			printMsg(connection->srvNbr, connection->clntSd, msg, ":\n" + msg1);
			printMsgToLogFile(connection->srvNbr, connection->clntSd, msg, ":\n" + msg1);
		}
	}
	return qtyBytes;
}

void ServerRouter::_removeSdFromPollfds(int clntSd)
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd == clntSd)
		{
			_pollfds[i].fd = -1;
			for (size_t j = i; j < _pollfdsQty - 1; j++)
				_pollfds[j] = _pollfds[j + 1];
			_pollfdsQty--;
		}
	}
}

void ServerRouter::_removeConnection(int clntSd)
{
	for (std::vector<t_connection>::iterator iter = _connections.begin(); iter < _connections.end(); iter++)
	{
		if ((*iter).clntSd == clntSd)
			_connections.erase(iter);
	}
}

void ServerRouter::_closeSockets()
{
	for (size_t i = 0; i < sizeof(_servers); i++)
	{
		if (_pollfds[i].fd >= 0)
			close (_pollfds[i].fd);
	}
}

void ServerRouter::_closeConnection(int clntSd)
{
	close (clntSd);
	_removeSdFromPollfds(clntSd);
	_removeConnection(clntSd);
}

bool ServerRouter::_isSocketServer(int fd)
{
	// #ifdef DEBUGMODE
	// 	std::cout << VIOLET << " DEBUGMODE SR _isSocketServer \nfd: " << NC << fd << std::endl;
	// #endif
	for (std::vector<Server>::iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << "_sd: " << NC << (*iter).getSd() << std::endl;
		// #endif
		if ((*iter).getSd() == fd)
		{
			// #ifdef DEBUGMODE
			// 	std::cout << "----------------------" << std::endl;
			// #endif
			return true;
		}
	}
	// #ifdef DEBUGMODE
	// 	std::cout << "----------------------" << std::endl;
	// #endif
	return false;
}

std::string ServerRouter::_extractLocalAddress(std::string const & address)
{
	size_t pos = address.find('/');
	return address.substr(pos + 1);
}

void ServerRouter::_findReferer(t_connection * connection)
{
	std::string referer;
	for (std::vector<std::pair<std::string, std::string> >::iterator iter = connection->inputData.headerFieldsVec.begin(); iter < connection->inputData.headerFieldsVec.end(); iter++)
	{
		if ((*iter).first.find("Referer") != std::string::npos)
			referer = (*iter).second;
	}
	connection->referer = urlDecode(referer);
}

t_connection * ServerRouter::_getConnection(int clntSd)
{
	for (std::vector<t_connection>::iterator iter = _connections.begin(); iter < _connections.end(); iter++)
	{
		if ((*iter).clntSd == clntSd)
			return &(*iter);
	}
	return NULL;
}

void ServerRouter::_saveConnection(int sdFrom, int srvNbr, std::string fromIP, unsigned fromPort)
{
	t_connection connection;
	connectionClear(connection);
	connection.srvNbr = srvNbr;
	connection.fromIp = fromIP;
	connection.fromPort = fromPort;
	connection.clntSd = sdFrom;
	connection.allowedMethods = _allowedMethods;
	connection.responseStatusCodesAll = _responseStatusCodes;
	connection.contentTypesAll = _contentTypes;
	_setConnectionLastActivity(connection.lastActivityTime);
	_connections.push_back(connection);
}

void ServerRouter::_setConnectionLastActivity(long int & lastActivityTime)
{
	struct timeval	tm;
	gettimeofday(&tm, NULL);
	lastActivityTime = tm.tv_sec;
}

void ServerRouter::_checkTimeout()
{
	std::string msg, msg1;
	struct timeval	tm1;
	gettimeofday(&tm1, NULL);
	for (std::vector<t_connection>::iterator iter = _connections.begin(); iter < _connections.end(); iter++)
	{
		// #ifdef DEBUGMODE
		// 	std::cout << VIOLET << " DEBUGMODE SR _checkTimeout \ntime now: " << NC << tm1.tv_sec << "\tlastActivityTime: " << (*iter).lastActivityTime << "\tTIMEOUT: " << TIMEOUT << "\n----------------------" << std::endl;
		// #endif
		bool keepAlive = false;
		for (std::vector<std::pair<std::string, std::string> >::iterator iterF = (*iter).inputData.headerFieldsVec.begin(); iterF != (*iter).inputData.headerFieldsVec.end(); iterF++)
		{
			size_t pos = (*iterF).second.find("keep-alive");
    		if (pos != std::string::npos)
				keepAlive = true;
		}
		if ((tm1.tv_sec - (*iter).lastActivityTime >= TIMEOUT))
		{
			msg = "closed sd ";
			msg1 = " by timeout";
			printMsg((*iter).srvNbr, (*iter).clntSd, msg, msg1);
			printMsgToLogFile((*iter).srvNbr, (*iter).clntSd, msg, msg1);
			_closeConnection((*iter).clntSd);
		}
	}
}

std::string ServerRouter::_getStatusCodeDescription(std::string statusCode)
{
	for (std::map<std::string, std::string>::iterator iter = _responseStatusCodes.begin(); iter != _responseStatusCodes.end(); iter++)
	{
		if ((*iter).first == statusCode)
			return (*iter).second;
	}
	return "Not Defined";
}

void ServerRouter::_responseStatusCodesInit()
{
	_responseStatusCodes.clear();
	std::pair<std::string, std::string> responseStatusCodesArr[] =
	{
		std::make_pair("100", "Continue"), std::make_pair("101", "Switching Protocols"), std::make_pair("102", "Processing"), 
		std::make_pair("103", "Early Hints"), std::make_pair("200", "OK"), std::make_pair("201", "Created"), std::make_pair("202", "Accepted"), 
		std::make_pair("203", "Non-Authoritative Information"), std::make_pair("204", "No Content"), std::make_pair("205", "Reset Content"), 
		std::make_pair("206", "Partial Content"), std::make_pair("207", "Multi-Status"), std::make_pair("208", "Already Reported"), 
		std::make_pair("226", "IM Used"), std::make_pair("300", "Multiple Choices"), std::make_pair("301", "Moved Permanently"), 
		std::make_pair("302", "Found"), std::make_pair("303", "See Other"), std::make_pair("304", "Not Modified"), 
		std::make_pair("305", "Use Proxy"), std::make_pair("307", "Temporary Redirect"), std::make_pair("308", "Permanent Redirect"), 
		std::make_pair("400", "Bad Request"), std::make_pair("401", "Unauthorized"), std::make_pair("402", "Payment Required"), 
		std::make_pair("403", "Forbidden"), std::make_pair("404", "Not Found"), std::make_pair("405", "Method Not Allowed"), 
		std::make_pair("406", "Not Acceptable"), std::make_pair("407", "Proxy Authentication Required"), 
		std::make_pair("408", "Request Timeout"), std::make_pair("409", "Conflict"), std::make_pair("410", "Gone"), 
		std::make_pair("411", "Length Required"), std::make_pair("412", "Precondition Failed"), std::make_pair("413", "Payload Too Large"), 
		std::make_pair("414", "URI Too Long"), std::make_pair("415", "Unsupported Media Type"), std::make_pair("416", "Range Not Satisfiable"), 
		std::make_pair("417", "Expectation Failed"), std::make_pair("418", "I'm a teapot"), std::make_pair("421", "Misdirected Request"), 
		std::make_pair("422", "Unprocessable Content"), std::make_pair("423", "Locked"), std::make_pair("424", "Failed Dependency"), 
		std::make_pair("425", "Too Early"), std::make_pair("426", "Upgrade Required"), std::make_pair("428", "Precondition Required"), 
		std::make_pair("429", "Too Many Requests"), std::make_pair("431", "Request Header Fields Too Large"), 
		std::make_pair("451", "Unavailable For Legal Reasons"), std::make_pair("500", "Internal Server Error"), 
		std::make_pair("501", "Not Implemented"), std::make_pair("502", "Bad Gateway"), std::make_pair("503", "Service Unavailable"), 
		std::make_pair("504", "Gateway Timeout"), std::make_pair("505", "HTTP Version Not Supported"), 
		std::make_pair("506", "Variant Also Negotiates"), std::make_pair("507", "Insufficient Storage"), std::make_pair("508", "Loop Detected"), 
		std::make_pair("510", "Not Extended"), std::make_pair("511", "Network Authentication Required")
	};

	#define ARRSIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
	for (size_t i = 0; i < ARRSIZE(responseStatusCodesArr); i++)
		_responseStatusCodes[responseStatusCodesArr[i].first] = responseStatusCodesArr[i].second;
	#undef  ARRSIZE
}

void ServerRouter::_contentTypesInit()
{
	_contentTypes.clear();
	std::pair<std::string, std::string> contentTypesArr[] =
	{
		std::make_pair("3dm", "x-world/x-3dmf"), 
		std::make_pair("3dmf", "x-world/x-3dmf"), 
		std::make_pair("a", "application/octet-stream"), 
		std::make_pair("aab", "application/x-authorware-bin"), 
		std::make_pair("aam", "application/x-authorware-map"), 
		std::make_pair("aas", "application/x-authorware-seg"), 
		std::make_pair("abc", "text/vnd.abc"), 
		std::make_pair("acgi", "text/html"), 
		std::make_pair("afl", "video/animaflex"), 
		std::make_pair("ai", "application/postscript"), 
		std::make_pair("aif", "audio/aiff"), 
		std::make_pair("aifc", "audio/aiff"), 
		std::make_pair("aiff", "audio/aiff"), 
		std::make_pair("aim", "application/x-aim"), 
		std::make_pair("aip", "text/x-audiosoft-intra"), 
		std::make_pair("ani", "application/x-navi-animation"), 
		std::make_pair("aos", "application/x-nokia-9000-communicator-add-on-software"), 
		std::make_pair("aps", "application/mime"), 
		std::make_pair("arc", "application/octet-stream"), 
		std::make_pair("arj", "application/arj"), 
		std::make_pair("art", "image/x-jg"), 
		std::make_pair("asf", "video/x-ms-asf"), 
		std::make_pair("asm", "text/x-asm"), 
		std::make_pair("asp", "text/asp"), 
		std::make_pair("asx", "video/x-ms-asf"), 
		std::make_pair("au", "audio/x-au"), 
		std::make_pair("avi", "video/avi"), 
		std::make_pair("avs", "video/avs-video"), 
		std::make_pair("bcpio", "application/x-bcpio"), 
		std::make_pair("bin", "application/mac-binary"), 
		std::make_pair("bm", "image/bmp"), 
		std::make_pair("bmp", "image/bmp"), 
		std::make_pair("boo", "application/book"), 
		std::make_pair("book", "application/book"), 
		std::make_pair("boz", "application/x-bzip2"), 
		std::make_pair("bsh", "application/x-bsh"), 
		std::make_pair("bz", "application/x-bzip"), 
		std::make_pair("bz2", "application/x-bzip2"), 
		std::make_pair("c", "text/plain"), 
		std::make_pair("c++", "text/plain"), 
		std::make_pair("cat", "application/vnd.ms-pki.seccat"), 
		std::make_pair("cc", "text/plain"), 
		std::make_pair("ccad", "application/clariscad"), 
		std::make_pair("cco", "application/x-cocoa"), 
		std::make_pair("cdf", "application/cdf"), 
		std::make_pair("cer", "application/pkix-cert"), 
		std::make_pair("cha", "application/x-chat"), 
		std::make_pair("chat", "application/x-chat"), 
		std::make_pair("class", "application/java"), 
		std::make_pair("com", "text/plain"), 
		std::make_pair("conf", "text/plain"), 
		std::make_pair("cpio", "application/x-cpio"), 
		std::make_pair("cpp", "text/x-c"), 
		std::make_pair("cpt", "application/x-cpt"), 
		std::make_pair("crl", "application/pkix-crl"), 
		std::make_pair("crt", "application/pkix-cert"), 
		std::make_pair("csh", "application/x-csh"), 
		std::make_pair("css", "text/css"), 
		std::make_pair("cxx", "text/plain"), 
		std::make_pair("dcr", "application/x-director"), 
		std::make_pair("deepv", "application/x-deepv"), 
		std::make_pair("def", "text/plain"), 
		std::make_pair("dif", "video/x-dv"), 
		std::make_pair("dir", "application/x-director"), 
		std::make_pair("dl", "video/dl"), 
		std::make_pair("doc", "application/msword"), 
		std::make_pair("dot", "application/msword"), 
		std::make_pair("dp", "application/commonground"), 
		std::make_pair("drw", "application/drafting"), 
		std::make_pair("dump", "application/octet-stream"), 
		std::make_pair("dv", "video/x-dv"), 
		std::make_pair("dvi", "application/x-dvi"), 
		std::make_pair("dwf", "model/vnd.dwf"), 
		std::make_pair("dwg", "image/x-dwg"), 
		std::make_pair("dxf", "application/dxf"), 
		std::make_pair("dxr", "application/x-director"), 
		std::make_pair("el", "text/x-script.elisp"), 
		std::make_pair("elc", "application/x-elc"), 
		std::make_pair("env", "application/x-envoy"), 
		std::make_pair("eps", "application/postscript"), 
		std::make_pair("es", "application/x-esrehber"), 
		std::make_pair("etx", "text/x-setext"), 
		std::make_pair("evy", "application/envoy"), 
		std::make_pair("exe", "application/octet-stream"), 
		std::make_pair("f", "text/plain"), 
		std::make_pair("f77", "text/x-fortran"), 
		std::make_pair("f90", "text/plain"), 
		std::make_pair("fdf", "application/vnd.fdf"), 
		std::make_pair("fif", "image/fif"), 
		std::make_pair("fli", "video/fli"), 
		std::make_pair("flo", "image/florian"), 
		std::make_pair("flx", "text/vnd.fmi.flexstor"), 
		std::make_pair("fmf", "video/x-atomic3d-feature"), 
		std::make_pair("for", "text/plain"), 
		std::make_pair("fpx", "image/vnd.fpx"), 
		std::make_pair("frl", "application/freeloader"), 
		std::make_pair("funk", "audio/make"), 
		std::make_pair("g", "text/plain"), 
		std::make_pair("g3", "image/g3fax"), 
		std::make_pair("gif", "image/gif"), 
		std::make_pair("gl", "video/gl"), 
		std::make_pair("gsd", "audio/x-gsm"), 
		std::make_pair("gsm", "audio/x-gsm"), 
		std::make_pair("gsp", "application/x-gsp"), 
		std::make_pair("gss", "application/x-gss"), 
		std::make_pair("gtar", "application/x-gtar"), 
		std::make_pair("gz", "application/x-compressed"), 
		std::make_pair("gzip", "application/x-gzip"), 
		std::make_pair("h", "text/plain"), 
		std::make_pair("hdf", "application/x-hdf"), 
		std::make_pair("help", "application/x-helpfile"), 
		std::make_pair("hgl", "application/vnd.hp-hpgl"), 
		std::make_pair("hh", "text/plain"), 
		std::make_pair("hlb", "text/x-script"), 
		std::make_pair("hlp", "application/hlp"), 
		std::make_pair("hpg", "application/vnd.hp-hpgl"), 
		std::make_pair("hpgl", "application/vnd.hp-hpgl"), 
		std::make_pair("hqx", "application/binhex"), 
		std::make_pair("hta", "application/hta"), 
		std::make_pair("htc", "text/x-component"), 
		std::make_pair("htm", "text/html"), 
		std::make_pair("html", "text/html"), 
		std::make_pair("htmls", "text/html"), 
		std::make_pair("htt", "text/webviewhtml"), 
		std::make_pair("htx", "text/html"), 
		std::make_pair("ice", "x-conference/x-cooltalk"), 
		std::make_pair("ico", "image/vnd.microsoft.icon"), 
		std::make_pair("idc", "text/plain"), 
		std::make_pair("ief", "image/ief"), 
		std::make_pair("iefs", "image/ief"), 
		std::make_pair("iges", "application/iges"), 
		std::make_pair("igs", "application/iges"), 
		std::make_pair("ima", "application/x-ima"), 
		std::make_pair("imap", "application/x-httpd-imap"), 
		std::make_pair("inf", "application/inf"), 
		std::make_pair("ins", "application/x-internett-signup"), 
		std::make_pair("ip", "application/x-ip2"), 
		std::make_pair("isu", "video/x-isvideo"), 
		std::make_pair("it", "audio/it"), 
		std::make_pair("iv", "application/x-inventor"), 
		std::make_pair("ivr", "i-world/i-vrml"), 
		std::make_pair("ivy", "application/x-livescreen"), 
		std::make_pair("jam", "audio/x-jam"), 
		std::make_pair("jav", "text/plain"), 
		std::make_pair("java", "text/plain"), 
		std::make_pair("jcm", "application/x-java-commerce"), 
		std::make_pair("jfif", "image/jpeg"), 
		std::make_pair("jfif-tbnl", "image/jpeg"), 
		std::make_pair("jpe", "image/jpeg"), 
		std::make_pair("jpeg", "image/jpeg"), 
		std::make_pair("jpg", "image/jpeg"), 
		std::make_pair("jps", "image/x-jps"), 
		std::make_pair("js", "text/javascript"), 
		std::make_pair("jut", "image/jutvision"), 
		std::make_pair("kar", "audio/midi"), 
		std::make_pair("ksh", "application/x-ksh"), 
		std::make_pair("la", "audio/nspaudio"), 
		std::make_pair("lam", "audio/x-liveaudio"), 
		std::make_pair("latex", "application/x-latex"), 
		std::make_pair("lha", "application/lha"), 
		std::make_pair("lhx", "application/octet-stream"), 
		std::make_pair("list", "text/plain"), 
		std::make_pair("lma", "audio/nspaudio"), 
		std::make_pair("log", "text/plain"), 
		std::make_pair("lsp", "application/x-lisp"), 
		std::make_pair("lst", "text/plain"), 
		std::make_pair("lsx", "text/x-la-asf"), 
		std::make_pair("ltx", "application/x-latex"), 
		std::make_pair("lzh", "application/x-lzh"), 
		std::make_pair("lzx", "application/lzx"), 
		std::make_pair("m", "text/plain"), 
		std::make_pair("m1v", "video/mpeg"), 
		std::make_pair("m2a", "audio/mpeg"), 
		std::make_pair("m2v", "video/mpeg"), 
		std::make_pair("m3u", "audio/x-mpequrl"), 
		std::make_pair("man", "application/x-troff-man"), 
		std::make_pair("map", "application/x-navimap"), 
		std::make_pair("mar", "text/plain"), 
		std::make_pair("mbd", "application/mbedlet"), 
		std::make_pair("mc$", "application/x-magic-cap-package-1.0"), 
		std::make_pair("mcd", "application/mcad"), 
		std::make_pair("mcd", "application/x-mathcad"), 
		std::make_pair("mcf", "image/vasa"), 
		std::make_pair("mcp", "application/netmc"), 
		std::make_pair("me", "application/x-troff-me"), 
		std::make_pair("mht", "message/rfc822"), 
		std::make_pair("mhtml", "message/rfc822"), 
		std::make_pair("mid", "audio/midi"), 
		std::make_pair("mid", "audio/x-midi"), 
		std::make_pair("midi", "audio/midi"), 
		std::make_pair("mif", "application/x-mif"), 
		std::make_pair("mime", "message/rfc822"), 
		std::make_pair("mjf", "audio/x-vnd.audioexplosion.mjuicemediafile"), 
		std::make_pair("mjpg", "video/x-motion-jpeg"), 
		std::make_pair("mm", "application/base64"), 
		std::make_pair("mme", "application/base64"), 
		std::make_pair("mod", "audio/mod"), 
		std::make_pair("moov", "video/quicktime"), 
		std::make_pair("mov", "video/quicktime"), 
		std::make_pair("movie", "video/x-sgi-movie"), 
		std::make_pair("mp2", "video/mpeg"), 
		std::make_pair("mp3", "audio/mpeg3"), 
		std::make_pair("mpa", "video/mpeg"), 
		std::make_pair("mpc", "application/x-project"), 
		std::make_pair("mpe", "video/mpeg"), 
		std::make_pair("mpeg", "video/mpeg"), 
		std::make_pair("mpg", "video/mpeg"), 
		std::make_pair("mpga", "audio/mpeg"), 
		std::make_pair("mpp", "application/vnd.ms-project"), 
		std::make_pair("mpt", "application/x-project"), 
		std::make_pair("mpv", "application/x-project"), 
		std::make_pair("mpx", "application/x-project"), 
		std::make_pair("mrc", "application/marc"), 
		std::make_pair("ms", "application/x-troff-ms"), 
		std::make_pair("mv", "video/x-sgi-movie"), 
		std::make_pair("my", "audio/make"), 
		std::make_pair("mzz", "application/x-vnd.audioexplosion.mzz"), 
		std::make_pair("nap", "image/naplps"), 
		std::make_pair("naplps", "image/naplps"), 
		std::make_pair("nc", "application/x-netcdf"), 
		std::make_pair("ncm", "application/vnd.nokia.configuration-message"), 
		std::make_pair("nif", "image/x-niff"), 
		std::make_pair("niff", "image/x-niff"), 
		std::make_pair("nix", "application/x-mix-transfer"), 
		std::make_pair("nsc", "application/x-conference"), 
		std::make_pair("nvd", "application/x-navidoc"), 
		std::make_pair("o", "application/octet-stream"), 
		std::make_pair("oda", "application/oda"), 
		std::make_pair("omc", "application/x-omc"), 
		std::make_pair("omcd", "application/x-omcdatamaker"), 
		std::make_pair("omcr", "application/x-omcregerator"), 
		std::make_pair("p", "text/x-pascal"), 
		std::make_pair("p10", "application/pkcs10"), 
		std::make_pair("p12", "application/pkcs-12"), 
		std::make_pair("p7a", "application/x-pkcs7-signature"), 
		std::make_pair("p7c", "application/pkcs7-mime"), 
		std::make_pair("p7m", "application/pkcs7-mime"), 
		std::make_pair("p7r", "application/x-pkcs7-certreqresp"), 
		std::make_pair("p7s", "application/pkcs7-signature"), 
		std::make_pair("part", "application/pro_eng"), 
		std::make_pair("pas", "text/pascal"), 
		std::make_pair("pbm", "image/x-portable-bitmap"), 
		std::make_pair("pcl", "application/vnd.hp-pcl"), 
		std::make_pair("pct", "image/x-pict"), 
		std::make_pair("pcx", "image/x-pcx"), 
		std::make_pair("pdb", "chemical/x-pdb"), 
		std::make_pair("pdf", "application/pdf"), 
		std::make_pair("pfunk", "audio/make"), 
		std::make_pair("pgm", "image/x-portable-graymap"), 
		std::make_pair("pic", "image/pict"), 
		std::make_pair("pict", "image/pict"), 
		std::make_pair("pkg", "application/x-newton-compatible-pkg"), 
		std::make_pair("pko", "application/vnd.ms-pki.pko"), 
		std::make_pair("pl", "text/plain"), 
		std::make_pair("plx", "application/x-pixclscript"), 
		std::make_pair("pm", "image/x-xpixmap"), 
		std::make_pair("pm4", "application/x-pagemaker"), 
		std::make_pair("pm5", "application/x-pagemaker"), 
		std::make_pair("png", "image/png"), 
		std::make_pair("pnm", "application/x-portable-anymap"), 
		std::make_pair("pot", "application/mspowerpoint"), 
		std::make_pair("pov", "model/x-pov"), 
		std::make_pair("ppa", "application/vnd.ms-powerpoint"), 
		std::make_pair("ppm", "image/x-portable-pixmap"), 
		std::make_pair("pps", "application/mspowerpoint"), 
		std::make_pair("ppt", "application/mspowerpoint"), 
		std::make_pair("ppz", "application/mspowerpoint"), 
		std::make_pair("pre", "application/x-freelance"), 
		std::make_pair("prt", "application/pro_eng"), 
		std::make_pair("ps", "application/postscript"), 
		std::make_pair("psd", "application/octet-stream"), 
		std::make_pair("pvu", "paleovu/x-pv"), 
		std::make_pair("pwz", "application/vnd.ms-powerpoint"), 
		std::make_pair("py", "text/x-script.phyton"), 
		std::make_pair("pyc", "application/x-bytecode.python"), 
		std::make_pair("qcp", "audio/vnd.qcelp"), 
		std::make_pair("qd3", "x-world/x-3dmf"), 
		std::make_pair("qd3d", "x-world/x-3dmf"), 
		std::make_pair("qif", "image/x-quicktime"), 
		std::make_pair("qt", "video/quicktime"), 
		std::make_pair("qtc", "video/x-qtc"), 
		std::make_pair("qti", "image/x-quicktime"), 
		std::make_pair("qtif", "image/x-quicktime"), 
		std::make_pair("ra", "audio/x-pn-realaudio"), 
		std::make_pair("ram", "audio/x-pn-realaudio"), 
		std::make_pair("ras", "image/cmu-raster"), 
		std::make_pair("rast", "image/cmu-raster"), 
		std::make_pair("rexx", "text/x-script.rexx"), 
		std::make_pair("rf", "image/vnd.rn-realflash"), 
		std::make_pair("rgb", "image/x-rgb"), 
		std::make_pair("rm", "application/vnd.rn-realmedia"), 
		std::make_pair("rmi", "audio/mid"), 
		std::make_pair("rmm", "audio/x-pn-realaudio"), 
		std::make_pair("rmp", "audio/x-pn-realaudio"), 
		std::make_pair("rng", "application/ringing-tones"), 
		std::make_pair("rnx", "application/vnd.rn-realplayer"), 
		std::make_pair("roff", "application/x-troff"), 
		std::make_pair("rp", "image/vnd.rn-realpix"), 
		std::make_pair("rpm", "audio/x-pn-realaudio-plugin"), 
		std::make_pair("rt", "text/richtext"), 
		std::make_pair("rtf", "text/richtext"), 
		std::make_pair("rtx", "text/richtext"), 
		std::make_pair("rv", "video/vnd.rn-realvideo"), 
		std::make_pair("s", "text/x-asm"), 
		std::make_pair("s3m", "audio/s3m"), 
		std::make_pair("saveme", "application/octet-stream"), 
		std::make_pair("sbk", "application/x-tbook"), 
		std::make_pair("scm", "video/x-scm"), 
		std::make_pair("sdml", "text/plain"), 
		std::make_pair("sdp", "application/sdp"), 
		std::make_pair("sdr", "application/sounder"), 
		std::make_pair("sea", "application/sea"), 
		std::make_pair("set", "application/set"), 
		std::make_pair("sgm", "text/sgml"), 
		std::make_pair("sgml", "text/sgml"), 
		std::make_pair("sh", "application/x-sh"), 
		std::make_pair("shar", "application/x-shar"), 
		std::make_pair("shtml", "text/html"), 
		std::make_pair("sid", "audio/x-psid"), 
		std::make_pair("sit", "application/x-sit"), 
		std::make_pair("skd", "application/x-koan"), 
		std::make_pair("skm", "application/x-koan"), 
		std::make_pair("skp", "application/x-koan"), 
		std::make_pair("skt", "application/x-koan"), 
		std::make_pair("sl", "application/x-seelogo"), 
		std::make_pair("smi", "application/smil"), 
		std::make_pair("smil", "application/smil"), 
		std::make_pair("snd", "audio/basic"), 
		std::make_pair("snd", "audio/x-adpcm"), 
		std::make_pair("sol", "application/solids"), 
		std::make_pair("spc", "application/x-pkcs7-certificates"), 
		std::make_pair("spl", "application/futuresplash"), 
		std::make_pair("spr", "application/x-sprite"), 
		std::make_pair("sprite", "application/x-sprite"), 
		std::make_pair("src", "application/x-wais-source"), 
		std::make_pair("ssi", "text/x-server-parsed-html"), 
		std::make_pair("ssm", "application/streamingmedia"), 
		std::make_pair("sst", "application/vnd.ms-pki.certstore"), 
		std::make_pair("step", "application/step"), 
		std::make_pair("stl", "application/sla"), 
		std::make_pair("stp", "application/step"), 
		std::make_pair("sv4cpio", "application/x-sv4cpio"), 
		std::make_pair("sv4crc", "application/x-sv4crc"), 
		std::make_pair("svf", "image/vnd.dwg"), 
		std::make_pair("svr", "application/x-world"), 
		std::make_pair("swf", "application/x-shockwave-flash"), 
		std::make_pair("t", "application/x-troff"), 
		std::make_pair("talk", "text/x-speech"), 
		std::make_pair("tar", "application/x-tar"), 
		std::make_pair("tbk", "application/toolbook"), 
		std::make_pair("tcl", "application/x-tcl"), 
		std::make_pair("tcsh", "text/x-script.tcsh"), 
		std::make_pair("tex", "application/x-tex"), 
		std::make_pair("texi", "application/x-texinfo"), 
		std::make_pair("texinfo", "application/x-texinfo"), 
		std::make_pair("text", "text/plain"), 
		std::make_pair("tgz", "application/gnutar"), 
		std::make_pair("tgz", "application/x-compressed"), 
		std::make_pair("tif", "image/tiff"), 
		std::make_pair("tiff", "image/tiff"), 
		std::make_pair("tr", "application/x-troff"), 
		std::make_pair("tsi", "audio/tsp-audio"), 
		std::make_pair("tsp", "audio/tsplayer"), 
		std::make_pair("tsv", "text/tab-separated-values"), 
		std::make_pair("turbot", "image/florian"), 
		std::make_pair("txt", "text/plain"), 
		std::make_pair("uil", "text/x-uil"), 
		std::make_pair("uni", "text/uri-list"), 
		std::make_pair("unis", "text/uri-list"), 
		std::make_pair("unv", "application/i-deas"), 
		std::make_pair("uri", "text/uri-list"), 
		std::make_pair("uris", "text/uri-list"), 
		std::make_pair("ustar", "application/x-ustar"), 
		std::make_pair("ustar", "multipart/x-ustar"), 
		std::make_pair("uu", "application/octet-stream"), 
		std::make_pair("uue", "text/x-uuencode"), 
		std::make_pair("vcd", "application/x-cdlink"), 
		std::make_pair("vcs", "text/x-vcalendar"), 
		std::make_pair("vda", "application/vda"), 
		std::make_pair("vdo", "video/vdo"), 
		std::make_pair("vew", "application/groupwise"), 
		std::make_pair("viv", "video/vivo"), 
		std::make_pair("vivo", "video/vivo"), 
		std::make_pair("vmd", "application/vocaltec-media-desc"), 
		std::make_pair("vmf", "application/vocaltec-media-file"), 
		std::make_pair("voc", "audio/voc"), 
		std::make_pair("vos", "video/vosaic"), 
		std::make_pair("vox", "audio/voxware"), 
		std::make_pair("vqe", "audio/x-twinvq-plugin"), 
		std::make_pair("vqf", "audio/x-twinvq"), 
		std::make_pair("vql", "audio/x-twinvq-plugin"), 
		std::make_pair("vrml", "application/x-vrml"), 
		std::make_pair("vrt", "x-world/x-vrt"), 
		std::make_pair("vsd", "application/x-visio"), 
		std::make_pair("vst", "application/x-visio"), 
		std::make_pair("vsw", "application/x-visio"), 
		std::make_pair("w60", "application/wordperfect6.0"), 
		std::make_pair("w61", "application/wordperfect6.1"), 
		std::make_pair("w6w", "application/msword"), 
		std::make_pair("wav", "audio/wav"), 
		std::make_pair("wb1", "application/x-qpro"), 
		std::make_pair("wbmp", "image/vnd.wap.wbmp"), 
		std::make_pair("web", "application/vnd.xara"), 
		std::make_pair("wiz", "application/msword"), 
		std::make_pair("wk1", "application/x-123"), 
		std::make_pair("wmf", "windows/metafile"), 
		std::make_pair("wml", "text/vnd.wap.wml"), 
		std::make_pair("wmlc", "application/vnd.wap.wmlc"), 
		std::make_pair("wmls", "text/vnd.wap.wmlscript"), 
		std::make_pair("wmlsc", "application/vnd.wap.wmlscriptc"), 
		std::make_pair("word", "application/msword"), 
		std::make_pair("wp", "application/wordperfect"), 
		std::make_pair("wp5", "application/wordperfect"), 
		std::make_pair("wp6", "application/wordperfect"), 
		std::make_pair("wpd", "application/x-wpwin"), 
		std::make_pair("wq1", "application/x-lotus"), 
		std::make_pair("wri", "application/mswrite"), 
		std::make_pair("wrl", "model/vrml"), 
		std::make_pair("wrz", "model/vrml"), 
		std::make_pair("wsc", "text/scriplet"), 
		std::make_pair("wsrc", "application/x-wais-source"), 
		std::make_pair("wtk", "application/x-wintalk"), 
		std::make_pair("xbm", "image/xbm"), 
		std::make_pair("xdr", "video/x-amt-demorun"), 
		std::make_pair("xgz", "xgl/drawing"), 
		std::make_pair("xif", "image/vnd.xiff"), 
		std::make_pair("xl", "application/excel"), 
		std::make_pair("xla", "application/excel"), 
		std::make_pair("xlb", "application/excel"), 
		std::make_pair("xlc", "application/excel"), 
		std::make_pair("xld", "application/excel"), 
		std::make_pair("xlk", "application/excel"), 
		std::make_pair("xll", "application/excel"), 
		std::make_pair("xlm", "application/excel"), 
		std::make_pair("xls", "application/excel"), 
		std::make_pair("xlt", "application/excel"), 
		std::make_pair("xlv", "application/excel"), 
		std::make_pair("xlv", "application/x-excel"), 
		std::make_pair("xlw", "application/excel"), 
		std::make_pair("xm", "audio/xm"), 
		std::make_pair("xml", "text/xml"), 
		std::make_pair("xmz", "xgl/movie"), 
		std::make_pair("xpix", "application/x-vnd.ls-xpix"), 
		std::make_pair("xpm", "image/xpm"), 
		std::make_pair("x-png", "image/png"), 
		std::make_pair("xsr", "video/x-amt-showrun"), 
		std::make_pair("xwd", "image/x-xwd"), 
		std::make_pair("xyz", "chemical/x-pdb"), 
		std::make_pair("z", "application/x-compressed"), 
		std::make_pair("zip", "application/zip"), 
		std::make_pair("zoo", "application/octet-stream"), 
		std::make_pair("zsh", "text/x-script.zsh")
	};

	#define ARRSIZE(arr) (sizeof((arr))/sizeof((arr)[0]))
	for (size_t i = 0; i < ARRSIZE(contentTypesArr); i++)
		_contentTypes[contentTypesArr[i].first] = contentTypesArr[i].second;
	#undef  ARRSIZE
}

void ServerRouter::_pollfdsInit()
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		_pollfds[i].fd = 0;
		_pollfds[i].events = 0;
		_pollfds[i].revents = 0;
	}
}

short ServerRouter::getEvents(int sd)
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd == sd)
			return _pollfds[i].events;
	}
	return -1;
}

short ServerRouter::getRevents(int sd)
{
	for (size_t i = 0; i < _pollfdsQty; i++)
	{
		if (_pollfds[i].fd == sd)
			return _pollfds[i].revents;
	}
	return -1;
}

std::vector<t_config> ServerRouter::getConfigs() const
	{ return _configs; }

std::string ServerRouter::getHostname() const
	{ return _hostname; }

std::vector<Server> ServerRouter::getServers() const
	{ return _servers; }

const pollfd * ServerRouter::getPollfds() const
	{ return  _pollfds; }

size_t ServerRouter::getPollfdsQty() const
	{ return _pollfdsQty; }

std::vector<t_connection> ServerRouter::getConnections() const
	{ return _connections; }

std::map<std::string, std::string> ServerRouter::getResponseStatusCodes() const
	{ return _responseStatusCodes; }

std::map<std::string, std::string> ServerRouter::getContentTypes() const
	{ return _contentTypes; }

Server ServerRouter::_getServer(int srvNbr) const
{
	for (std::vector<Server>::const_iterator iter = _servers.begin(); iter < _servers.end(); iter++)
	{
		if ((*iter).getServNbr() == srvNbr)
			return *iter;
	}
	return _servers[0];
}

bool ServerRouter::_isPathAutoindex(t_connection * connection)
{
	Server server = _getServer(connection->srvNbr);
	t_config config = server.getConfig();
	bool outp = false;
	if (config.autoindex)
		outp = true;
	for (size_t i = 0; i < server.getConfig().locations.size(); i++)
	{
		if (connection->inputData.address == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].autoindex)
				outp = true;
		}
	}
	return outp;
}
