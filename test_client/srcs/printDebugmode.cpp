#include "main.hpp"
#include "Client.hpp"

void printCommandParsed(Client * client, std::string msg)
{
	if (msg != "")
		std::cout << VIOLET << " " << msg << " " << NC << std::endl;
	else
		std::cout << VIOLET << " printCommandParsed " << NC << std::endl;
	std::cout << "_command: " << client->getCommand() << std::endl;
	std::vector<std::string> commandsArg = client->getCommandArgs();
	for (size_t i = 0; i < commandsArg.size(); i++)
		std::cout << "arg[" << i << "]: " << commandsArg[i] << std::endl;
	std::cout << "----------------------" << std::endl;
}
