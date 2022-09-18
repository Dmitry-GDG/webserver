#include "Connection.hpp"

Connection::Connection() : _sd(-1) {}

Connection::Connection(int sd) : _sd(sd) {}

Connection::~Connection() {}

int	Connection::getSd() const
	{ return _sd; }
