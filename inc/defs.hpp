#ifndef DEFS_HPP
# define DEFS_HPP

//MAIN

# define PORT 1
# define PASSWORD 2
# define VALIDATOR_ERROR true
# define VALIDATOR_OK false

//User

# define CONNECTED      true
# define DISCONNECTED   false
# define MAX_CHANNELS   2
# define EMPTY          0

//Channel

//FLAGS

# define TOPIC      0
# define KEY        1
# define LIMIT      2
# define BAN        3
# define MODERATED  4
# define INVITE     5

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

//Server

//# define PORT			    "4242"
# define SOCKET_ERROR 		"Error al crear el socket"
# define SOCKET_PREF_ERROR 	"Error al setear las preferencias del socket"
# define MAX_CLIENTS 		50
# define BUFFER_SIZE        1024
# define TIMEOUT	        0
# define MAIN               0
//# define PASSWORD           "42Urduliz"

//MAIN ChaNNEL MSG

# define MAIN_CHANNEL     "Server"
# define WELCOME_MSG      "Welcome to the Server Lobby."
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

#define OPER_PASSWORD "oper"
#define TEST_USER_PASSWORD "."

//CONFIG FILES
# define BAD_ROOM -1
# define INVITE_ONLY true

//ERROR_COMMANDS
# define ERR_CUSTOM ":juluk.org 999 * "
# define INVISIBLE ":You are invisible now\r\n"
# define INVISIBLE_ALR ":You are invisible alredy\r\n"
# define VISIBLE ":You are visible now\r\n"
# define VISIBLE_ALR ":You are visible alredy\r\n"

# define ERR_NICKNAMEINUSE ":juluk.org 433 "
# define NICKNAMEINUSE ":Nickname is already in use\r\n"

# define ERR_PASSWDMISMATCH ":juluk.org 464 * "
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

# define ERR_NOSUCHCHANNEL "403 * "
# define NOSUCHCHANNEL " :No such channel.\r\n"

# define ERR_BADCHANNELKEY "475 "
# define BADCHANNELKEY " :Cannot join channel (+k)\r\n"

# define ERR_TOOMANYCHANNELS "405 "
# define TOOMANYCHANNELS " :You have joined too many channels\r\n"

# define ERR_ALREADYREGISTRED "462 "
# define ALREADYREGISTRED ":You may not reregister\r\n"

//OPER
# define RPL_YOUREOPER "381 * "
# define YOUREOPER " :You are now an IRC operator\r\n"

//TOPIC
# define RPL_NOTOPIC  "331 * "
# define NOTOPIC " :No topic is set\r\n"


//MODE RESPONSES

# define MODECHANNELMODS "+-tklbm"
# define MODEUSERMODS "+-iwsovq"

# define RPL_CHANNELMODEIS "324 * "
# define CHANNELMODEIS " :<channel> <mode> <mode params>\r\n"

# define ERR_CHANOPRIVSNEEDED ":juluk.org 482 * "
# define CHANOPRIVSNEEDED " :You're not channel operator.\r\n"

# define ERR_NOSUCHNICK "401 * "
# define NOSUCHNICK " :No such nick/channel.\r\n"

# define ERR_NOTONCHANNEL "442 * "
# define NOTONCHANNEL " :You're not on that channel.\r\n"

# define ERR_KEYSET "467 * "
# define KEYSET " :Channel key already set.\r\n"

# define RPL_BANLIST "367 * "
# define BANLIST " :<channel> <banmask>\r\n"

# define RPL_ENDOFBANLIST "368 * "
# define ENDOFBANLIST " :End of channel ban list.\r\n"

# define ERR_UNKNOWNMODE "472 * "
# define UNKNOWNMODE " :is unknown mode char to me.\r\n"

# define ERR_USERSDONTMATCH "502 * "
# define USERSDONTMATCH " :Can't change mode for other users.\r\n"

# define RPL_UMODEIS "221 * "
# define UMODEIS " :<user mode string>\r\n"

# define ERR_UMODEUNKNOWNFLAG "501 * "
# define UMODEUNKNOWNFLAG " :Unknown MODE flag.\r\n"

// Invite
# define ERR_NOTONCHANNEL "442 * "
# define ERR_USERONCHANNEL "443 * "
# define USERONCHANNEL " :User is alredy on this channel.\r\n"


//RESPONSE COMMANDS
# define RPL_LISTSTART ":juluk.org 321 "
# define LISTSTART "Channel :Users  Name"

# define RPL_LIST ":juluk.org 322 * "

# define RPL_LISTEND ":juluk.org 323 * "
# define LISTEND ":End of LIST\r\n"

# define RPL_WELCOME ":juluk.org 001 "

# define WELCOME    " Welcome to the Internet Relay Network "

# define RPL_TOPIC ":juluk.org 332 "

# define RPL_NAMREPLY ":juluk.org 353 "

# define RPL_ENDOFNAMES ":juluk.org 366 "
# define ENDOFNAMES " :End of /NAMES list\r\n"

//WHOIS
# define RPL_WHOISUSER ":juluk.org 311 "

# define RPL_WHOISSERVER ":juluk.org 312 "

# define RPL_WHOISCHANNELS ":juluk.org 319 "

# define RPL_ENDOFWHOIS ":juluk.org 318 "

//MOTD
# define RPL_MOTDSTART ":juluk.org 375 "
# define MOTDSTART ":- juluk.org Message of the day - \r\n"

# define RPL_MOTD ":juluk.org 372 "
# define MOTD ":- "

# define RPL_ENDOFMOTD ":juluk.org 376 "
# define ENDOFMOTD ":End of MOTD command\r\n"

# define ERR_NOMOTD ":juluk.org 422 "
# define NOMOTD ":MOTD File is missing\r\n"

#endif  //DEFS_HPP
