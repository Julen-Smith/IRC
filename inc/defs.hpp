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
# define JOIN_MSG         "has joined the channel."
# define COMMAND_LIST     "This is a placeholder to notice that the user must use some command."

// MODES
 
 #define USR_RPL "USR_RPL"
 #define SRV_RPL "SRV_RPL"
 #define NON_NUMERIC "NON_NUMERIC"

//CONFIG FILES


#endif  //DEFS_HPP
