#ifndef WEBSERVERCONFIG_HPP
# define WEBSERVERCONFIG_HPP

# include "main.hpp"

class WebserverConfig {
	public:
		WebserverConfig(void);
		WebserverConfig(WebserverConfig const &src);
		virtual ~WebserverConfig(void);

		WebserverConfig						&operator=(WebserverConfig const &src);
		int									parseServer(unsigned int &i, std::vector<std::string> &file);
		void								passMembers(WebserverConfig &server) const;

		class	ExceptionInvalidArguments: public std::exception {
			virtual const char	*what() const throw();
		};

		static WebserverConfig					_initDefaultServer(const char *filename);

		std::vector<t_listen>				getListen() const;
		std::string							getRoot() const;
		std::vector<std::string>   			getServerName() const;
		std::map<int, std::string>			getErrorPage() const;
		int									getClientBodyBufferSize() const;
		std::map<std::string, std::string>	getCgiParam() const;
		std::string							getCgiPass() const;
		std::map<std::string, WebserverConfig> getLocation() const;
		std::set<std::string>				getAllowedMethods() const;
		std::vector<std::string>			getIndex() const;
		bool								getAutoIndex() const;
		std::string							getAlias() const;
		bool								getAliasSet() const;
		static WebserverConfig					&getDefaultServer();

		// RETURN CONFIG ACCORDING TO URI
		WebserverConfig						getLocationForRequest(std::string const path, std::string &locationPath);

		friend	std::ostream &operator<<(std::ostream &out, const WebserverConfig &server);

	private:
		int				parseLocation(unsigned int &i, std::vector<std::string> &file);
		void    							addListen(std::vector<std::string> args);
		void    							addRoot(std::vector<std::string> args);
		void    							addServerName(std::vector<std::string> args);
		void    							addErrorPage(std::vector<std::string> args);
		void    							addClientBodyBufferSize(std::vector<std::string> args);
		void								addCgiParam(std::vector<std::string> args);
		void    							addCgiPass(std::vector<std::string> args);
		void								addAllowedMethods(std::vector<std::string> args);
		void								addIndex(std::vector<std::string> args);
		void								addAutoIndex(std::vector<std::string> args);
		void								addAlias(std::vector<std::string> args);

		// std::vector<t_listen>				_listen;
		std::string					_listen;
		size_t						_port;
		std::string							_root;
		std::vector<std::string>   			_server_name;
		std::map<int, std::string>			_error_page; // error page redirections
		int									_client_body_buffer_size; // max size for the client body, defaults to 8 000
		std::map<std::string, std::string>	_cgi_param;
		std::string							_cgi_pass;
		std::map<std::string, WebserverConfig>_location;
		std::set<std::string>				_allowed_methods;
		std::vector<std::string>			_index;
		bool								_autoindex;
		std::string							_alias;
		bool								_aliasSet;
		static  WebserverConfig				_defaultServer;
		static	parseMap					serverParsingMap;
		static	parseMap					locationParsingMap;
		static parseMap 					_initServerMap();
		static parseMap 					_initLocationMap();

};

#endif
