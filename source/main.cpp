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


/*
static void loop_client(Server &server)
{
}
*/

/** @attention Main loop del servidor @param none @return */
void main_loop(Server &server)
{
    //signal(SIGPIPE, SIG_IGN);

    server.users.push_back(new User("Bot", server.get_socket()));
    server.fds.push_back(pollfd());
    server.fds[0].fd = server.get_socket();
    server.fds[0].events = POLLIN;

    server.users.push_back(new User("nickname", 8));
    server.fds.push_back(pollfd());
    server.fds[1].fd = 8;
    server.fds[1].events = POLLIN;

    server.generate_default_channels();
    while (true)
    {
        server.event_to_handle = poll(server.fds.data(), server.fds.size(), 0);
        for(int client = 0; client < server.fds.size(); client++)
        {
            if (server.fds[client].revents & POLLHUP)  //Erase client
            {
                server.erase_client(server.fds[client].fd);
                continue ;
            }
            if (server.fds[client].revents & POLLIN)  //Comprobación cambios fichero
            {
                if (server.fds[client].fd == server.get_socket())
                    server.accept_new_user();
                else
                    server.manage_response(client);     
            }
        }
    }
}


/** @attention Main @param Argc,Port,Password @return exit mode */
int main(int argc, char const *argv[])
{
    /*
    if (argc == 1)
        Server	server(PORT, PASSWORD);
    else
        Server	server(PORT, argv[1]);
        */
    Server	server(PORT);

    main_loop(server);
    return (0);
}

