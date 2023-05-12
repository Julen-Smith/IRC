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

void generate_socket(Server *server)
{
    server->port = 4242;
    server->ipv4 = AF_INET; //2
    server->socket_type = SOCK_STREAM; //1
    server->server_socket = socket(server->ipv4, server->socket_type, 0);
    if (server->server_socket == -1)
    {
        std::cerr << SOCKET_ERROR << std::endl;
        //Soltar excepción y propagarla o gestionarla de alguna manera
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
        //
        std::cerr << "El servidor ha sido iniciado ... \n"
        << " _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n" 
        << "| Puerto : " 
        << server->port << "                                 |" 
        << std::endl
        << "| Protocolo : " 
        << (server->ipv4 == 2 ? "Ipv4" : server->ipv4 == 1 ? "Ipv6" : "Uknown")
        << "                              |" 
        << std::endl
        << "| Tipo de conexión : " << (server->socket_type == 1 ? "Socket de conexiones" : "Uknown")
        << "       |"
        << std::endl 
        << " ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯ ¯\n";
    }
}

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

void main_loop(Server *server)
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
        int event_to_handle = poll(&fds[0], fds.size(), 0);
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
                            User new_user(buffer, lobby);
                            lobby.join_channel(buffer, new_user);
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
    Server server;
   //int client_socket;


    generate_socket(&server); //Generar socket
    bind_socket(&server); //Bindear Socket
    listen /* Devuelve cero en caso de exito -1 en caso de eror*/
    (  // Escucharse así mismo para cambios en poll
    server.server_socket, /* Fd socket Server*/
    MAX_CLIENTS /* Es un entero que especifica el número máximo de conexiones pendientes en la cola de conexiones entrantes que aún no han sido aceptadas por el servidor.*/ 
    );
    std::cout << server.server_socket << std::endl;
    main_loop(&server);


    return (0);
}
