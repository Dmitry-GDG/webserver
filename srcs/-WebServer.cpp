#include "WebServer.hpp"

WebServer::WebServer(t_config config)
{
	_config = config;
	// _parser(config_file_name);
}

WebServer::~WebServer() {}

t_config WebServer::getConfig() const
	{ return _config; }

// void WebServer::_parser(std::string config_file_name)
// {
//     std::string buf;

//     std::ifstream inpt;
// 	inpt.open(config_file_name);
// 	if (!inpt.is_open())
// 	{
// 		std::cerr << "Error of read entered file" << std::endl;
// 		throw "Parcer cannot be implemented, error of config file";
// 		// return 1;
// 	}
// 	while (getline(inpt, buf))
// 		_vectorConfigFile.push_back(buf);
// 	inpt.close();
// 	for (std::vector<std::string>::iterator iter = _vectorConfigFile.begin(); iter < _vectorConfigFile.end(); iter++)
// 	{
// 		if ((*iter).empty())
//             iter++;
// 	}
// }
