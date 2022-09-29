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

enum ConnectionStatus
{
	READ,
	READ_DONE,
	WRITE,
	WRITE_DONE
};

enum InputDataType
{
	HTTP,
	DATA_START,
	DATA_CONTIN
};

enum ResponseType
{
	NOT_DEFINED,
	GET,
	POST,
	DELETE,
	REDIRECT,
	CGI_NET,
	CGI_LOC
};

typedef struct s_inputdata
{
	std::string		method; // GET, POST, DELETE
	// RequestType		requestType;
	std::string		address;
	std::string		httpVersion;
	InputDataType	dataType; // HTTP, DATA_START, DATA_CONTIN
	std::map<std::string, std::string>	htmlFields;
	std::string		inputStr;
} t_inputdata;

typedef struct s_response
{
	std::string		responseStatusCode;
	ResponseType	method; // NOT_DEFINED, GET, POST, DELETE, REDIRECT, CGI_NET, CGI_LOC

} t_response;

typedef struct s_connection
{
	int					srvNbr;
	std::string			fromIp;
	unsigned			fromPort;
	int					clntSd;
	size_t				position;
	pollfd				* pfd;
	long int			lastActivityTime;
	ConnectionStatus	status;
	// std::string			status; // 	READ, READ_DONE, WRITE, WRITE_DONE
	std::vector<std::string> allowedMethods; // GET, POST, DELETE
	std::map<std::string, std::string> responseStatusCodesAll;
	std::map<std::string, std::string>	contentTypesAll;
	t_inputdata			inputData;
	t_response			responseData;
} t_connection;

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
	std::map<std::string, std::string>	error_pages;
} t_location;

typedef struct s_config
{
	std::string					serverName;
	std::string					listen;
	std::string					ip;
	std::string					port;
	std::string					limitSize;
	std::string					root;
	std::string					index;
	std::string					upload;
	std::string					autoindex;
	std::string					redirs;
	std::vector<std::string>	methods;
	std::vector<t_location>		locations;
	std::map<std::string, std::string>	error_pages;
	std::map<std::string, std::string>	cgi;
} t_config;

#endif
