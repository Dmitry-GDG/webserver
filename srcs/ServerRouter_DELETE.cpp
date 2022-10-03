#include "ServerRouter.hpp"

void ServerRouter::_prepareDeleteAnswer(t_connection * connection)
{
	std::string msg;
	connection->responseData.type = DELETE;
	Server server = _getServer(connection->srvNbr);

}
