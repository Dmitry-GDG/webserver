#ifndef SERVER_HPP
# define SERVER_HPP

# include "main.hpp"
# include "Connection.hpp"

class Connection;

class Server
{
	private:
		t_config	_config;
		int			_sd; //socket descriptor
		std::string _colors[4];
		unsigned	_nextColor;
		int			_sdMaxCount; // = SD_MAXCOUNT
		char		_serverIp[sizeof(struct sockaddr_in)];
		std::vector<Connection>	* _connections;

	public:
		Server(t_config, std::vector<Connection> *);
		~Server();
		
		t_config	getConfig() const;
		bool		start();
		void		sdSet(fd_set & allActiveSdSets, int & sdMaxCountRouter, std::vector<int> allSds);
	
	private:

};

#endif
