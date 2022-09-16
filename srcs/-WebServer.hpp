#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "main.hpp"

class WebServer
{
	private:
		// std::vector<std::string>				_vectorConfigFile; //tmp store input config data in vector
		t_config	_config;
		// u_long									_valueServers;
		// std::string								_listen;
		// std::vector<size_t>						_port;
		// // size_t								_port;
		// std::string								_host;
		// std::map<int, std::vector<std::string> >	_server_name;
		// // std::string							_server_name:
		// bool									_ifdefaultserver;
		// std::map<int, std::string>				_error_pages;
		// size_t									_client_max_body_size;
		// // std::vector<t_location>				_locations;
		// std::map<std::string, std::string>		_cgi;  //расширение - ключ, значение - путь к cgi-файлу
		// std::map<std::string, std::set<std::string> >	_cgi_methods;

	public:
		WebServer(t_config);
		~WebServer();
		
		t_config getConfig() const;

		// void	start();
	
	private:
		// void _parser(std::string config_file_name);

};

#endif
