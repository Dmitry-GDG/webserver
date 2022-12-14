#ifndef SERVERROUTER_HPP
# define SERVERROUTER_HPP

# include "main.hpp"

class Server;

class ServerRouter
{
	private:
		std::string				_configFile; //config file name
		std::vector<t_config>	_configs; //store vector of all server configs
		std::vector<Server>		_servers;
		std::string				_hostname; // il-a3.msk.21-school.ru
		char 					_serverIp[sizeof(struct sockaddr_in)];
		
		pollfd	_pollfds[SD_MAXCOUNT]; // real size = _servers.size() + new income sds
		size_t	_pollfdsQty;

		std::vector<t_connection> _connections;
		std::map<std::string, std::string>	_responseStatusCodes;
		std::map<std::string, std::string>	_contentTypes;
		std::vector<std::string> _allowedMethods; // GET, POST, DELETE

	public:
		ServerRouter(std::vector<t_config> configs, std::string confifFile);
		~ServerRouter();

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
		bool	_launch(Server &, int);
		bool	_mainLoop();
		void	_closeSockets();
		void	_closeConnection(int clntSd);
		bool	_isSocketServer(int);
		void	_saveConnection(int, int,  std::string, unsigned);
		void	_setConnectionLastActivity(long int & lastActivityTime);
		void	_checkTimeout();
		void	_removeSdFromPollfds(int);
		void	_removeConnection(int);
		t_connection	* _getConnection(int clntSd);
		Server	_getServer(int srvNb) const;
		std::string	_getStatusCodeDescription(std::string statusCode);
		int		_readSd(t_connection *);
		int		_sendAnswer(t_connection *);
		void	_pollfdsInit();
		void	_responseStatusCodesInit();
		void	_contentTypesInit();
		void	_prepareGetAnswer(t_connection *);
		void	_preparePostAnswer(t_connection *);
		void	_postUrlencoded(t_connection * connection, std::string contentType);
		void	_postFormData(t_connection * connection, std::string contentType);
		void	_postMixed(t_connection * connection, std::string contentType);
		void	_findBoundary(std::string contentType, std::string &boundary);
		void	_postGetFileName(t_connection * connection, std::string str);
		void	_makefilelist(t_connection * connection);
		void	_postGetFilePathToSave(t_connection * connection);
		int		_postCheckIsFileExist(t_connection * connection);
		void	_postSaveFile(t_connection * connection);
		void	_postFindUniqueFileNameToSave(t_connection * connection);
		bool	_delGetPath(t_connection * connection, std::string & contentTypeAndLengthAndData);
		void	_prepareDeleteAnswer(t_connection *);
		bool	_addFileToAnswer(t_connection *, std::string & contentTypeAndLength);
		void	_findPathToStatusCodePage(t_connection * connection);
		void	_addStatusCodePage(t_connection * connection, std::string & contentTypeAndLengthAndData);
		bool	_acceptFile(t_connection * connection, std::string & contentTypeAndLength, std::string const & path);
		bool	_findConnection??ontentLength(t_connection *);
		std::string	_extractLocalAddress(std::string const & address);
		void	_findReferer(t_connection *);
		std::string	_createAutoindex(t_connection * connection);
		bool	_isPathAutoindex(t_connection * connection);

		bool	_checkDelimeterAtTheEnd(std::string str);
		void	_delWhiteSpacesStr(std::string & inptStr);
		void	_parseMultiStringData(std::vector<std::string>	splitBuf, t_connection * connection);
		bool	_parseInputDataHeader(t_connection * connection);
		void	_parseInputBodyStr(t_connection * connection);
		void	_parseParamsStr(t_connection * connection);
		void	_choosePostContentType(t_connection * connection);

};

#endif
