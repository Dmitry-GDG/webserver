#ifndef SERVER_HPP
# define SERVER_HPP

# include "main.hpp"
// # include "Connection.hpp"

// class Connection;

class Server
{
	private:
		t_config	_config;
		int			_sd; //socket descriptor
		std::string _colors[4];
		unsigned	_nextColor;
		int			_srvrNbr;
		// std::vector<std::string> _methods; // GET, POST, DELETE
		// size_t		_sdMaxCount; // = SD_MAXCOUNT
		// char		_serverIp[sizeof(struct sockaddr_in)];
		// std::vector<Connection>	_connections;

	public:
		Server(t_config);
		~Server();
		Server & operator=(const Server & server);
		
		const t_config	& getConfig() const;
		int		getSd() const;
		void	setSd(int);
		int		getServNbr() const;
		void	setSrvrNbr(size_t);
		// bool		start();
		// void		sdSet(fd_set & allActiveSdSets, int & sdMaxCountRouter);
		// bool		readSd(int sd);
	
	private:
		// std::vector<int>	_getAllSds() const;
		// bool				_foundSd(int sd) const;

};

#endif
