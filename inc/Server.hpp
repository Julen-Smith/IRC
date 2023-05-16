#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h> 
#include <cstring>
#include <exception>
#include <arpa/inet.h>


class Server
{
	public:
		Server();
		Server(const char *);
		~Server();
		const int	max_clients;
		const int	get_socket() const;

	protected:

	private:

		std::string	_port;
		int		_socket;
		struct addrinfo	sv_address_info;
		struct addrinfo *_serv_info;

		struct sockaddr_in sv_socket_info;
		//sockaddr_in es necesaria para funciones como bind, connect
		// y accept

		void	_init_cout();
};


#endif //SERVER_HPP
