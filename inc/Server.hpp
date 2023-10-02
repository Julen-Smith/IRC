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
#include <sstream>
#include <map>
#include <fcntl.h>
#include <fstream>
#include <cstdarg>

#include "defs.hpp"
#include "User.hpp"
#include "Channel.hpp"

class Channel;
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
		std::vector<Channel *>		channels;
		
		//CALLBACK MAP
		typedef void (Server::*MemberFunction)(std::string, int);
		std::map<std::string, MemberFunction> callback_map; 
		std::map<std::string, MemberFunction>::iterator it; 

		int							event_to_handle;
		char						buffer[BUFFER_SIZE];

		void						init_server();
		void						accept_new_user();
		void						enter_msg(int);
		void						manage_response(int);
		void    					erase_client(int);
		int							command_checker(std::string &);
		void						generate_default_channels(void);
		void    					build_message_and_send(std::string,int,std::string, std::string,std::string, int, ...);
		int							check_channel(std::string&);
		std::string&				response_cleaner(std::string&);
		bool						contains(std::string&, const char *);
		void						tokenizer(const char *, int);
		
		//commands
		void						LIST(int);
		void						JOIN(int, int);

	protected:

	private:

		std::string			_port;
		int					_socket;
		struct sockaddr_in 	sv_socket_info;

		void	_init_cout() const;
		void	_create_new_user(ssize_t, int, std::string);
		void	test_create_new_user(std::string, int);
		void	send_intro(ssize_t, int);
};



#endif //SERVER_HPP
