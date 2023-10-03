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
		typedef void (Server::*MemberFunction)(std::stringstream&, int);
		std::map<std::string, MemberFunction> callback_map; 
		std::map<std::string, MemberFunction>::iterator it; 

		int							event_to_handle;
		char						buffer[BUFFER_SIZE];

		void						accept_new_user();
		void						enter_msg(int);
		void						manage_response(int);
		void    					erase_client(int);
		int							command_checker(std::string &);
		void						generate_default_channels(void);
		void    					build_message_and_send(std::string,int,std::string, std::string,std::string, int, ...);
		int							check_channel(std::string&);
		void						tokenizer(int, const char *);
		bool						read_socket(int,  char [BUFFER_SIZE]);
		void						erase_match(std::string &source, const std::string &to_erase);
		
		//commands
		void						list_command(std::stringstream&, int);
		void						join_command(std::stringstream&, int);
		void						nick_command(std::stringstream&, int);
		void						user_command(std::stringstream&, int);

	protected:

	private:

		std::string			_port;
		int					_socket;
		struct sockaddr_in 	sv_socket_info;

		void	_init_cout() const;
		void	_create_new_user(ssize_t, int, std::string);
		void	send_intro(int);
		std::istream&	get_token(std::stringstream&, std::string&, char, const std::string &);
};



#endif //SERVER_HPP
