#ifndef SERVERROUTER_HPP
# define SERVERROUTER_HPP

# include "main.hpp"

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
		

	public:
		ServerRouter(std::vector<t_config> configs);
		~ServerRouter();

		// void launch();
		std::vector<t_config>	getConfigs() const;
		// void					getAllSds();
		void					start();

	private:
		// fd_set _getAllActiveSdSets();
		bool	_launch(Server &, int);
		bool	_mainLoop();
		void	_closeSockets();
		bool	_isSocketServer(int);
		void	_saveConnection(int, int,  std::string, unsigned);
		// void	_initInputdata(t_inputdata & data);
		void	_removeSdFromPollfds(int);
		void	_removeConnection(int);
		t_connection	* _getConnection(int clntSd);
		int		_readSd(t_connection *);
		// void	_parseInputData(char *, t_connection *);

};

#endif
