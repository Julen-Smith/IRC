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

void    Server::notice_new_user(User *user, int client_index) {

    std::stringstream string_builder;
    std::string server_stream;
    //contruir mensaje de usuario nuevo al resto presente
    string_builder << PRIVMSG << " " << MAIN_CHANNEL << " : " << user->get_nickname() << " " << JOIN_MSG << MSG_END;
    server_stream = string_builder.str();

    //mandar el mensaje
    for(int cli = 0; cli != this->users.size(); cli++) {
        if (cli != client_index)
            send(this->fds[client_index].fd, server_stream.c_str(),server_stream.size(), 0);
    }
}

void    Server::user_command(std::stringstream &key_stream, int client_index) {
    Server::unvalidated_user    unva_user;
    CreateUser  *               created_user;
    std::string                 holder;
    int                         client_socket;
    User    *user;

    client_socket = this->fds[client_index].fd;
    unva_user = this->unvalidated_users.find(client_socket);
    created_user = unva_user->second;

    //LOGINNAME
    get_token(key_stream, holder, SPACE, MSG_END_SPACE);
    created_user->login_name = holder;
    //LEVEL -> casteo a integer
    get_token(key_stream, holder, SPACE, MSG_END_SPACE);
    created_user->level = holder;

    //UNUSED
    get_token(key_stream, holder, SPACE, MSG_END_SPACE);
    //REALNAME
    get_token(key_stream, holder, SPACE, MSG_END_SPACE);
    created_user->realname = holder;

    user = new User(unva_user->second);
    this->users.push_back(user);
    this->send_intro(client_index);
    notice_new_user(user, client_index);
    
    std::cout << "User command\n" << " - login: " << user->_login_name << "\n - realname: " << user->_realname << std::endl;

}

User *  Server::get_user_by_socket(int client_socket) {
    Server::validated_user  it;

    for (it = this->users.begin(); it != this->users.end(); it++) {
        if ((*it)->get_socket() == client_socket)
            return (*it);
    }
    return NULL;
}

bool    Server::check_operator(const std::string &user, const std::string &password, int client_socket) {
    User    *current_user;

    current_user = NULL;
    for (int i = 0; i < 2; i++) {
        if (user == this->priv_list[i].user
        && password == this->priv_list[i].password) {

            current_user = this->get_user_by_socket(client_socket);
            current_user->_level = priv_list[i].level;
            return true;
        }
    }
    return false;
}

void    Server::oper_command(std::stringstream& key_stream, int client_index) {
    std::string user;
    std::string password;
    std::string builder;
    std::stringstream   response;
    int         client_socket;

    client_socket = this->fds[client_index].fd;
    get_token(key_stream, user, SPACE, MSG_END_SPACE);
    get_token(key_stream, password, SPACE, MSG_END_SPACE);

    if (check_operator(user, password, client_socket))
        response << RPL_YOUREOPER << ":You are now an IRC operator\r\n";
    else
        response << ERR_PASSWDMISMATCH << ":Password incorrect\r\n";

    builder = response.str();
    send(client_socket, builder.c_str(), builder.size(), 0);
}

void    Server::kick_command(std::stringstream &key_stream, int client_index) {
    std::string         nickname;
    std::stringstream   response;
    std::string         builder;
    int                 client_socket;

    client_socket = this->fds[client_index].fd;
    get_token(key_stream, nickname, SPACE, MSG_END_SPACE);

    std::cout << "Kick command:\n - nickname: " << nickname << std::endl;

    response << ERR_NICKNAMEINUSE << nickname << " :nickname is already in use\r\n";
    builder = response.str();

    send(client_socket, builder.c_str(), builder.size(), 0);
    close(client_socket);
}

void    Server::nick_command(std::stringstream &key_stream, int client_index) {
    std::stringstream   holder;
    int                 client_socket;
    std::string         nickname;
    validated_user      user;

    get_token(key_stream, nickname, SPACE, MSG_END_SPACE);
    client_socket = this->fds[client_index].fd;
    //TODO check if the user exists, input the client socket
    user = this->get_user_by_nickname(nickname);
    this->unvalidated_users[client_socket]->nickname = nickname;

    std::cout << "Nick command:\n - nickname: " << nickname << std::endl;
    if (user == this->users.end())
        return ;
    //TODO kick and erase unvalidated user
    else if ((*user)->get_notices() == true) {
        holder << nickname;
        kick_command(holder, client_index);
        this->delete_unvalidated_user(client_socket);
        return ;
    }
    this->users.erase(user);
    return ;
}

void    Server::manage_response(int client_index) {
    char    buffer[BUFFER_SIZE];

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