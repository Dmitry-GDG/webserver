#include "main.hpp"
#include "Client.hpp"

void exitErr(std::string errmsg)
{
	if (errmsg != "Interrupted system call")
	{
		if (errmsg == "std::exception")
			std::cerr << "\r" << REV << "Something is wrong. Please check the arguments and server status, try again later" << std::endl;
		else
			std::cerr << "\r" << REV << errmsg << std::endl;
		exit (EXIT_FAILURE);
	}
	std::cout << "\r" << REV << "Buy!" << std::endl;
	exit (EXIT_SUCCESS);
}

int	main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::string errmsg = "Usage: ./";
		errmsg += NAME;
		errmsg += " <host> <port> [<password> [<nick> [<your full name>]]]";
		exitErr(errmsg);
	}
	int port = atoi(argv[2]);
	if (port < 0 || port > 65535)
		exitErr("port range not valid");
	try
	{
		Client client(argv[1], argv[2]);
		client.start();
	}
	catch (const std::exception &e)
	{
		exitErr(e.what());
	}
	return (0);
}
