#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include <string.h>

void    Server::build_message_and_send(std::string code,int client,std::string user, std::string reply_mode,
std::string reply, int nbr_strings, ...)
{   
    va_list args;
    std::string builder = "";

    va_start(args, nbr_strings);
    if (reply_mode == USR_RPL)
        builder = ":" + user + " " + code + " ";
    else if (reply_mode == SRV_RPL)
        builder = ":"+ code + " " + reply + " ";
    for (int i = 0; i < nbr_strings; ++i)
    {
        std::string str = va_arg(args, const char*);
        builder += str + " ";
    }
    builder += MSG_END;
    send(fds[client].fd,builder.c_str(),builder.size(),0);
    va_end(args); 
}


void    Server::list_command(std::stringstream &key_sream, int client_index)
{
    build_message_and_send("321", client_index,"Server",USR_RPL, "", 0);       
    for (int channels = 0; channels != this->channels.size(); channels++)
    {
        build_message_and_send("322",client_index,"Server",USR_RPL,"",
        4,
        this->users.at(client_index)->get_nickname().c_str(),
        this->channels.at(channels)->get_name().c_str(),
        std::to_string(this->channels.at(channels)->get_users_size()).c_str(),
        this->channels.at(channels)->get_topic().c_str()
        );
    }
    build_message_and_send("323",client_index,"Server",USR_RPL, "", 0);
}

void    Server::join_command(std::stringstream &key_stream, int client_index) {

    std::string room_name;
    std::string nickname;
    int         room_index;

    get_token(key_stream, room_name, ' ', MSG_END_SPACE);
    room_index = check_channel(room_name);
    if (room_index == BAD_ROOM)
        return ;

    std::cout << "join command\n - room name: " << room_name << "\nroom index: " << room_index << std::endl;
    nickname = this->users.at(client_index)->get_nickname(); 

    build_message_and_send("JOIN",client_index,
    nickname,
    USR_RPL,
    "",1, this->channels[room_index]->get_name().c_str());
    build_message_and_send("332",client_index,"",SRV_RPL,
    "RPL_TOPIC", 1, this->channels[room_index]->get_name().c_str()," :Bienvenido");
    //hacer función que devuelva usuarios dentro del channel
    build_message_and_send("353",client_index,"",SRV_RPL,
    "RPL_TOPIC", 1, this->channels[room_index]->get_name().c_str()," :",
    this->channels.at(room_index)->get_user_list().c_str()); 
    build_message_and_send("366",client_index,"",SRV_RPL,
    "RPL_ENDOFNAMES", 1, this->channels[room_index]->get_name().c_str()," :Fin de la lista de nombres");
    this->channels.at(room_index)->add_user(this->users.at(client_index));
}

void    Server::user_command(std::stringstream &key_stream, int client_index) {
    std::string login;
    std::string tmp;
    std::string realname;

    get_token(key_stream, login, SPACE, MSG_END_SPACE);
    get_token(key_stream, tmp, SPACE, MSG_END_SPACE);
    get_token(key_stream, tmp, SPACE, MSG_END_SPACE);
    get_token(key_stream, realname, SPACE, MSG_END_SPACE);
    std::cout << "User command\n" << " - login: " << login << "\n - realname: " << realname << std::endl;
}

void    Server::nick_command(std::stringstream &key_stream, int client_index) {
    int                 client_socket;
    std::stringstream   string_builder;
    std::string         server_stream;
    std::string         nickname;
    validated_user      user;

    client_socket = this->fds[client_index].fd;
    //TODO check if the user exists, input the client socket
    if (check_unvalidated_user(client_socket)) {
        user = this->get_user_by_nickname(nickname);
        if (check_validated_user(user))
            close(client_socket);
        else {
            this->users.erase(user);
            this->unvalidated_users[client_socket]->nickname = nickname;
        }
    }
        std::cout << "asdf" << std::endl;
    std::cout << "idfnfg" << std::endl;

    //asignar nombre de usuario
    get_token(key_stream, nickname, SPACE, MSG_END_SPACE);
    std::cout << "Nick command\n" << " - nickname: " << nickname << std::endl;
    this->users[client_index]->set_nickname(nickname);

    //contruir mensaje de usuario nuevo al resto presente
    string_builder << PRIVMSG << " " << MAIN_CHANNEL << " : " << nickname << " " << JOIN_MSG << MSG_END;
    server_stream = string_builder.str();

    //mandar el mensaje
    for(int cli = 0; cli != this->users.size(); cli++) {
        if (cli != client_index)
            send(this->fds[client_index].fd, server_stream.c_str(),server_stream.size(), 0);
    }
}

void    Server::manage_response(int client_index) {
    char    buffer[BUFFER_SIZE];

    //recibir el mensaje de un usuario
    if (this->read_socket(client_index, buffer))
        return ;

    this->tokenizer(client_index, buffer);
}
    //        else
    //        {
    //            std::string channel_name("#");
    //            channel_name+=compare;
    //            Channel *new_channel = new Channel(channel_name,"Default topic");
    //            this->channels.push_back(new_channel);
    //            JOIN(clientIndex,this->channels.size() - 1);
    //        }