#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

const int PORT = 4242;
const int BUFFER_SIZE = 1024;

#define CREATE "/create"


int main() {
    // Crear el socket
    std::string password  = "42Urduliz";
    std::string admin_password  = "AP";
    int server_fd;
    struct sockaddr_in server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Server fd : " <<  server_fd << std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    std::cout << "Conectado a servidor" << std::endl;
    
    //SEND PASSWORD ARGV[2];
    char buffer[BUFFER_SIZE];
    std::cout << "Cliente envia contraseña" << std::endl;
    send(server_fd, password.c_str(), password.size(), 0);
    std::cout << "Cliente espera respuest del servidor\n\n" << std::endl;
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    
    std::cout << buffer << std::endl; // Welcome to lobby

    recv(server_fd, buffer, BUFFER_SIZE, 0); // Enter your nickname
    /* NICKNAME PART */
    std::cout << buffer << " ";
    std::string nickname;
    getline(std::cin,nickname);
    send(server_fd,nickname.c_str(),nickname.size(),0);

    
    while (true)
    {      
            std::cout << "Cliente esperando" << std::endl;
    }

    return 0;
}
