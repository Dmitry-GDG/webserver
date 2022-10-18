#include "Client.hpp"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

Client::Client(char * host, char * port) :
	_host(host), _port(port)
{
	_contentTypesInit();
	_command.clear();
	_commandArgs.clear();
	_isCommandInvalid = false;
	_colors[0] = GREENS;
	_colors[1] = BLUS;
	_colors[2] = VIOLETS;
	_colors[3] = AQUAMARINES;
	_nextColor = 0;
	_qtyMsgSent = 0;
	_isTimeToExit = false;
	char hostname[HOSTNAME_LENGTH];
	_hostname = (gethostname(hostname, HOSTNAME_LENGTH) != -1) ? hostname : "\0";

	struct addrinfo hints, *servinfo, *p;
	char            s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((getaddrinfo(_host, _port, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo: " << stderr  << std::endl;
        throw std::exception();
    }
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((_sd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }
        if (connect(_sd, p->ai_addr, p->ai_addrlen) == -1)
        {
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        std::cerr  << "Can not connect to the server"  << std::endl;
        throw std::exception();
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    freeaddrinfo(servinfo);
}

void Client::start()
{
	std::cout << NC << timestamp() << YELLOS << NAME << ": Welcome to the " << CLIENT_NAME << " v." << CLIENT_VERSION << "\nby " \
	<< CLIENT_AUTHORS << ". " << HELP_CLIENT_MSG << "\nYour host is: " << NC << _hostname << std::endl;
	std::cout << YELLOS << "Connected to: " << NC << _host << ":" << _port << std::endl;
	std::cout << YELLOS << "Quit the client with CONTROL-C." << NC << std::endl;
	// log_in();

	while (21)
	{
		try
		{
			_bufSend.clear();
			std::getline(std::cin, _bufSend);
			std::string tmp = _bufSend;
			stringToUpper(tmp);
			if (tmp == "CHELP" || tmp == "CLIENTHELP")
			{
				_printMsgToTerminal(0, 0, "CHELP :");
				_printMsgToTerminal(0, 0, HELP_CLIENT_MSG);
				_printMsgToTerminal(0, 0, "\n");
			}
			else if (tmp == "CFL" || tmp == "CLIENTFILESLIST")
				_printfileslist();
			else if (tmp == "QUIT")
				_quit();
			else if (tmp.substr(0, 4) == "POST")
			{
				_post(_bufSend);
				_getMsgFromServer();
			}
			else
			{
				_sendMsg(_bufSend);
				_getMsgFromServer();
			}
		}
		catch (const std::exception &e)
		{
			close(_sd);
			return;
		}
	}
}

void	Client::_printMsgToTerminal(int sdFrom, int sdTo, std::string msg)
{
	if (sdFrom == 0 && sdTo == 0)
	{
		std::cout << NC << msg << NC;
		return;
	}
	std::cout << NC << timestamp();
	if (sdFrom == 0)
		std::cout << YELLOS << NAME << " to ";
	else
	{
		std::cout << _colors[_nextColor];
		_nextColor = (_nextColor + 1) % 4;
		std::cout << "ircserv to ";
	}
	if (sdTo == 0)
		std::cout << NAME;
	else
	{
		std::cout << "ircserv";
		
	}
	if (sdFrom == 0)
	{
		setQtyMsgSent();
		std::cout << " " << getQtyMsgSent() << " message";
	}
	std::cout << ": " << msg << NC << std::endl;
}

void Client::_sendMsg(std::string msg)
{
	if (msg != "")
	{
		_printMsgToTerminal(0, _sd, msg);
		if (msg.find(DELIMETER) != msg.length() - DELIMETER_LENGHT)
			msg += DELIMETER;
		int qtyBytesRemaining = msg.length();
		int qtyBytesSendedTotal = 0;
		int qtyBytesSended;
		while (qtyBytesRemaining > 0)
		{
			qtyBytesSended = send(_sd, msg.c_str() + qtyBytesSendedTotal, qtyBytesRemaining, 0);
			if (qtyBytesSended < 0)
			{
				// // https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/recv.2.html
				if (errno == EAGAIN) // The socket is marked non-blocking, and the receive operation would block, or a receive timeout had been set, and the timeout expired before data were received.
				{
					return ;
				}
				std::cerr << strerror(errno) << std::endl;
				break;
			}
			qtyBytesSendedTotal += qtyBytesSended;
			qtyBytesRemaining -= qtyBytesSended;
		}
	}
}

Client::~Client() {}

// CommandParced Client::_splitCommands(std::string msg)
// {
// 	CommandParced commandParced;
// 	if (msg[0] == ' ' || !msg[0])
// 	{
// 		commandParced.isCommandInvalid = true;
// 		return (commandParced);
// 	}
// 	std::string delimiter = " ";
// 	size_t pos = 0;
// 	std::string token;
// 	size_t i = 0;
// 	while (((pos = msg.find(delimiter)) != std::string::npos) && i < 6)
// 	{
// 		token = msg.substr(0, pos);
// 		if (token != " " && token != "")
// 			commandParced.args.push_back(token);
// 		msg.erase(0, pos + delimiter.length());
// 		i++;
// 	}
// 	if (msg != " " && msg != "")
// 		commandParced.args.push_back(msg);
// 	if (commandParced.args.size() < 4)
// 		return (commandParced);
// 	commandParced.args.erase(commandParced.args.begin(), commandParced.args.begin() + 3);
// 	commandParced.command = (commandParced.args[0]);
//     if (commandParced.command.front() == ':')
//         commandParced.command.erase(commandParced.command.begin());
// 	commandParced.args.erase(commandParced.args.begin());
// 	stringToUpper(commandParced.command);
// 	commandParced.command.erase( std::remove(commandParced.command.begin(), commandParced.command.end(), '\r'), commandParced.command.end() );
// 	commandParced.command.erase( std::remove(commandParced.command.begin(), commandParced.command.end(), '\n'), commandParced.command.end() );
// 	for (unsigned i = 0; i < commandParced.args.size(); i++)
// 	{
// 		commandParced.args[i].erase( std::remove(commandParced.args[i].begin(), commandParced.args[i].end(), '\r'), commandParced.args[i].end() );
// 		commandParced.args[i].erase( std::remove(commandParced.args[i].begin(), commandParced.args[i].end(), '\n'), commandParced.args[i].end() );
// 	}
// 	return (commandParced);
// }

void Client::_splitCommandsPost(std::string msg)
{
	if (msg[0] == ' ' || !msg[0])
	{
		_isCommandInvalid = true;
		return ;
	}

	std::string delimiter = " ";

	size_t pos = 1;
	std::string token;
	int i = 0;
	while (((pos = msg.find(delimiter)) != std::string::npos))
	{
		token = msg.substr(0, pos);
		if (token != " " && token != "")
		{
			_commandArgs.push_back(token);
			i++;
		}
		msg.erase(0, pos + delimiter.length());
	}
	if (msg != " " && msg != "")
		_commandArgs.push_back(msg);
	_command = (_commandArgs[0]);
	_commandArgs.erase(_commandArgs.begin());
	stringToUpper(_command);
	_command.erase( std::remove(_command.begin(), _command.end(), '\r'), _command.end() );
	_command.erase( std::remove(_command.begin(), _command.end(), '\n'), _command.end() );
	for (size_t i = 0; i < _commandArgs.size(); i++)
	{
		_commandArgs[i].erase( std::remove(_commandArgs[i].begin(), _commandArgs[i].end(), '\r'), _commandArgs[i].end() );
		_commandArgs[i].erase( std::remove(_commandArgs[i].begin(), _commandArgs[i].end(), '\n'), _commandArgs[i].end() );
	}
}

// CommandParced	Client::_splitCommandsFS(std::string msg)
// {
// 	CommandParced commandParced;
// 	if (msg[0] == ' ' || !msg[0])
// 	{
// 		commandParced.isCommandInvalid = true;
// 		return (commandParced);
// 	}

// 	std::string delimiter = " ";

// 	size_t pos = 1;
// 	std::string token;
// 	int i = 0;
// 	while (((pos = msg.find(delimiter)) != std::string::npos))
// 	{
// 		token = msg.substr(0, pos);
// 		if (token != " " && token != "")
// 		{
// 			commandParced.args.push_back(token);
// 			i++;
// 		}
// 		msg.erase(0, pos + delimiter.length());
// 	}
// 	if (msg != " " && msg != "")
// 		commandParced.args.push_back(msg);
// 	commandParced.command = (commandParced.args[0]);
// 	commandParced.args.erase(commandParced.args.begin());
// 	stringToUpper(commandParced.command);
// 	commandParced.command.erase( std::remove(commandParced.command.begin(), commandParced.command.end(), '\r'), commandParced.command.end() );
// 	commandParced.command.erase( std::remove(commandParced.command.begin(), commandParced.command.end(), '\n'), commandParced.command.end() );
// 	for (unsigned i = 0; i < commandParced.args.size(); i++)
// 	{
// 		commandParced.args[i].erase( std::remove(commandParced.args[i].begin(), commandParced.args[i].end(), '\r'), commandParced.args[i].end() );
// 		commandParced.args[i].erase( std::remove(commandParced.args[i].begin(), commandParced.args[i].end(), '\n'), commandParced.args[i].end() );
// 	}
// 	_command = commandParced.command;
// 	_commandArgs = commandParced.args;
// 	return (commandParced);
// }

// void Client::log_in()
// {
// 	std::string user;
// 	_getMsgFromServer();
// 	while (!_isLoggedIn)
// 	{
// 		if (_password == "")
// 		{
// 			_printMsgToTerminal(0, 0, "Please enter the correct password: ");
// 			std::cin >> _password;
// 		}
// 		_sendMsg("PASS " + _password);
// 		_bufGet = "";
// 		_getMsgFromServer();
// 		if (_bufGet.find("Password accepted") == std::string::npos)
// 		{
// 			while (_bufGet.find("Password accepted") == std::string::npos)
// 			{
// 				_printMsgToTerminal(0, 0, "Wrong password. Please enter the correct password: ");
// 				std::cin >> _password;
// 				_sendMsg("PASS " + _password);
// 				_getMsgFromServer();
// 			}
// 		}
// 		if (_nick == "")
// 		{
// 			_printMsgToTerminal(0, 0, "Please enter your nick: ");
// 			std::cin >> _nick;
// 		}
// 		_sendMsg("NICK " + _nick);
// 		_bufGet = "";
// 		_getMsgFromServer();
// 		if (_bufGet.find("Nickname accepted") == std::string::npos)
// 		{
// 			while (_bufGet.find("Nickname accepted") == std::string::npos)
// 			{
// 				_printMsgToTerminal(0, 0, "Wrong nickname. Try again: ");

// 				std::cin >> _nick;
// 				_sendMsg("NICK " + _nick);
// 				_getMsgFromServer();
// 			}
// 		}
// 		_bufGet = "";
// 		(_name == "") ? _user = "" : _user = _nick + " " + _hostname + " * " + _name;
// 		if (_user.size() < 7)
// 		{
// 			_printMsgToTerminal(0, 0, "Please enter your full real name: ");
// 			std::getline(std::cin, user);
// 			std::getline(std::cin, user);
// 			if (user != "")
// 				_user = _nick + " " + _hostname + " * " + user;
// 		}
// 		_sendMsg("USER " + _user);
// 		_getMsgFromServer();
// 		if (_bufGet.find("Username accepted") == std::string::npos)
// 		{
// 			while (_bufGet.find("Username accepted") == std::string::npos)
// 			{
// 				_printMsgToTerminal(0, 0, "Wrong username. Please enter your full real name: ");
// 				std::getline(std::cin, user);
// 				_user = _nick + " " + _hostname + " * " + user;
// 				_sendMsg("USER " + _user);
// 				_getMsgFromServer();
// 			}
// 		}
// 		_isLoggedIn = true;
// 		return;
// 	}
// }

void Client::_getMsgFromServer()
{
    char c_buf[BUFLEN];
    int bytesLeft;
    int bytes = 1;

    _bufGet.clear();
    while (_bufGet.find(DELIMETER) == std::string::npos && _bufGet.length() < sizeof(c_buf))
    {
        memset(c_buf, 0, sizeof(c_buf));
        bytes = recv(_sd, c_buf, sizeof(c_buf), MSG_PEEK);
        if (bytes < 0)
        {
            std::cerr << strerror(errno) << std::endl;
            throw std::exception();
        }
        if (bytes == 0)
		{
            throw std::exception();
		}

        bytesLeft = std::string(c_buf).find(DELIMETER);
        if (std::string(c_buf).find(DELIMETER) == std::string::npos)
            bytesLeft = bytes;
        else
            bytesLeft += DELIMETER_LENGHT;
        while (bytesLeft > 0)
        {
            memset(c_buf, 0, sizeof(c_buf));
            bytes = recv(_sd, c_buf, bytesLeft, 0);
            if (bytes < 0)
            {
                std::cerr << strerror(errno) << std::endl;
                throw std::exception();
            }
            if (bytes == 0)
			{
                throw std::exception();
			}
            bytesLeft -= bytes;
            _bufGet += c_buf;
        }
    }
	replaceAllDelimeters(_bufGet);
	_printMsgToTerminal(_sd, 0, _bufGet);
}

std::string Client::getHost() const
	{ return _host; }

char * Client::getPort() const
	{ return _port; }

// std::string Client::getPassword() const
// 	{return _password; }

// std::string Client::getNick() const
// 	{ return _nick; }

// std::string Client::getUser() const
// 	{ return _user; }

unsigned Client::getQtyMsgSent() const
	{ return _qtyMsgSent; }

std::string Client::getCommand() const
	{ return _command; }
std::vector<std::string> Client::getCommandArgs() const
	{ return _commandArgs; }

// void Client::setPassword(int pass)
// 	{ _password = pass; }

// void Client::setNick(std::string nick)
// 	{ _nick = nick; }

// void Client::setUser(std::string user)
// 	{ _user = user; }

void Client::setQtyMsgSent()
	{ _qtyMsgSent++; }

void Client::_contentTypesInit()
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
		std::make_pair("ico", "image/x-icon"), 
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
