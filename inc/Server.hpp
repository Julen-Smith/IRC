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

typedef struct s_privilege
{
	std::string user;
	std::string password;
	std::string level;
}	t_privilege;

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
		typedef std::vector<User *>::iterator	validated_user;
		
		//CALLBACK MAP
		typedef void (Server::*MemberFunction)(std::stringstream&, int);
		std::map<std::string, MemberFunction> callback_map; 
		std::map<std::string, MemberFunction>::iterator it; 

		typedef	std::map<int, CreateUser *>				unvalidated_user_map;
		typedef	std::map<int, CreateUser *>::iterator	unvalidated_user;
		unvalidated_user_map							unvalidated_users;
		//std::map<int, CreateUser *>::iterator	unvalidate_user_it;

		//privilege structure
		t_privilege					priv_list[2];

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
		bool    					check_operator(const std::string &user, const std::string &password, int client_socket);
		User						*get_user_by_socket(int client_socket);

		//validated user
		bool 						add_unva_user(int client_index);
		bool						add_validated_user(int client_index);
		validated_user				get_user_by_nickname(const std::string &);
		bool 						check_validated_user(Server::validated_user);
		void    					notice_new_user(User *user, int client_index);
		
		//commands
		void						oper_command(std::stringstream&, int);
		void						kick_command(std::stringstream&, int);
		void						list_command(std::stringstream&, int);
		void						join_command(std::stringstream&, int);
		void						nick_command(std::stringstream&, int);
		void						user_command(std::stringstream&, int);

		//unvalidated user methods
		bool						check_unvalidated_user(int);
		void						add_unvalidated_user(int);
		bool						delete_unvalidated_user(int);

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
