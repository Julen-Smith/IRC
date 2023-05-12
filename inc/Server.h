#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

class Server
{
	public:
		Server();
		Server(const char *);

		~Server();

		const int	max_clients;
	protected:

	private:

		std::string	_port;
		int		_socket;
		struct addrinfo	_hint;
		struct addrinfo *_serv_info;

		void	_init_cout();
};


#endif //SERVER_HPP
