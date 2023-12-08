#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>

#include "Server.hpp"
#include "User.hpp"
#include "defs.hpp"
#include <stdlib.h>


/** @attention Main loop del servidor @param none @return */
void main_loop(Server &server)
{
    server.users.push_back(new User("Bot", server.get_socket()));
    server.fds.push_back(pollfd());
    server.fds[0].fd = server.get_socket();
    server.fds[0].events = POLLIN;
    server.fds[0].revents = 1;

    while (server.loop)
    {
        server.event_to_handle = poll(server.fds.data(), server.fds.size(), 0);
        for(size_t client = 0; client < server.fds.size(); client++)
        {
            if (server.fds[client].revents & POLLHUP)  {//Erase client
                server.delete_user_by_socket(server.fds[client].fd);
            }
            
            else if (server.fds[client].revents & POLLIN)  //Comprobación cambios fichero
            {
                if (server.fds[client].fd == server.get_socket())
                    server.accept_new_user();
                else
                    server.manage_response(client);
            }
        }
    }
}

static int validator(char const **argv) {

    int     retval;
    size_t  n = strlen(argv[PORT]);
    size_t  m = strlen(argv[PASSWORD]);

    //Longitud maxima de contrasena 10 caracteres
    if (m > 10) {
        std::cerr << "Error: password too long\r\n";
        return VALIDATOR_ERROR;
    }

    //Comprobar que el pueerto solo tenga digitos/numeros.
    for (size_t i = 0; i < n; i++) {
        if (!isdigit(argv[PORT][i])) {
            std::cerr << "Error: Invalid character for port\r\n";
            return VALIDATOR_ERROR;
        }
    }

    retval = atoi(argv[PORT]);

    //Comprobar que el puerto no sea mayor de 65535
    if (retval > 65535) {
        std::cerr << "Error: bad port\r\n";
        return VALIDATOR_ERROR;
    }
    return VALIDATOR_OK;
}

/** @attention Main @param Argc,Port,Password @return exit mode */
int main(int argc, char const *argv[])
{
    if (argc != 3) {
        std::cerr << "Error: Invalid number of arguments. Required port and password" << std::endl;
        return 2;
    }

    if (validator(argv))
        return 1;
    Server	server(argv[PORT], argv[PASSWORD]);

    main_loop(server);
    return (0);
}
