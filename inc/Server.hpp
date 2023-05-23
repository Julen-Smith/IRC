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
#include <poll.h>
#include <vector>
#include <sys/socket.h>

#include "defs.hpp"
#include "User.hpp"

class Server
{
	public:
		Server();
		Server(const char *);

		~Server();

		const int	max_clients;
		const int	get_socket() const;

		std::vector<pollfd>			fds;
		std::vector<User *>			users;
		int							event_to_handle;
		char						buffer[BUFFER_SIZE];

		void						init_server();
		void						accept_new_user();
		void						enter_msg(int);
		void						send_msg(int);
		void    					erase_client(int);

	protected:

	private:

		std::string			_port;
		int					_socket;
		struct sockaddr_in 	sv_socket_info;

		void	_init_cout() const;
		void	_create_new_user(ssize_t);
};


#endif //SERVER_HPP
