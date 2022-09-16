#ifndef SERVER_HPP
# define SERVER_HPP

# include "main.hpp"

class Server
{
	private:
		t_config	_config;
		int			_sd; //socket descriptor
		std::string 			_colors[4];
		unsigned				_nextColor;
		// std::string				_hostname; // il-a3.msk.21-school.ru
		int						_sdMaxCount; // = SD_MAXCOUNT
		char					_serverIp[sizeof(struct sockaddr_in)];

	public:
		Server(t_config);
		~Server();
		
		t_config	getConfig() const;
		bool		start();
	
	private:

};

#endif
