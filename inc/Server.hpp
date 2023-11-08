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
#include <ctime>

#include "defs.hpp"
#include "User.hpp"
#include "Channel.hpp"
#include "Message.hpp"

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
		Server(const char *, const char *);

		~Server();

		Message		message;

		const int	max_clients;
		const int	get_socket() const;

		std::vector<pollfd>			fds;
		std::vector<User *>			users;
		std::vector<Channel *>		channels;
		typedef std::vector<User *>::iterator	validated_user;
		
		//CALLBACK MAP
		typedef void (Server::*MemberFunction)(Message&);
		std::map<std::string, MemberFunction> callback_map; 
		std::map<std::string, MemberFunction>::iterator it; 

		typedef	std::map<int, UnvalidatedUser *>				unvalidated_user_map;
		typedef	std::map<int, UnvalidatedUser *>::iterator	unvalidated_user;
		unvalidated_user_map							unvalidated_users;
		//std::map<int, UnvalidatedUser *>::iterator	unvalidate_user_it;

		//privilege structure
		t_privilege					priv_list[2];

		int							event_to_handle;

		void						accept_new_user();
		void						enter_msg(int);
		void						manage_response(int);
		void    					erase_client(int);
		int							command_checker(std::string &);
		void						generate_default_channels(void);
		void    					build_message_and_send(std::string,int,std::string, std::string,std::string, int, ...);
		int							check_channel(std::string&);
		void						tokenizer(Message& msg);
		bool						read_socket(Message &msg);
		bool    					check_operator(Message &msg);
		void						check_users();
		void						check_inac();

		//channel getters
		Channel						*get_channel_by_name(const std::string &name) const;
		Channel    					*create_channel(User *user, const std::string &romm_name);
		bool 						delete_channel(const std::string &name);

		//user getters
		User						*get_user_by_socket(int client_socket);
		std::string					get_nickname_by_socket(int client_socket);
		User						*get_user_by_nickname(const std::string &);

		//user modifiers
		bool						delete_user_by_socket(int client_socket);

		//validated user
		bool 						add_unva_user(int client_index);
		User						*add_validated_user(unvalidated_user);
		bool 						check_validated_user(Server::validated_user);
		bool						check_name(const std::string &name);
		void    					notice_new_user(Message &msg);
		
		//commands
		void						pass_command(Message&);
		void						part_command(Message&);
		void						quit_command(Message&);
		void						mode_command(Message&);
		void						oper_command(Message&);
		void						kick_command(Message&);
		void						list_command(Message&);
		void						join_command(Message&);
		void						nick_command(Message&);
		void						user_command(Message&);
		void						ping_command(Message&);
		void						prvmsg_command(Message&);

		//flags
		void						flag_manager(Message &msg);

		//commands error

		void						mode_error_manager(int);

		//unvalidated user methods
		bool						find_unva_user_by_socket(int);
		void						add_unvalidated_user(int);
		bool						delete_unvalidated_user(int);

		//setter
		void						set_curr_time(time_t curr_time);

		//getter
		time_t						get_curr_time() const;

	protected:

	private:

		std::string			_port;
		std::string			_password;
		int					_socket;
		struct sockaddr_in 	sv_socket_info;
		time_t				_curr_time;

		void			_init_cout() const;
		void			_create_new_user(ssize_t, int, std::string);
		void			send_intro(int);
		std::istream&	get_token(Message&, std::string&, char, const std::string &);
};



#endif //SERVER_HPP
