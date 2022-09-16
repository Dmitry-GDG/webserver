#ifndef SERVERROUTER_HPP
# define SERVERROUTER_HPP

# include "main.hpp"
# include "Server.hpp"

class ServerRouter
{
	private:
		std::vector<t_config>	_configs; //store vector of all server configs
		std::vector<Server>		_servers;
		std::string				_hostname; // il-a3.msk.21-school.ru
		int						_sdMaxCount;
		

	public:
		ServerRouter(std::vector<t_config> configs);
		~ServerRouter();

		void launch();
		std::vector<t_config> getConfigs() const;

	private:
		fd_set _getAllSds();

};

#endif
