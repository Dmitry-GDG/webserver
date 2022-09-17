#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "main.hpp"

class Connection
{
	private:
		int	_sd;
	
	public:
		Connection();
		Connection(int sd);
		~Connection();

		int	getSd() const;

};

#endif
