#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include <string.h>

//TODO revisar funcionallidad de list
void    Server::list_command(Message &msg) {

    msg.res.str("");
    for (int i = 0; i != this->channels.size(); i++)
    {
        msg.res << RPL_LIST << this->channels[i]->get_name() << " " << this->channels[i]->get_users_size();
        msg.res << " :" << this->channels[i]->get_topic() << MSG_END;
    }
    msg.res << RPL_LISTEND << LISTEND;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

//TODO crear canal solo con invitación.
//TODO nick, usuario, host no puede estar en lista ban.
//TODO en caso de tener contraseña, que sea correcta.
void    Server::join_command(Message &msg) {

    std::deque<std::string> *rooms;
    std::deque<std::string> *keys;

    std::string room_name;
    std::string key;
    Channel     *channel;

    key.clear();
    room_name.clear();

    rooms = NULL;
    keys = NULL;
    if (msg.params->size() == 0 or msg.params->size() > 2 or msg.params->front().size() < 2) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << "JOIN " << NEEDMOREPARAMS;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    } else {

        //TODO hay que hacer split de los canales
        room_name = msg.get_params_front();
        if (msg.params->size() == 2)
            key = msg.get_params_front();
        
        rooms = msg.split(room_name, CSV);
        if (key.size())
            keys = msg.split(key, CSV);

        std::cout << "Join command:\n - channels: " << room_name << "\n - keys: " << key << std::endl;
        for (std::deque<std::string>::iterator it = rooms->begin(); it != rooms->end(); it++) {
            room_name = *it;

            channel = this->get_channel_by_name(room_name);

            //TODO create new channel
            //el canal no existe
            if (channel == NULL) {
                if (this->check_name(room_name)) {
                    msg.res.str("");
                    msg.res << ERR_NOSUCHCHANNEL << msg.user->get_nickname() << room_name << NOSUCHCHANNEL;
                } else {
                    channel = this->create_channel(msg.user, room_name);
                    this->mode_command(msg);
                }
            } 
            //el canal existe
            else {
                //canal solo para invitados
                if (channel->is_already(msg.user->get_nickname()))
                    return ;
                else if (channel->get_invite() == INVITE_ONLY) {
                    msg.res.str("");
                    msg.res << ERR_INVITEONLYCHAN << msg.user->get_nickname() << room_name << " " << INVITEONLYCHAN;
                //canal con limite de usuarios superado
                } else if (channel->is_limit_raised()) {
                    msg.res.str("");
                    msg.res << ERR_CHANNELISFULL << msg.user->get_nickname() << " " << room_name << CHANNELISFULL;
                //usuario baneado
                } else if (channel->is_banned(msg.user->get_nickname())) {
                    msg.res.str("");
                    msg.res << ERR_BANNEDFROMCHAN << msg.user->get_nickname() << " " << room_name << BANNEDFROMCHAN;
                } else {
                    if (channel->enter_key(key) == INCORRECT_KEY) {
                        msg.res.str("");
                        msg.res << ERR_BADCHANNELKEY << msg.user->get_nickname() << " " << room_name << BADCHANNELKEY;
                    } else {
                        if (msg.user->get_count() >= MAX_CHANNELS) {
                            msg.res.str("");
                            msg.res << ERR_TOOMANYCHANNELS << msg.user->get_nickname() << " " << room_name << TOOMANYCHANNELS;
                        } else {
                            channel->add_user(msg.user);
                            msg.res.str("");
                            msg.res << channel->get_topic_msg(msg.user) << channel->get_user_list_msg(msg.user);
                        }
                    }
                }
            }
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        }
    }
}

//TODO revisar funcionalidad del comando quit
void    Server::quit_command(Message &msg) {
    close(msg.client_socket);
}

//TODO revisar funcionalidad del comando mode
void    Server::mode_command(Message &msg) {
    std::string room_name;


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

    //TODO unused
    msg.get_params_front();
    created_user->realname = msg.get_params_front();

    user = this->add_validated_user(unva_user);
    msg.set_user(user);

    msg.res.str("");
    msg.res << RPL_WELCOME << WELCOME << user->get_nickname() << "!" << "paco@yahoo\r\n";
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

    this->send_intro(msg.client_socket);
    ////this->notice_new_user(msg);

    this->delete_unvalidated_user(msg.client_socket);
    
    std::cout << "User command\n" << " - login: " << user->_login_name << "\n - realname: " << user->_realname << std::endl;
}

bool    Server::check_operator(Message &msg) {
    User    *current_user;
    std::string     user;
    std::string     password;

    user = msg.get_params_front();
    password = msg.get_params_front();

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

//TODO revisar funcionalidad de oper
void    Server::oper_command(Message& msg) {

    if (msg.params->size() != 2) {
        msg.res.str("");
        msg.res << "OPER " << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS;
    } else {
        
        if (check_operator(msg)) {
            msg.res << RPL_YOUREOPER << YOUREOPER;
            msg.res << "MODE +o\r\n";
        }
        else
            msg.res << ERR_PASSWDMISMATCH << PASSWDMISMATCH;
    }

    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

//TODO revisar funcionalidad de kick
void    Server::kick_command(Message &msg) {

    std::string nickname;

    std::cout << "Kick command:\n - nickname: " << nickname << std::endl;

    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    close(msg.client_socket);
}

void    Server::part_command(Message &msg) {

    std::deque<std::string> *params;
    std::string token;
    Channel *channel;

    token = msg.get_params_front();
    params = msg.split(token, CSV);

    for (size_t i = 0; i < params->size(); i++) {

        channel = this->get_channel_by_name(params->front());
        if (channel == NULL)
         continue ;

        channel->delete_user(msg.user->get_nickname());
        params->pop_front();
    }

    delete params;
}

void    Server::nick_command(Message &msg) {

    UnvalidatedUser *unvalid_user;
    User            *user;
    std::string     nickname;

    nickname = msg.get_params_front();
    if (msg.params->size() != 0 or nickname.size() == 0) {
        msg.res << ERR_NONICKNAMEGIVEN << NONICKNAMEGIVEN;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
    else if (this->check_name(nickname)) {
        msg.res << ERR_ERRONEUSNICKNAME << nickname << ERRONEUSNICKNAME;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

        if (this->find_unva_user_by_socket(msg.client_socket))
            close(msg.client_socket);
    }
    //TODO avisar al restor del cambio de nick
    else if (this->find_unva_user_by_socket(msg.client_socket) == false) {
        user = this->get_user_by_socket(msg.client_socket);
        user->set_nickname(nickname);
        return ;
    }

    unvalid_user = this->unvalidated_users[msg.client_socket];
    unvalid_user->nickname = nickname;
    user = this->get_user_by_nickname(unvalid_user->nickname);

    std::cout << "Nick command:\n - nickname: " << unvalid_user->nickname << std::endl;
    if (!user)
        return ;

    else if (user->get_notices() == true) {
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
    User    *user;

    msg.client_index = client_index;
    msg.client_socket = this->fds[client_index].fd;

    if (this->read_socket(msg))
        return ;

    //TODO asegurar flujo de codigo
    user = this->get_user_by_socket(msg.client_socket);
    if (user != NULL)
        msg.set_user(user);
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