#ifndef SERVER_HPP
# define SERVER_HPP

# include "main.hpp"

class Server
{
	private:
		t_config	_config;
		int			_sd; //socket descriptor
		std::string _colors[4];
		unsigned	_nextColor;
		int			_srvrNbr;
		unsigned	_port;

	public:
		Server(t_config);
		~Server();
		Server & operator=(const Server & server);
		
		const t_config	& getConfig() const;
		void	setConfig(t_config);
		int		getSd() const;
		void	setSd(int);
		int		getServNbr() const;
		void	setSrvrNbr(size_t);
		unsigned	getPort() const;
		void	setPort(unsigned);
	
};

#endif
