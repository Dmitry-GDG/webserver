#ifndef STRUCTS_HPP
# define STRUCTS_HPP

# include "main.hpp"

enum RequestProcessingStep
{
	NOT_DEFINED_REQUEST_PROCESSING_STEP,
	READING_HEADER,
	READING_BODY,
	READING_DONE,
	WRITING,
	WRITING_DONE
};

enum InputDataType
{
	NOT_DEFINED_INPUT_DATA_TYPE,
	HTTP
};

enum ResponseType
{
	NOT_DEFINED_RESPONSE_DATA_TYPE,
	GET,
	POST,
	DELETE,
	REDIRECT,
	CGI_NET,
	CGI_LOC
};

enum PostContentType
{
	NOT_DEFINED_POST_CONTENT_TYPE,
	URLENCODED,
	FORM_DATA,
	MIXED
};

typedef struct s_inputdata
{
	std::string		method; // GET, POST, DELETE
	std::string		address;
	std::string		addressParamsStr; // POST request's params in line, separator between params '&', separator between key and value '='
	std::string		httpVersion;
	InputDataType	dataType; // HTTP
	PostContentType	postContentType;
	std::vector<std::string> boundary;
	std::string		postFileName;
	std::string		postFileData;
	std::string		postFilePathToSave;
	std::vector<std::pair<std::string, std::string> >	headerFieldsVec; //map - notacceptable, some keys repeated witn other values
	std::vector<std::string>							inputStrBodyVec;
	std::vector<std::pair<std::string, std::string> >	bodyFieldsVec;
} t_inputdata;

typedef struct s_response
{
	std::string		statusCode;
	ResponseType	type; // NOT_DEFINED_RESPONSE_DATA_TYPE, GET, POST, DELETE, REDIRECT, CGI_NET, CGI_LOC
	std::string		connectionAnswer;
	std::string		fileToSendInBinary;
	ssize_t			lenAnswer;
	ssize_t 		lenSent;

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
	std::string			inputStr; // the whole request
	std::string			inputStrHeader;
	std::string			inputStrBody;
	size_t				lenGet; // qty bytes recieved
	unsigned long		сontentLength; // = Content-Length
	RequestProcessingStep	requestProcessingStep;
	std::vector<std::string> allowedMethods; // GET, POST, DELETE
	std::map<std::string, std::string> responseStatusCodesAll;
	std::map<std::string, std::string>	contentTypesAll;
	t_inputdata			inputData;
	t_response			responseData;
	std::string			referer;
	std::string			pathToStatusCode;
} t_connection;

typedef struct s_location
{
	std::string					path;
	std::string					root;
	std::string					index;
	bool						autoindex;
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
	unsigned long				limitClientBodySize; // Это полное тело, с учетом всех request-ов. "stoul"
	std::string					root;
	std::string					index;
	std::string					upload;
	bool						autoindex;
	std::string					redirs;
	std::vector<std::string>	methods;
	std::vector<t_location>		locations;
	std::map<std::string, std::string>	error_pages;
	std::map<std::string, std::string>	cgi;
} t_config;

#endif
