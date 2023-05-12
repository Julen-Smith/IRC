#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>

#include "Server.h"
#include "Channel.hpp"
#include "User.hpp"


const int BUFFER_SIZE = 1024;
const int MAX_CLIENTS = 10;




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

void main_loop(Server &server)
{
    /*
        FLAGS POLL
        POLLIN BUSCA DATOS DISPONIBLES PARA LEER EN LOS FDS ASIGNADOS
        POLLOUT VUELVE EN CASO DE QUE SE PUEDA ESCRIBIR SIN BLOQUEAR EN LOS FDS ASIGNADOS
        POLLHUP PARA AVISOS DE CIERRE DE CONEXION
        POLLNVAL NO ES VALIDO O NO ESTÁ ABIERTO
    */
    char buffer[BUFFER_SIZE];
    std::vector<pollfd> fds;
    std::vector<bool> notices;
    //NO VALE. notices.size() == 0.
    for(int i = 0; i < notices.size(); i++)
        notices[i] = false;
    std::vector<std::string> message_list;
    std::string password  = "42Urduliz";
    Channel lobby(server);

    fds.push_back(pollfd());
    fds[0].fd = server->server_socket;
    fds[0].events = POLLIN;
    while (true)
    {
        int event_to_handle = poll(&fds[0], fds.size(), TIMEOUT);
       // std::cout << event_to_handle << std::endl;
        for(int i = 0; i < fds.size(); i++)
        {
            //Se comprueba a nivel de bit si se puede leer el fichero
            //Realmente se comprueba el último bit para ver si esta activado o no
            // La unica diferencia con una comparativa habitual es la eficiencia 000001 0000011 != and true
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == server->server_socket)
                {
                    sockaddr_in clientes;
                    socklen_t client_address_size = sizeof(clientes);
                    int client_fd = accept(server->server_socket, (sockaddr *)&clientes, &client_address_size);
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
                        ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                        std::cout << "Se recibe la password : " << buffer << std::endl;
                        if (!password.compare(buffer))
                        {
                            memset(buffer, 0, sizeof(buffer));
                            std::cout << "Password confirm successfull" << std::endl;
                            std::string lobby_message = "<-Server-> Welcome to Lobby\n";
                            //Rutina de creación de usuarios
                            send(fds[i].fd, lobby_message.c_str(), lobby_message.length(), 0);

                            std::string nickname_question = "<-Server-> Please enter your nickname : " ;
                            send(fds[i].fd, nickname_question.c_str(), nickname_question.length(), 0);
                            memset(buffer, 0, sizeof(buffer));

                            ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                            std::cout << "User " << buffer << " has been logged :D" << std::endl;
                            //User* new_user = new User(buffer, lobby);

                            lobby.join_channel(buffer);
                            //Creación de objeto usuario;
                            /* HARDCODE*/
                        }
                        else
                        {
                            std::cout << "Se cierra el fd del cliente por contraseña erronea" << std::endl;
                            //close(fds[i].fd);
                        }
                    //std::cout << " Me bloqueo aqui con el siguiente fd " << std::endl;
                    //ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                    //if (!strcmp(buffer,"/exit") || bytes_read <= 0)
                    //{
                    //    close(fds[i].fd);
                    //    fds.erase(fds.begin() + i);
                    //    std::cout << "Se ha desconectado el cliente " << i << " " << std::endl;
                    }
                    else
                    {
                       // std::cout << "Gestion de mensaje normal " << std::endl;
                        //std::cout<< "entro aqui";
                       // std::string message(buffer, bytes_read);
                        //if (command_manager(message,server))
                       // std::cout << "Mensaje recibido de cliente " << i << " : " << message << std::endl;
                            //for(int u = 0; u < fds.size(); u++)
                            //    send(fds[i].fd, copytry.c_str(), copytry.length(),0);
                        // message_list.push_back(buffer); Historial de las salas?
                    }
                }
            }
        }
    }
}


int main(int argc, char const *argv[])
{
    //Server server;
   //int client_socket;


	Server	server(PORT);
    //generate_socket(); //Generar socket
    //bind_socket(&server); //Bindear Socket
    std::cout << server.server_socket << std::endl;
    main_loop(&server);


    return (0);
}
