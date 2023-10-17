#ifndef DEFS_HPP
# define DEFS_HPP

//User

# define CONNECTED      true
# define DISCONNECTED   false
# define MAX_CHANNELS   2
# define EMPTY          0

//Channel

# define FOUND              true
# define NOT_FOUND          false

# define LIMIT_RAISED       true
# define LIMIT_NOT_RAISED   false

# define USER_ALREADY_IN        true
# define USER_NOT_ALREADY_IN    false

# define INCORRECT_KEY  true
# define CORRECT_KEY    false

# define KEY_SET        true
# define KEY_NOT_SET    false
# define NO_KEY         0

# define STANDARD_LIMIT 10

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
# define PARAMS_PATTERN   " "
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
# define INVITE_ONLY true

//ERROR_COMMANDS
# define ERR_NICKNAMEINUSE ":Server 433 * "

# define ERR_PASSWDMISMATCH ":Server 464 * "
# define PASSWDMISMATCH ":Password incorrect\r\n"

# define ERR_NONICKNAMEGIVEN "431 * "
# define NONICKNAMEGIVEN ":No nickname given\r\n"

# define ERR_ERRONEUSNICKNAME "432 * "
# define ERRONEUSNICKNAME " :Erroneus nickname\r\n"

# define ERR_NEEDMOREPARAMS "461 * "
# define NEEDMOREPARAMS ":Not enough parameters\r\n"

# define ERR_INVITEONLYCHAN "473 "
# define INVITEONLYCHAN " :Cannot join channel (+i)\r\n"

# define ERR_CHANNELISFULL "471 "
# define CHANNELISFULL " :Cannot join channel (+l)\r\n"

# define ERR_BANNEDFROMCHAN "474 "
# define BANNEDFROMCHAN " :Cannot join channel (+b)\r\n"

# define ERR_NOSUCHCHANNEL "403 "
# define NOSUCHCHANNEL " :No such channel"

# define ERR_BADCHANNELKEY "475 "
# define BADCHANNELKEY " :Cannot join channel (+k)\r\n"

# define ERR_TOOMANYCHANNELS "405 "
# define TOOMANYCHANNELS " :You have joined too many channels\r\n"

//RESPONSE COMMANDS
# define RPL_LISTSTART ":Server 321 "
# define LISTSTART "Channel :Users  Name"

# define RPL_LIST ":Server 322 * "

# define RPL_LISTEND ":Server 323 * "
# define LISTEND ":End of LIST\r\n"

# define RPL_WELCOME ":Server 001 "

# define WELCOME    " Welcome to the Internet Relay Network "

# define RPL_TOPIC ":Server 332 "

# define RPL_YOUREOPER ":Server 381 "
# define YOUREOPER ":You are now an IRC operator\r\n"

# define RPL_NAMREPLY ":Server 353 "

# define RPL_ENDOFNAMES ":Server 366 "
# define ENDOFNAMES " :End of NAMES list\r\n"

#endif  //DEFS_HPP
