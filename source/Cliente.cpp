#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

const int PORT = 4242;
const int BUFFER_SIZE = 1024;

int main() {
    // Crear el socket
    int client_fd;
    struct sockaddr_in server_addr;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    struct pollfd fds[2];
    memset(fds, 0, sizeof(fds));

    fds[0].fd = STDIN_FILENO; // Entrada estándar (teclado)
    fds[0].events = POLLIN;

    fds[1].fd = client_fd; // Socket del cliente
    fds[1].events = POLLIN;

    char buffer[BUFFER_SIZE];

    while (true) {
        poll(fds, 2, -1);

        if (fds[0].revents & POLLIN) {
            std::string input;
            std::getline(std::cin, input);
            send(client_fd, input.c_str(), input.length(), 0);
        }
        

        if (fds[1].revents & POLLIN) {
            int n = recv(client_fd, buffer, BUFFER_SIZE, 0);

            if (n <= 0) {
                std::cout << "Conexión con el servidor cerrada." << std::endl;
                close(client_fd);
                break;
            } else {
                buffer[n] = '\0';
                std::cout << "Mensaje del servidor: " << buffer << std::endl;
            }
        }
    }

    return 0;
}
