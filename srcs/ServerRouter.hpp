#ifndef SERVERROUTER_HPP
# define SERVERROUTER_HPP

# include "main.hpp"

class Server;

class ServerRouter
{
	private:
		std::vector<t_config>	_configs; //store vector of all server configs
		std::vector<Server>		_servers;
		std::string				_hostname; // il-a3.msk.21-school.ru
		// int						_sdMaxCount;
		// std::vector<Connection>	_connections;
		// std::vector<int> 		_allSds;
		
		pollfd	_pollfds[SD_MAXCOUNT]; // real size = _servers.size() + new income sds
		size_t	_pollfdsQty;
		// std::vector<int>		_sds;
		// std::vector<struct pollfd> _pollfds;

		// t_connection	_connections[SD_MAXCOUNT];
		std::vector<t_connection> _connections;
		std::map<std::string, std::string>	_responseStatusCodes;
		std::map<std::string, std::string>	_contentTypes;
		std::vector<std::string> _allowedMethods; // GET, POST, DELETE

	public:
		ServerRouter(std::vector<t_config> configs);
		~ServerRouter();

		// void launch();
		// void					getAllSds();
		void					start();
		short					getEvents(int sd);
		short					getRevents(int sd);
		std::vector<t_config>	getConfigs() const;
		std::string				getHostname() const;
		std::vector<Server>		getServers() const;
		const pollfd			* getPollfds() const;
		size_t					getPollfdsQty() const;
		std::vector<t_connection>	getConnections() const;
		std::map<std::string, std::string>	getResponseStatusCodes() const;
		std::map<std::string, std::string>	getContentTypes() const;

	private:
		// fd_set _getAllActiveSdSets();
		bool	_launch(Server &, int);
		bool	_mainLoop();
		void	_closeSockets();
		void	_closeConnection(int clntSd);
		bool	_isSocketServer(int);
		void	_saveConnection(int, int,  std::string, unsigned);
		void	_setConnectionLastActivity(long int & lastActivityTime);
		void	_checkTimeout();
		// void	_initInputdata(t_inputdata & data);
		void	_removeSdFromPollfds(int);
		void	_removeConnection(int);
		t_connection	* _getConnection(int clntSd);
		Server	_getServer(int srvNb) const;
		int		_readSd(t_connection *);
		int		_sendAnswer(t_connection *);
		// void	_parseInputData(char *, t_connection *);
		void	_pollfdsInit();
		void	_responseStatusCodesInit();
		void	_contentTypesInit();
		// bool	_responseCheckMethod();
		void	_prepareGetAnswer(std::string & answer, t_connection *);
		void	_preparePostAnswer(std::string & answer, t_connection *);
		void	_prepareDeleteAnswer(std::string & answer, t_connection *);
		void	_addFileToAnswer(std::string & contentTypeAndLength, t_connection *);
		// void	_addStatusCode(std::string & answer, t_connection * connection, std::string code);

		bool	_checkDelimeterAtTheEnd(std::string str);
		void	_delWhiteSpacesStr(std::string & inptStr);
		void	_splitStrDelimeter(std::string str, std::vector<std::string> & outp, std::string delim);
		void	_parseMultiStringData(std::vector<std::string>	splitBuf, t_connection * connection);
		bool	_parseInputData(t_connection * connection);

};

#endif
