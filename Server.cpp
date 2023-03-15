#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define SOCKET_ERROR "Error al crear el socket"
#define SOCKET_PREF_ERROR "Error al setear las preferencias del socket"

typedef struct server
{
    int port; // Puerto
    int server_socket; //Fd que utiliza el servidor como socket
    int ipv4;
    int socket_type;

} Server;


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
            usleep(10000);
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
    if (listen(server->server_socket, 3) < 0) {
        std::cerr << "Error al escuchar conexiones entrantes\n";
        exit(0) ;
    }
}


int main(int argc, char const *argv[])
{
    Server server;
    sockaddr_in clientes{};
    int client_socket;
    socklen_t client_address_size = sizeof(clientes);

    generate_socket(&server);
    bind_socket(&server);
    while(1)
    {
        client_socket = accept(server.server_socket, (sockaddr *) &clientes, &client_address_size);
        if (client_socket < 0) {
            std::cerr << "Error al aceptar la conexión entrante\n";
            return (1);
        }else
            std::cout << "Cliente aceptado" << std::endl;
    }

    return (0);
}
