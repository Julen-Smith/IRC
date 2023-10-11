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
        builder = code + " ";
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

std::string Server::get_nickname_by_socket(int client_socket) {
    return this->get_user_by_socket(client_socket)->get_nickname();
}


void    Server::list_command(Message &msg) {

    for (int i = 0; i != this->channels.size(); i++)
    {
        msg.res << RPL_LIST;
        msg.res << this->get_nickname_by_socket(msg.client_socket);
        msg.res << " " << this->channels[i]->get_name() << " " << this->channels[i]->get_users_size();
        msg.res << " :" << this->channels[i]->get_topic() << MSG_END;
    }
    msg.res << RPL_LISTEND << LISTEND;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

Channel    *Server::create_channel(const User *user, const std::string &room_name) {
    Channel *channel;

    channel = new Channel(room_name, "There is no topic"); 
    this->channels.push_back(channel);
    channel->add_user(user);
    std::cout << "New channel created: " << room_name << std::endl;
    return channel;
}

void    Server::join_command(Message &msg) {

    std::string room_name;
    std::string key;
    Channel     *channel;
    User        *user;

    //msg.request >> room_name;
    //msg.request >> key;

    channel = this->get_channel_by_name(room_name);
    user = this->get_user_by_socket(msg.client_socket);

    //TODO create new channel
    if (channel == NULL) {
        channel = this->create_channel(user, room_name);
        msg.push_bucket(room_name);
        this->mode_command(msg);
    }

    msg.res.str("");
    msg.res << channel->get_topic_msg(user);

    msg.res << channel->get_user_list_msg(user);
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
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

void    Server::quit_command(Message &msg) {
    close(msg.client_socket);
}

void    Server::mode_command(Message &msg) {
    std::string room_name;

    room_name = msg.pop_bucket();

    msg.res.str("");
    msg.res << ":Server MODE " << room_name << " +nt" << MSG_END;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

void    Server::user_command(Message &msg) {
    unvalidated_user    unva_user;
    UnvalidatedUser     *created_user;
    User                *user;

    unva_user = this->unvalidated_users.find(msg.client_socket);
    created_user = unva_user->second;

    created_user->login_name = msg.get_params_front();
    created_user->level = msg.get_params_front();
    created_user->realname = msg.get_params_front();

    user = this->add_validated_user(unva_user);

    msg.res.str("");
    msg.res << RPL_WELCOME << WELCOME << user->get_nickname() << "!" << "paco@yahoo\r\n";
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

    this->send_intro(msg.client_index);
    notice_new_user(user, msg.client_index);
    
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

bool    Server::check_operator(Message &msg) {
    User    *current_user;
    std::string     user;
    std::string     password;

    //msg.request >> user;
    //msg.request >> password;

    current_user = NULL;
    for (int i = 0; i < 2; i++) {
        if (user == this->priv_list[i].user
        && password == this->priv_list[i].password) {

            current_user = this->get_user_by_socket(msg.client_socket);
            current_user->_level = priv_list[i].level;
            return true;
        }
    }
    return false;
}

void    Server::oper_command(Message& msg) {

    if (check_operator(msg))
        msg.res << RPL_YOUREOPER << ":You are now an IRC operator\r\n";
    else
        msg.res << ERR_PASSWDMISMATCH << ":Password incorrect\r\n";

    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

void    Server::kick_command(Message &msg) {

    std::string nickname;

    nickname = msg.pop_bucket();
    std::cout << "Kick command:\n - nickname: " << nickname << std::endl;

    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    close(msg.client_socket);
}

bool    Server::delete_user_by_socket(int client_socket) {
    validated_user  user;

    user = this->users.begin();
    for (; user != this->users.end(); user++) {
        if ((*user)->get_socket() == client_socket) {
            this->users.erase(user);
            return true;
        }
    }
    return false;
}

static bool check_nickname(const std::string &nickname) {
    for (size_t i = 0; i < nickname.size(); i++) {
        if (!isalpha(nickname[i]))
            return true;
    }
    return false;
}

void    Server::nick_command(Message &msg) {

    UnvalidatedUser *unvalid_user;
    User            *user;
    std::string     nickname;

    nickname = msg.params->front();
    std::cout << "size: " << msg.params->size() << std::endl;
    if (msg.params->size() == 0) {
        msg.res << ERR_NONICKNAMEGIVEN << NONICKNAMEGIVEN;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }
    else if (check_nickname(msg.params->front())) {
        msg.res << ERR_ERRONEUSNICKNAME << nickname << ERRONEUSNICKNAME;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }
    unvalid_user = this->unvalidated_users[msg.client_socket];
    unvalid_user->nickname = nickname;
    user = this->get_user_by_nickname(unvalid_user->nickname);

    std::cout << "Nick command:\n - nickname: " << unvalid_user->nickname << std::endl;
    if (!user)
        return ;

    else if (user->get_notices() == true) {
        msg.push_bucket(unvalid_user->nickname);
        msg.res.str("");
        msg.res << ERR_NICKNAMEINUSE << "* ";
        msg.res << unvalid_user->nickname;
        msg.res << ": nickname is already in use\r\n";
        kick_command(msg);

        this->delete_unvalidated_user(msg.client_socket);
        return ;
    }
    this->delete_user_by_socket(user->get_socket());
}

void    Server::manage_response(int client_index) {
    Message msg;

    msg.client_index = client_index;
    msg.client_socket = this->fds[client_index].fd;

    if (this->read_socket(msg))
        return ;
    this->tokenizer(msg);
}



    //        else
    //        {
    //            std::string channel_name("#");
    //            channel_name+=compare;
    //            Channel *new_channel = new Channel(channel_name,"Default topic");
    //            this->channels.push_back(new_channel);
    //            JOIN(clientIndex,this->channels.size() - 1);
    //        }