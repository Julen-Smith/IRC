#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>

#include "../inc/Server.hpp"
#include "../inc/User.hpp"
#include "defs.hpp"

# define TIMEOUT	0
# define PASSWORD   "42Urduliz"


/** @attention Main loop del servidor @param none @return */
void main_loop(Server &server)
{

    char buffer[BUFFER_SIZE];
    std::vector<pollfd> fds;
    std::vector<bool> notices;
    std::vector<std::string> message_list;
    std::string password  = "42Urduliz";

    fds.push_back(pollfd());
    fds[0].fd = server.get_socket();
    fds[0].events = POLLIN;
    while (true)
    {
        int event_to_handle = poll(&fds[0], fds.size(), 0);
        for(int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)  //Comprobación cambios fichero
            {
                if (fds[i].fd == server.get_socket())
                {
                    sockaddr_in clientes;
                    socklen_t client_address_size = sizeof(clientes);
                    int client_fd = accept(server.get_socket(), (sockaddr *)&clientes, &client_address_size);
                    fds.push_back(pollfd());
                    fds.back().fd = client_fd;
                    fds.back().events = POLLIN;
                    notices.push_back(false);
                }
                else
                {
                    if (notices[i] == false)
                    {
                        notices[i] = true;
                        memset(buffer, 0, sizeof(buffer));
                        recv(fds[i].fd, buffer, BUFFER_SIZE,0);
                        std::cout << buffer << std::endl;
                        //Mensaje de bienvenida al servidor
                        std::string channel = "Server";
                        std::string saludo = "Welcome to the Server Lobby.";
                        std::string ircMessage = "PRIVMSG " + channel + " :" + saludo + "\r\n";

                        send(fds[i].fd,ircMessage.c_str(),ircMessage.size(),0);
                    }
                    else
                    {
                        memset(buffer, 0, sizeof(buffer));
                        recv(fds[i].fd, buffer, BUFFER_SIZE,0);
                        for(int u = 0; u != fds.size(); u++)
                        {
                            if (u != i)
                            {
                                std::string channel = "Server";
                                std::string saludo(buffer);
                                std::string ircMessage = "PRIVMSG " + channel + " :" + "<OTHER> "+ saludo + "\r\n";
                                send(fds[u].fd,ircMessage.c_str(),ircMessage.size(),0);
                            }
                            else
                            {
                                std::string channel = "Server";
                                std::string saludo(buffer);
                                std::string ircMessage = "PRIVMSG " + channel + " :" + "<YOU> "+ saludo + "\r\n";
                                send(fds[u].fd,ircMessage.c_str(),ircMessage.size(),0);
                            }
                        }
                    }
                }
            }
        }
    }
}


/** @attention Main @param Argc,Port,Password @return exit mode */
int main(int argc, char const *argv[])
{
	Server	server(PORT);

    main_loop(server);

    return (0);
}

