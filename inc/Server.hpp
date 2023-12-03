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
		int	loop;

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
		void						manage_response(int);
		void    					erase_client(int);
		void						generate_default_channels(void);
		int							check_channel(std::string&);
		void						tokenizer(Message& msg);
		bool						read_socket(Message &msg);
		bool    					check_operator(Message &msg);
		void 						broadcast(Message &msg);

		//channel getters
		Channel						*get_channel_by_name(const std::string &name);
		Channel    					*create_channel(User *user, const std::string &romm_name);
		bool 						delete_channel(const std::string &name);

		//user getters
		User						*get_user_by_socket(int client_socket);
		User						*get_user_by_nickname(const std::string &);

		//user modifiers
		bool						delete_user_by_socket(int client_socket);

		//validated user
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
		void						motd_command(Message&);
		void						invite_command(Message&);
		void						prvmsg_command(Message&);
		void						whois_command(Message&);
		void						topic_command(Message&);
		void 						close_command(Message&);

		//flags
		void						flag_manager(Message &msg);

		//unvalidated user methods
		bool						find_unva_user_by_socket(int);
		void						add_unvalidated_user(int);
		bool						delete_unvalidated_user(int);

		//is funcitons

		bool						is_already(const std::string &nickname);

	protected:

	private:

		std::string			_port;
		std::string			_password;
		int					_socket;
		struct addrinfo		_hints;
		struct addrinfo		*_res;

		void			_init_cout() const;
		void			_create_new_user(ssize_t, size_t, std::string);
		void			send_intro(int);
		std::istream&	get_token(Message&, std::string&, char, const std::string &);
};



#endif //SERVER_HPP
