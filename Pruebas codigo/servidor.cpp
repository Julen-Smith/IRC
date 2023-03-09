#include <iostream>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h> 
#include <cstring>
#include <exception>

class Server
{
    private:
        int server_fd;
        int new_socket;
        int valred;
        struct sockaddr_in address;
        int addrlen;
        char buffer[1024];
    public:
        Server();
        ~Server();
        void start_server();
        void init_socket();
        void bind_socket();
        void listen_cli();
        void incoming();
        void send_message();
        void recibir_mensaje();
};

Server::Server()
{
    this->addrlen = sizeof(address);
    this->address.sin_family = AF_INET; // IPv4
    this->address.sin_addr.s_addr = INADDR_ANY; // Accept connections on any interface
    this->address.sin_port = htons(500); // Puerto
}

void Server::init_socket()
{
    int example = 1;
    try {

    //Crear socket
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (!this->server_fd)
        throw std::runtime_error("ERR SOCKET");
    //Setear socket a puerto
    int response = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &example, sizeof(example)) != 0 ? response = 1 : throw std::runtime_error("ERR SETSOCK");
    } 
    catch (const std::exception& e)
    {
            //print
    }
}

void Server::bind_socket()
{
    try
    {
       int response = bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0 ? response = 1 : throw std::runtime_error("ERR BINDSOCK");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void Server::listen_cli()
{
    try
    {
       int response = listen(this->server_fd, 3) < 0 ? response = 1 : throw std::runtime_error("ERR LISTEN");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}


void Server::incoming()
{
    try
    {
       int response = accept(this->server_fd, (struct sockaddr *)&this->address, (socklen_t*)&this->addrlen) <0 ? response = 1 : throw std::runtime_error("ERR INC");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Server::send_message()
{
    const char* mensaje = "Primer mensaje del servidor";

    send(this->new_socket, &mensaje, strlen(mensaje), 0);
}

void Server::recibir_mensaje()
{
    this->valred = read(new_socket, this->buffer, 1024);
    std::cout << "Mensaje: " << buffer << std::endl;
}

void Server::start_server()
{
    //Hacer propagación de excepciones desde aqui, que las funciones hagan el throw y recuperarlo desde aqui
    //Mirar error mensajes de preprocesado para ver si se pueden printar cosillas extra mas profesional como en java #
      try
      {
        init_socket();
        bind_socket();
        listen_cli();
        incoming();
        send_message();
        recibir_mensaje();
      }catch(const std::exception& e)
      {

      }

}


Server::~Server()
{
    //Fd servidor
    close(this->server_fd);
}

int main(int argc, char **argv) {
    
    Server servidor;

    servidor.start_server();

    
    return (0);
}