#include "ServerRouter.hpp"

void ServerRouter::_findPathToStatusCodePage(t_connection * connection)
{
	Server server = _getServer(connection->srvNbr);
	bool foundPath = false;
	std::string path = "";
	size_t posLast = findLastSlashInAddress(connection->inputData.address);
	// std::cout << RED << "posLast: " << posLast << NC << "\n";
	std::string addressFolder = connection->inputData.address.substr(0, posLast);

	if (server.getConfig().root != "")
		path += server.getConfig().root + "/";
	size_t i;
	for (i = 0; i < server.getConfig().locations.size(); i++)
	{
		// std::cout << RED << addressFolder << "\t" << server.getConfig().locations[i].path << NC << "\n";
		if (addressFolder == server.getConfig().locations[i].path)
		{
			if (server.getConfig().locations[i].root != "")
				path += server.getConfig().locations[i].root + "/";
			if (server.getConfig().locations[i].error_pages.size() > 0)
			{
				for (std::map<std::string, std::string>::const_iterator iter = server.getConfig().locations[i].error_pages.begin(); iter != server.getConfig().locations[i].error_pages.end(); iter++)
				{
					// std::cout << RED << "1(*iter).first == " << (*iter).first << "\t1(*iter).second == " << (*iter).second << NC << "\n";
					if ((*iter).first == connection->responseData.statusCode && (*iter).second != "")
					{
						path += (*iter).second;
						foundPath = true;
					}
				}

			}
		}
	}
	if (!foundPath)
	{
		if (server.getConfig().error_pages.size() > 0)
		{
			for (std::map<std::string, std::string>::const_iterator iter = server.getConfig().error_pages.begin(); iter != server.getConfig().error_pages.end(); iter++)
			{
				// std::cout << RED << "(*iter).first == " << (*iter).first << "\t(*iter).second == " << (*iter).second << NC << "\n";
				if ((*iter).first == connection->responseData.statusCode && (*iter).second != "")
				{
					path += (*iter).second;
					foundPath = true;
				}
			}

		}
	}
	connection->pathToStatusCode = path;
	// #ifdef DEBUGMODE
	// 	std::cout << RED << "path == " << path << NC << "\n";
	// 	std::cout << RED << "connection->inputData.address == " << connection->inputData.address << NC << "\n";
	// 	std::cout << RED << "addressFolder == " << addressFolder << NC << "\n";
	// #endif
}

void ServerRouter::_addStatusCodePage(t_connection * connection, std::string & contentTypeAndLengthAndData)
{
	std::string msg;
	Server server = _getServer(connection->srvNbr);

	_findPathToStatusCodePage(connection);

	struct stat buf;
	std::string contType;
	std::string path = "./" + connection->pathToStatusCode;
	const char * pathChar = path.c_str();

	FILE * fileOpen;
	
	lstat(pathChar, & buf);
	fileOpen = fopen(pathChar, "rb"); //r - read only, b - in binary
	if (fileOpen == NULL || S_ISDIR(buf.st_mode))
	{
		std::string newStatusCodePage = "<!DOCTYPE html><html lang=en><head><meta charset=utf-8><title>Error " + connection->responseData.statusCode + " (" + _getStatusCodeDescription(connection->responseData.statusCode) + ")!!</title><style>*{margin:0;padding:0}html,code{font:15px/22px arial,sans-serif}html{background:#fff;color:#222;padding:15px}body{margin:7% auto 0;max-width:390px;min-height:180px;padding:30px 0 15px}* > body{padding-right:205px}p{margin:11px 0 22px;overflow:hidden}ins{color:#777;text-decoration:none}a img{border:0}@media screen and (max-width:772px){body{background:none;margin-top:0;max-width:none;padding-right:0}}#logo{background:url(//www.google.com/images/branding/googlelogo/1x/googlelogo_color_150x54dp.png) no-repeat;margin-left:-5px}@media only screen and (min-resolution:192dpi){#logo{background:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) no-repeat 0% 0%/100% 100%;-moz-border-image:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) 0}}@media only screen and (-webkit-min-device-pixel-ratio:2){#logo{background:url(//www.google.com/images/branding/googlelogo/2x/googlelogo_color_150x54dp.png) no-repeat;-webkit-background-size:100% 100%}}#logo{display:inline-block;height:54px;width:150px}</style></head><body><p><b><font face=Noteworthy size=6 color=#fa4747>" + connection->responseData.statusCode[0] + "</font><font face=Noteworthy size=6 color=faf147>" + connection->responseData.statusCode[1] + "</font><font face=Noteworthy size=6 color=4afa47>" + connection->responseData.statusCode[2] + "</font>.</b> <ins>That’s an error.</ins>";
		if (connection->responseData.statusCode == "404")
			newStatusCodePage += "<p>The requested URL <code>" + connection->inputData.address + "</code> was not found on this server.";
		else if (connection->responseData.statusCode == "405")
			newStatusCodePage += "<p>The requested method <code>" + connection->inputData.method + "</code> was not allowed on this server.";
		newStatusCodePage += "  <ins>That’s all we know.</ins></body></html>";
		connection->responseData.fileToSendInBinary.clear();
		connection->responseData.fileToSendInBinary = newStatusCodePage;

		contentTypeAndLengthAndData += "Content-Type: text/html; charset=utf-8";
		contentTypeAndLengthAndData += DELIMETER;
		contentTypeAndLengthAndData += "Content-Length: " + std::to_string(newStatusCodePage.size()) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
	}

	else if (S_ISREG(buf.st_mode)) //it's path to file
	{
		fseek(fileOpen, 0L, SEEK_END); // перемотать на конец файла
		size_t fileLength = ftell(fileOpen);

		std::string pathTmp = path;
		pathTmp.erase(pathTmp.begin());
		size_t dot = pathTmp.find('.');
		std::string ext = pathTmp.substr(dot + 1, path.length() - dot);
		std::cout << RED << "ext = " << ext << NC << std::endl;

		if (connection->contentTypesAll.find(ext) != connection->contentTypesAll.end())
			contType = connection->contentTypesAll[ext];
		else
			contType = "text/html";
		fclose(fileOpen);
		readFileToStrInBinary(pathChar, connection->responseData.fileToSendInBinary);
		contentTypeAndLengthAndData += "Content-Type: " + contType + "; charset=utf-8" + DELIMETER + "Content-Length: " + std::to_string(fileLength) + DDELIMETER + connection->responseData.fileToSendInBinary + DDELIMETER;
	}
}
