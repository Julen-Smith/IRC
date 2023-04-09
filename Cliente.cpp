#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Crear el socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Configurar la dirección del servidor
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(4242);

    // Conectar al servidor
    int con = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (con == 0)
        std::cout << "Conexion con el servidor es correcta" << std::endl;

    char message[1024];
    
/*
    // Recibir el mensaje del servidor
    char buffer[1024] = {0};
    usleep(5000);
    read(client_socket, buffer, 1024);
    std::cout << buffer << std::endl;
*/
    // Cerrar el socket
    while(1)
    {
        std::cin >> message;
        send(client_socket, message, strlen(message), 0);
    }
    close(client_socket);

    return 0;
}