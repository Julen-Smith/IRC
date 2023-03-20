

void init_socket(Server_data *server)
{
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Setear socket a puerto
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &example, sizeof(example));

}

/*
void bind_socket(Server_data *server)
{
    bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address));
}

void listen_cli(Server_data *server)
{
    listen(this->server_fd, 3);
}


void incoming(Server_data *server)
{
    
    accept(this->server_fd, (struct sockaddr *)&this->address, (socklen_t*)&this->addrlen);
}

void send_message(Server_data *server)
{
    const char* mensaje = "Primer mensaje del servidor";

    send(this->new_socket, &mensaje, strlen(mensaje), 0);
}

void recibir_mensaje(Server_data *server)
{
    this->valred = read(new_socket, this->buffer, 1024);
    std::cout << "Mensaje: " << buffer << std::endl;
}

void start_server(Server_data *server)
{
        init_socket();
        bind_socket();
        listen_cli();
        incoming();
        send_message();
        recibir_mensaje();
}
*/