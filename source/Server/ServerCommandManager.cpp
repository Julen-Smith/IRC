#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include <string.h>

static int error_return(const std::string error_code,const std::string error_string, Message &msg)
{
    msg.res.str("");
    msg.res << error_code << " " << error_string;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    return (1);
}

static bool erase_back_match(std::string &source, const std::string &to_erase) {

    size_t  erase_len = 0;
    size_t  source_len = source.size();
    
    for (int i = 0; i < to_erase.size(); i++) {
        if (source.find(to_erase[i]) != -1)
            erase_len++;
    }
    if (erase_len)
    {
        source.erase(source_len - erase_len, source_len);
        return true;
    }
    return false;        
}

void    Server::prvmsg_command(Message& msg) {
    Channel *channel;
    std::string channel_name;
    std::string nickname;

    channel_name = msg.get_params_front();
    channel = this->get_channel_by_name(channel_name);
    if (channel == NULL)
        return ;
    //std::cout << "raw: " << msg.buffer << std::endl;
    std::cout << "PRVMSG command\n - Channel name: " << channel_name << ".\n - Nickname: " << msg.user->get_nickname();
    std::cout << "\n - Message: ";
    
    channel->send_msg(msg);
}


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

void    Server::ping_command(Message &msg) {
    std::cout << "Ping command\n - nickname: " << msg.user->get_nickname() << std::endl;
    msg.user->set_curr_time(this->_curr_time);
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
    std::cout << keys << std::endl;
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
            std::cout << msg.get_res_str() << std::endl;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        }
    }
}

//TODO revisar funcionalidad del comando quit
void    Server::quit_command(Message &msg) {
    msg.user->set_notices(DISCONNECTED);
    close(msg.client_socket);
}

//Mode.cpp

void    Server::user_command(Message &msg) {
    unvalidated_user    unva_user;
    UnvalidatedUser     *created_user;
    User                *user;

    if (msg.params->size() < 2) {
        std::cerr << "Invalid params! : " << msg.buffer;
        return ;
    }

    std::cout << "Buffer: " << msg.buffer << std::endl;
    std::cout << "Size: " << msg.params->size() << std::endl;

    unva_user = this->unvalidated_users.find(msg.client_socket);
    if (unva_user == this->unvalidated_users.end())
    {
        close(msg.client_socket);
        return ;
    }
    created_user = unva_user->second;

    //Si el usuario hace user antes de nick es expulsado
    if (created_user->nickname.size() == 0) {
        this->delete_unvalidated_user(msg.client_socket);
        close(msg.client_socket);
        return ;
    }

    created_user->login_name = msg.get_params_front();
    created_user->level = msg.get_params_front();

    //TODO unused
    msg.get_params_front();
    created_user->realname = msg.get_params_front();

    user = this->add_validated_user(unva_user);
    msg.set_user(user);

    msg.res.str("");
    msg.res << RPL_WELCOME << msg.user->get_nickname() << WELCOME << user->get_nickname() << "!" << "paco@yahoo\r\n";
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

    this->send_intro(msg.client_socket);
    this->delete_unvalidated_user(msg.client_socket);
    
    std::cout << "User command\n" << " - login: " << user->_login_name << "\n - realname: " << user->_realname << std::endl;
}

bool    Server::check_operator(Message &msg) {
  /*
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
    */
    return false;
}

//TODO revisar funcionalidad de oper
void    Server::oper_command(Message& msg) {
    bool exist = false;

    msg.holder = msg.split(msg.buffer," ");
    if (msg.holder->size() != 3)
        error_return(ERR_NEEDMOREPARAMS,NEEDMOREPARAMS,msg);
    else
    {
        for (int i = 0; i < this->users.size(); i++){
            std::cout << this->users.at(i)->get_nickname() << msg.holder->at(1) <<std::endl;
            if (this->users.at(i)->get_nickname() == msg.holder->at(1)) {
                
                exist = true;
                break;
            }
        }
        if (!exist && error_return(ERR_NOSUCHNICK,NOSUCHNICK,msg))
            return;
        erase_back_match(msg.holder->at(2),MSG_END);
        if (msg.holder->at(2) == OPER_PASSWORD || msg.holder->at(2) == TEST_USER_PASSWORD)
        {
            if (msg.user->get_operator_status())
            {
                msg.res.str("");
                msg.res << RPL_YOUREOPER <<" :You are no longer an IRC operator\r\n";
                send(msg.client_socket, msg.get_res_str(), msg.get_res_size(),0);
                msg.user->set_operator_status(false);
            }
            else
            {
                msg.res.str("");
                msg.res << RPL_YOUREOPER << YOUREOPER;
                send(msg.client_socket, msg.get_res_str(), msg.get_res_size(),0);
                msg.user->set_operator_status(true);
            }
        }
        else
          error_return(ERR_PASSWDMISMATCH,PASSWDMISMATCH,msg);
    }
}

//TODO revisar funcionalidad de kick
void    Server::kick_command(Message &msg) {

    std::string nickname;

    if (msg.user == NULL)
        return ;
    std::cout << "Kick command:\n - nickname: " << nickname << std::endl;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    msg.user->set_notices(DISCONNECTED);
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

        if (this->find_unva_user_by_socket(msg.client_socket)) {
            msg.user->set_notices(DISCONNECTED);
            close(msg.client_socket);
        }
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

void    Server::check_inac() {

        time_t  curr_time = std::clock();

        curr_time = std::clock();
        if (((curr_time - this->get_curr_time()) / 1000000) > 10) {
            std::cout << "Checking inactive users!\n";
            this->check_users();
            this->set_curr_time(curr_time);
        }
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




void    Server::invite_command(Message &msg) {

    int channel_index = 0;
    bool exist = false;
    bool channel_exist = false;
    bool user_exist = false;
    int real_index = 0;
    
    msg.holder = msg.split(msg.buffer," ");
    std::cout << msg.holder->size() << std::endl;
    if (msg.holder->size() != 3)
        error_return(ERR_NEEDMOREPARAMS,NEEDMOREPARAMS,msg);
    else
    {
        erase_back_match(msg.holder->at(2),MSG_END);
        for (int i = 0; i < this->users.size(); i++){
                if (this->users.at(i)->get_nickname() == msg.holder->at(2)) {
                    exist = true;
                    break;
                }
        }
        if (!exist && error_return(ERR_NOSUCHNICK,NOSUCHNICK,msg))
        {
            std::cout << msg.user->get_nickname().size() << msg.holder->at(2).size() << std::endl;
            std::cout << "No such nick con " << msg.user->get_nickname() << " y " << msg.holder->at(2) << std::endl;
            return;
        } 
        for (channel_index = 0; channel_index < this->channels.size(); channel_index++)
        {
            if (this->channels.at(channel_index)->get_name() == msg.holder->at(1))
            {
                channel_exist = true;
                real_index = channel_index;
            }   
        }
        if (!channel_exist && error_return(ERR_NOSUCHCHANNEL,NOSUCHCHANNEL,msg))
             return ;   
        for (int i = 0; i < this->channels.at(real_index)->get_users().size();i++)
            user_exist = ((this->channels.at(real_index)->get_users().at(i)->get_nickname() == msg.user->get_nickname()) && (!user_exist)) ? true : false;
        if (!user_exist && error_return(ERR_NOTONCHANNEL,NOTONCHANNEL,msg))
            return;
        msg.res.str(":Server NOTICE " + msg.user->get_nickname() + " :Inviting " + msg.holder->at(2) + " to channel " + this->channels.at(real_index)->get_name() + MSG_END);
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

        msg.res.str(":Server NOTICE " + msg.holder->at(2) + " :" + msg.user->get_nickname() + " invited you to channel " + this->channels.at(real_index)->get_name() + MSG_END);
        for(int i = 0; i < this->users.size(); i++)
            if (this->users.at(i)->get_nickname() == msg.holder->at(2))
                send(this->users.at(i)->get_socket(), msg.res.str().c_str(), msg.res.str().size(), 0);
    }

}