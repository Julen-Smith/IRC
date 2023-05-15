#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>

#include "Server.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "defs.hpp"

int command_manager(std::string &command,Server *server)
{
    #define CREATE "/create"
    std::cout << "Im in" << std::endl;
    if (!command.compare(CREATE))
    {
        //Channel channel("TEST ROOM",server);
        return (0);
    }
    return (1);
}

# define TIMEOUT	0
# define PASSWORD   "42Urduliz"

/** @attention Función que genera el socket @param @return */


/** @attention sdfdfs @param sdfsd @return dfdfs */

/*
    @details
    @param
    @return
*/

/** @attention Main loop del servidor @param none @return */
void main_loop(Server &server)
{

    char buffer[BUFFER_SIZE];
    std::vector<pollfd> fds;
    std::vector<bool> notices;
    std::vector<std::string> message_list;
    std::string password  = "42Urduliz";
    Channel lobby(server);

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
                    //ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
            //        memset(buffer, 0, sizeof(buffer));
            //        recv(fds[1].fd, buffer, BUFFER_SIZE,0);

                    std::string saludo = "Welcome to the Server Lobby.";
                    std::string channel = "";
                    //std::string message = "SQUIT";
                    std::string ircMessage = "PRIVMSG " + channel + " :" + saludo + "\r\n";

                    send(fds[1].fd,ircMessage.c_str(),ircMessage.size(),0);
                    std::string format(buffer);

                    std::cout << buffer << std::endl;
                    //format.clear();
                    std::string lobby_message = "<-Server-> Welcome to Lobby\n";
                    send(fds[i].fd, lobby_message.c_str(), lobby_message.length(), 0);
                    if (notices[i] == false)
                    {
                          notices[i] = true;




                        //
                        //

                        ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                        std::cout << "Se recibe la password : " << buffer << std::endl;

                        if (!password.compare(buffer))
                        {
                            memset(buffer, 0, sizeof(buffer));
                            std::cout << "Password confirm successfull" << std::endl;
                            std::string lobby_message = "<-Server-> Welcome to Lobby\n";
                            //Rutina de creación de usuarios
                            send(fds[i].fd, lobby_message.c_str(), lobby_message.length(), 0);

                            std::string nickname_question = "<-Server-> Please enter your nickname : ";
                            send(fds[i].fd, nickname_question.c_str(), nickname_question.length(), 0);
                            memset(buffer, 0, sizeof(buffer));

                            ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                            std::cout << "User " << buffer << " has been logged :D" << std::endl;
                            User new_user(buffer, lobby);
                            lobby.join_channel(buffer, new_user);
                            //Creación de objeto usuario;
                        }
                        else
                        {
                            std::cout << "Se cierra el fd del cliente por contraseña erronea" << std::endl;
                            close(fds[i].fd);
                        }
                    }
                    else
                    {
                    }
                }
            }
        }
    }
}


/**
 * @attention Funciones destacadas
 * socket : Crea un endpoint
 *
 *
 *
 * setsockopt :
 *
 * @param Argc,Port,Password
 * @return Void
 * */

/** @attention Main @param Argc,Port,Password @return exit mode */

/*
    FLAGS POLL
    POLLIN BUSCA DATOS DISPONIBLES PARA LEER EN LOS FDS ASIGNADOS
    POLLOUT VUELVE EN CASO DE QUE SE PUEDA ESCRIBIR SIN BLOQUEAR EN LOS FDS ASIGNADOS
    POLLHUP PARA AVISOS DE CIERRE DE CONEXION
    POLLNVAL NO ES VALIDO O NO ESTÁ ABIERTO
*/

int main(int argc, char const *argv[])
{
    //Server server;
   //int client_socket;


	Server	server(PORT);
    //generate_socket(); //Generar socket
    //bind_socket(&server); //Bindear Socket
    std::cout << server.get_socket() << std::endl;
    main_loop(server);


    return (0);
}

