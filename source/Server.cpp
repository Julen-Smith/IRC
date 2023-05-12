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

#define SOCKET_ERROR "Error al crear el socket"
#define SOCKET_PREF_ERROR "Error al setear las preferencias del socket"

const int BUFFER_SIZE = 1024;
const int MAX_CLIENTS = 10;


/** @attention Función que genera el socket @param @return */


/** @attention sdfdfs @param sdfsd @return dfdfs */
void bind_socket(Server *server)
{
    struct sockaddr_in address;

    address.sin_family = server->ipv4;
    address.sin_addr.s_addr = INADDR_ANY; //Aceptar cualquier dirección
    address.sin_port = htons(server->port);
    if (bind(server->server_socket, (sockaddr *) &address, sizeof(address)) < 0) {
        std::cerr << "Error al asignar la dirección al socket\n";
        exit (0);
    }
   //if (listen(server->server_socket, 3) < 0) {
   //     std::cerr << "Error al escuchar conexiones entrantes\n";
   //     exit(0) ;
   // }
}

#define MAX_CLIENTS 50


/*
    @details
    @param 
    @return
*/

/** @attention Main loop del servidor @param  @return */
void main_loop(Server *server)
{

    char buffer[BUFFER_SIZE];
    std::vector<pollfd> fds;
    std::vector<bool> notices;
    std::vector<std::string> message_list;
    std::string password  = "42Urduliz";
    Channel lobby(server);

    fds.push_back(pollfd());
    fds[0].fd = server->server_socket;
    fds[0].events = POLLIN;
    while (true)
    {
        int event_to_handle = poll(&fds[0], fds.size(), 0);
        for(int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)  //Comprobación cambios fichero
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
 * 
 * 
 * 
 * @param Argc,Port,Password 
 * @return Void
 * */
void generate_socket(Server *server)
{
    server->port = 4242;
    server->ipv4 = AF_INET; //2
    server->socket_type = SOCK_STREAM; //1
    server->server_socket = socket(server->ipv4, server->socket_type, 0);
    if (server->server_socket == -1)
    {
        std::cerr << SOCKET_ERROR << std::endl;
        exit(0);
    }
    else
    {
        //  Añadir preferencias de conexión para que reutilice IPS
        int option = 1;
        //params
        //server fd, nivel de socket , función a utilizar, valor de la función a utilizar, longitud de la función
        while (setsockopt(server->server_socket, SOL_SOCKET,SO_REUSEADDR, &option,sizeof(option)) < 0)
        {
            std::cerr << SOCKET_PREF_ERROR << std::endl;
            std::cerr << "Intentandolo de nuevo . . ." << std::endl;
            usleep(500000);
            //Soltar excepción y propagarla o gestionarla de alguna manera
            //  exit(0);
        }
        print_server_menu(server);
    }
}

/** @attention Main @param Argc,Port,Password @return exit mode */
int main(int argc, char const *argv[])
{
    Server server;

    generate_socket(&server); //Generar socket
    bind_socket(&server); //Bindear Socket
    listen(server.server_socket,MAX_CLIENTS); 
    main_loop(&server);
    return (0);
}

/*
    FLAGS POLL
    POLLIN BUSCA DATOS DISPONIBLES PARA LEER EN LOS FDS ASIGNADOS
    POLLOUT VUELVE EN CASO DE QUE SE PUEDA ESCRIBIR SIN BLOQUEAR EN LOS FDS ASIGNADOS
    POLLHUP PARA AVISOS DE CIERRE DE CONEXION
    POLLNVAL NO ES VALIDO O NO ESTÁ ABIERTO
*/