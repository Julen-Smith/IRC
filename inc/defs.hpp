#ifndef DEFS_HPP
# define DEFS_HPP


# define PORT			    "4242"
# define SOCKET_ERROR 		"Error al crear el socket"
# define SOCKET_PREF_ERROR 	"Error al setear las preferencias del socket"
# define MAX_CLIENTS 		50
# define BUFFER_SIZE        1024
# define TIMEOUT	        0
# define MAIN               0
# define PASSWORD           "42Urduliz"

//MAIN ChaNNEL MSG

# define MAIN_CHANNEL       "Server"
# define WELCOME_MSG     "Welcome to the Server Lobby."
# define PRIVMSG          "PRIVMSG"
# define OTHER            "<OTHER>"
# define YOU              "<YOU>"

# define NICK             "NICK "
# define OFF_NICK         5
# define MSG_END          "\r\n"
# define MSG_END_SPACE    "\r\n "
# define PARAMS_PATTERN   " :"
# define SPACE            ' '
# define CSV              " ,"
# define JOIN_MSG         "has joined the channel."
# define COMMAND_LIST     "This is a placeholder to notice that the user must use some command."

// MODES
 
 #define USR_RPL "USR_RPL"
 #define SRV_RPL "SRV_RPL"
 #define NON_NUMERIC "NON_NUMERIC"

// USERS
//
# define DEFAULT_USER "ANON"
# define USER_ACTIVE        true
# define USER_INACTIVE      false
# define BAD_USER_PARAMETER false


//CONFIG FILES
# define BAD_ROOM -1

//ERROR_COMMANDS
# define ERR_NICKNAMEINUSE ":Server 433 "
# define ERR_PASSWDMISMATCH ":Server 464 "
# define ERR_NONICKNAMEGIVEN "431 * "
# define NONICKNAMEGIVEN ":No nickname given\r\n"
# define ERR_ERRONEUSNICKNAME "432 * "
# define ERRONEUSNICKNAME " :Erroneus nickname\r\n"

//RESPONSE COMMANDS
# define RPL_WELCOME ":Server 001 * "

# define WELCOME    " Welcome to the Internet Relay Network "
# define RPL_LIST ":Server 322 "
# define RPL_LISTEND ":Server 323 "
# define LISTEND ":End of LIST\r\n"

# define RPL_TOPIC "332"

# define RPL_YOUREOPER ":Server 381 "

# define RPL_NAMREPLY ":Server 353 "

# define RPL_ENDOFNAMES ":Server 366"
# define ENDOFNAMES " :End of NAMES list"

#endif  //DEFS_HPP
