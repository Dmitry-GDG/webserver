#include "ServerRouter.hpp"

std::string ServerRouter::_createAutoindex(t_connection * connection)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);
	std::string pathToServer = "./";

	if (server.getConfig().root != "")
		pathToServer += server.getConfig().root + "/";
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_createAI _createAutoindex pathToServer \npathToServer: " << NC << pathToServer << "\n----------------------" << std::endl;
	#endif
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_createAI _createAutoindex connection->inputData.address \nconnection->inputData.address: " << NC << connection->inputData.address << "\n----------------------" << std::endl;
	#endif
	std::string path = pathToServer + connection->inputData.address;
	// std::string path = connection->inputData.address;
	#ifdef DEBUGMODE
		std::cout << VIOLET << " DEBUGMODE SR_createAI _createAutoindex path \npath: " << NC << path << "\n----------------------" << std::endl;
	#endif
	const char * pathChar = path.c_str();

	std::string htmlStr = "<!DOCTYPE html><html lang=en><meta charset=utf-8><title>Index</title><style>*{margin:0;padding:0}html,code{font:15px/22px arial,sans-serif}html{background:#fff;color:#222;padding:15px}body{margin:7% auto 0;max-width:1000px;min-height:180px;padding:30px 0 15px}* > body{padding-right:205px}p{margin:11px 0 22px;overflow:hidden}ins{color:#777;text-decoration:none}a img{border:0}@media screen and (max-width:772px){body{background:none;margin-top:0;max-width:none;padding-right:0}}#logo{background:url(//www.google.com/images/branding/googlelogo/1x/googlelogo_color_150x54dp.png) no-repeat;margin-left:-5px}@media only screen and (min-resolution:192dpi){#logo{background:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) no-repeat 0% 0%/100% 100%;-moz-border-image:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) 0}}@media only screen and (-webkit-min-device-pixel-ratio:2){#logo{background:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) no-repeat;-webkit-background-size:100% 100%}}#logo{display:inline-block;height:54px;width:150px}</style></head>";
	htmlStr += "<body><p><b><font face=Noteworthy size=6 color=#fa4747>Index of </font>" + path + "</b><br><table>";

	DIR *dir;
    struct dirent *entry;
	struct stat buf;

    dir = opendir(pathChar);
	while ( (entry = readdir(dir)) != NULL) 
	{
		if (strcmp(entry->d_name, "."))
		{
			std::string ppath = path + "/" + entry->d_name; 
			// std::string ppath = "/" + std::to_string(entry->d_name); 
			std::cout << "path: " << path << std::endl;
			std::cout << "entry->d_name: " << entry->d_name << std::endl;
			std::cout << "ppath: " << ppath << std::endl;
			htmlStr += "<tr style=\"height: 18px;\"><td style=\"width: 50.25%; text-align: center; height: 30px;\" bgcolor=\"#e3f5fa\">";
			if (strcmp(entry->d_name, ".."))
				htmlStr += "<a href=\"" + connection->inputData.address + "/" + entry->d_name + "\">";
			else
				htmlStr += "<a href=\"\\\">";
			htmlStr += entry->d_name;
			stat(ppath.c_str(), & buf);
			htmlStr += "</a></td>\n<td style=\"width: 49.75%; height: 30px; text-align: center;\" bgcolor=\"#e3f5fa\">" + getTime(buf.st_mtimespec.tv_sec) + "</td>\n</tr>";
		}
	}

    closedir(dir);
	htmlStr += "<br></table></body></html>";
	return htmlStr;
}
