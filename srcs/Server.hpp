#ifndef SERVER_HPP
# define SERVER_HPP

# include "main.hpp"

class Server
{
	private:
		t_config	_config;


	public:
		Server(t_config);
		~Server();
		
		t_config getConfig() const;

		bool	start();
	
	private:

};

#endif
