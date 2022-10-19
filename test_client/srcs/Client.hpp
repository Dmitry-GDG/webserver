#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "main.hpp"

// class CommandParced;

class Client
{
private:
	char *		_host;
	char *		_port;
	// std::string	_password;
	// std::string	_nick;
	// std::string	_user;
	// std::string	_name;
	// bool		_isPasswAccepted;
	// bool		_isNickAccepted;
	// bool		_isLoggedIn;
	bool		_isTimeToExit;
	std::string					_command;
	std::vector<std::string>	_commandArgs;
	bool						_isCommandInvalid;
	std::string					_msgToSend;
	std::map<std::string, std::string>	_contentTypes;

	int						_sd;
	struct sockaddr_in		_ircBotAddr;
	std::string 			_colors[4];
	unsigned				_nextColor;
	std::string				_hostname; // il-a3.msk.21-school.ru
	unsigned				_qtyMsgSent;
	std::string				_bufGet;
	std::string				_bufSend;

public:
	Client(char * host, char * port);
	~Client();

	void	start();
	// void	log_in();

	std::string	getHost() const;
	char *		getPort() const;
	// std::string	getPassword() const;
	// std::string	getNick() const;
	// std::string	getUser() const;
	unsigned	getQtyMsgSent() const;
	std::string	getCommand() const;
	std::vector<std::string>	getCommandArgs() const;

	// void	setPassword(int);
	// void	setNick(std::string);
	// void	setUser(std::string);
	void	setQtyMsgSent();

private:
	void	_contentTypesInit();
	void	_printMsgToTerminal(int sdFrom, int sdTo, std::string msg);
	void	_sendMsg(std::string msg); 
	void	_getMsgFromServer();
	// CommandParced	_splitCommands(std::string msg);
	// CommandParced	_splitCommandsFS(std::string msg);
	void	_quit();
	void	_makefilelist();
	void	_post(std::string msg);
	void	_splitCommandsPost(std::string msg);
	void	_preparePostRequest();
	std::string	_getFileType(std::string str);
	std::string	_getContentType(std::string str);
	bool	_isText(std::string str);

	void	_chelp();
	void	_welcome();
	void	_printfileslist();
	// void	_fileget(CommandParced);
	// void	_filesave(CommandParced);
};

#endif
