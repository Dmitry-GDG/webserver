#ifndef STRUCTS_HPP
# define STRUCTS_HPP

# include "main.hpp"

// typedef struct {
//     u_char    *name;
//     uint32_t   method;
// } ngx_http_method_name_t;

// static ngx_http_method_name_t  ngx_methods_names[] = {
//     { (u_char *) "GET",       (uint32_t) ~NGX_HTTP_GET },
//     { (u_char *) "HEAD",      (uint32_t) ~NGX_HTTP_HEAD },
//     { (u_char *) "POST",      (uint32_t) ~NGX_HTTP_POST },
//     { (u_char *) "PUT",       (uint32_t) ~NGX_HTTP_PUT },
//     { (u_char *) "DELETE",    (uint32_t) ~NGX_HTTP_DELETE },
//     { (u_char *) "MKCOL",     (uint32_t) ~NGX_HTTP_MKCOL },
//     { (u_char *) "COPY",      (uint32_t) ~NGX_HTTP_COPY },
//     { (u_char *) "MOVE",      (uint32_t) ~NGX_HTTP_MOVE },
//     { (u_char *) "OPTIONS",   (uint32_t) ~NGX_HTTP_OPTIONS },
//     { (u_char *) "PROPFIND",  (uint32_t) ~NGX_HTTP_PROPFIND },
//     { (u_char *) "PROPPATCH", (uint32_t) ~NGX_HTTP_PROPPATCH },
//     { (u_char *) "LOCK",      (uint32_t) ~NGX_HTTP_LOCK },
//     { (u_char *) "UNLOCK",    (uint32_t) ~NGX_HTTP_UNLOCK },
//     { (u_char *) "PATCH",     (uint32_t) ~NGX_HTTP_PATCH },
//     { NULL, 0 }
// };


// typedef struct s_redir
// {
//   int			code;
//   std::string	target;

//   s_redir(void) : code(0) {}
// } t_redir;

// typedef struct s_location
// {
//   std::set<std::string>	methods;
//   t_redir				redir;
//   std::string			path;
// //   std::string			root;
// //   std::string			index;
// //   size_t				client_max_body_size;
//   std::string			uploads_path;
//   bool					autoindex;
//   bool					file_uploads;

//   s_location(void)
//       : client_max_body_size(0), autoindex(false), file_uploads(false) {}
// } t_location;


// // Перенёс эту структуру в файл WebServer.hpp
// typedef struct s_server
// {
//   u_long                                        valueServers;
//   std::string					                          listen;
//   std::vector<size_t>						                port;
//   // size_t                                        port;
//   std::string					                          host;
//   std::map<int, std::vector<std::string>>				server_name;
//   // std::string                                   server_name:
//   bool							                            ifdefaultserver;
//   std::map<int, std::string>	                  error_pages;
//   size_t						                            client_max_body_size;
//   // std::vector<t_location>		                    locations;
//   std::map<std::string, std::string>			      cgi;  //расширение - ключ, значение - путь к cgi-файлу
//   std::map<std::string, std::set<std::string> >	cgi_methods;
// } t_server;





typedef struct s_location
{
	std::string					path;
	std::string					root;
	std::string					index;
	std::string					autoindex;
	std::string					upload;
	std::string					limit_size;
	std::string					redirs;
	std::vector<std::string>	methods;
	std::map<std::string, std::string>	cgi;
	std::map<std::string, std::string>	error_page;
} t_location;

typedef struct s_config
{
	std::string					serverName;
	std::string					listen;
	std::string					ip;
	std::string					port;
	std::string					limitSize;
	std::string					root;
	std::string					autoindex;
	std::string					redirs;
	std::vector<std::string>	methods;
	std::vector<t_location>		locations;
	std::map<std::string, std::string>	error_page;
	std::map<std::string, std::string>	cgi;
} t_config;


#endif
