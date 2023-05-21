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
    server.users.push_back(new User("Bot"));
    server.users[0]->set_pollfd(server.get_socket(), POLLIN);

    while (true)
    {

        server.event_to_handle = poll(server.users[0], server.users.size(), 0);
        for(int client = 0; client < server.users.size(); client++)
        {
            if (server.users[client]->revents & POLLIN)  //Comprobación cambios fichero
            {
                if (server.users[client]->fd == server.get_socket())
                    server.accept_new_user();
                else
                {
                    if (server.users[client]->get_notices() == false)
                    {
                        server.enter_msg(client);
                    }
                    else
                        server.send_msg(client);
                }
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

