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

#include <fstream>

void    Server::motd_command(Message& msg) {

    std::ifstream   inputFile("inc/Server.info");

    msg.res.str("");
    if (!inputFile.is_open()) {
        std::cout << "No se pudo abrir el archivo." << std::endl;
        msg.res << ERR_NOMOTD << NOMOTD;
    } else if (msg.params->size() or !msg.user) {
        return ;
    } else {
        msg.res << RPL_MOTDSTART << msg.user->get_nickname() << MOTDSTART;

        //for (std::string line; std::getline(inputFile, line);)
        //    msg.res << RPL_MOTD << msg.user->get_nickname() << line << MSG_END;
        msg.res << RPL_ENDOFMOTD << msg.user->get_nickname() << " " << ENDOFMOTD;
    }
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

void    Server::whois_command(Message& msg) {

    std::string nickname;
    User        *user;

    if (msg.params->size() > 1 or msg.user == NULL) {
        return ;
    }

    msg.res.str("");
    if (msg.params->size() == 1) {
        nickname = msg.get_params_front();
	std::cout << "nickname: " << nickname << std::endl;
        user = this->get_user_by_nickname(nickname);
        if (!user) {
            msg.res << ERR_NOSUCHNICK << NOSUCHNICK;
        } else {

            msg.res << RPL_WHOISUSER << user->get_nickname() << " " << user->get_nickname() << " " << user->_login_name << " juluk.org * " << user->_realname << "\r\n";
            msg.res << RPL_WHOISSERVER << user->get_nickname() << " " << user->get_nickname() << " juluk.org :42 Urduliz IRC chat\r\n";
            if (user->_channels.size()) {
                msg.res << RPL_WHOISCHANNELS << user->get_nickname() << " " << user->get_nickname();
                msg.res << user->get_channels_str() << MSG_END;
            }
            msg.res << RPL_ENDOFWHOIS << "<nick> :End of WHOIS list\r\n";
        }

    } else {

        msg.res << RPL_WHOISUSER << msg.user->get_nickname() << " " << msg.user->get_nickname() << " " << msg.user->_login_name << " juluk.org * " << msg.user->_realname << "\r\n";
        msg.res << RPL_WHOISSERVER << msg.user->get_nickname() << " " << msg.user->get_nickname() << " juluk.org :42 Urduliz IRC chat\r\n";
        if (msg.user->_channels.size()) {
            msg.res << RPL_WHOISCHANNELS << msg.user->get_nickname() << " " << msg.user->get_nickname();
            msg.res << msg.user->get_channels_str() << MSG_END;
        }
        msg.res << RPL_ENDOFWHOIS << "<nick> :End of WHOIS list\r\n";
    }

    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

}

void    Server::prvmsg_command(Message& msg) {
    Channel *channel;
    User    *user;
    std::string name;

    if (msg.params->size() == 0 or !msg.user)
        return ;

    name = msg.get_params_front();
    channel = this->get_channel_by_name(name);
    if (channel != NULL)
        channel->send_msg(msg);
    else {
        user = this->get_user_by_nickname(name);
        if (!user) {
            std::cout << "handle this error\n";
            return ;
        }
        msg.res.str("");
        msg.res << ":" << msg.user->get_nickname() << " " << PRIVMSG << " " << user->get_nickname();
        while (msg.params->size() > 0) {
         msg.res << " " << msg.get_params_front();
        }
        msg.res << MSG_END;
        send(user->get_socket(), msg.get_res_str(), msg.get_res_size(), 0);
    }
    //std::cout << "raw: " << msg.buffer << std::endl;
    std::cout << "PRVMSG command\n - Channel name: " << name << ".\n - Nickname: " << msg.user->get_nickname();
    std::cout << "\n - Message: ";

}


//TODO revisar funcionallidad de list
void    Server::list_command(Message &msg) {

    if (!msg.user)
        return ;

    msg.res.str("");
    for (int i = 0; i != this->channels.size(); i++)
    {
        msg.res << RPL_LIST << this->channels[i]->get_name() << " " << this->channels[i]->get_users_size();
        msg.res << " :" << this->channels[i]->get_topic() << MSG_END;
    }
    msg.res << RPL_LISTEND << LISTEND;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

void    Server::pass_command(Message &msg) {

    UnvalidatedUser *unvalid_user;
    std::string     nickname;
    std::cout << msg.params->size() << std::endl;
    if (msg.params->size() == 0)
    {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS << MSG_END;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }
    if (msg.params->size() != 1) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << "Too many arguments" << MSG_END;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }

    std::cout << msg.user << std::endl;
    if (msg.user and msg.user->get_notices())
    {
        return ;
    }

    std::string password = msg.get_params_front();
    std::cout << "Password: " << password << std::endl;

    if (msg.user and msg.user->get_notices()) {

        msg.res.str("");
        msg.res << ERR_ALREADYREGISTRED << ALREADYREGISTRED;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;

    } else if (this->_password != password) {

        close(msg.client_socket);
        this->delete_unvalidated_user(msg.client_socket);
        return ;
    }
    unvalid_user = this->unvalidated_users[msg.client_socket];
    unvalid_user->password = 1;

}

void    Server::ping_command(Message &msg) {

    if (!msg.user)
        return ;

    std::cout << "Ping command\n - nickname: " << msg.user->get_nickname() << std::endl;

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

    if (!msg.user)
        return ;

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
        if (msg.params->size() == 1)
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
                channel = this->create_channel(msg.user, room_name);
                msg.res << channel->get_topic_msg(msg.user) << channel->get_user_list_msg(msg.user);
                msg.res << ":juluk.org MODE " << room_name << " +o " << msg.user->get_nickname() << MSG_END;
                this->channels.push_back(channel);
            }
            //el canal existe
            else {
                //canal solo para invitados
                if (channel->is_already(msg.user->get_nickname()))
                    return ;
                else if (channel->get_invite() == INVITE_ONLY && msg.user->get_operator_status() == false) {
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
                            channel->notice_join(msg);
                        }
                    }
                }
            }
            std::cout << "Aqui " << msg.get_res_str() << std::endl;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        }
    }
}

# define EXIT ":exited without saying bye :("

//TODO revisar funcionalidad del comando quit
void    Server::quit_command(Message &msg) {
    std::vector<Channel *>::iterator it;

    if (!msg.user)
        return ;

    it = this->channels.begin();
    for (; it != this->channels.end(); it++) {
        if ((*it)->is_already(msg.user->get_nickname())) {
            (*it)->delete_user(msg.user->get_nickname());
            (*it)->notice_part(msg, EXIT);
        }
    }
    msg.user->set_notices(DISCONNECTED);
    close(msg.client_socket);
}

//Mode.cpp

void    Server::user_command(Message &msg) {
    unvalidated_user    unva_user;
    UnvalidatedUser     *created_user;
    User                *user;

    if (msg.params->size() != 4) {
        std::cerr << "Invalid params! : " << msg.buffer;
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }

    std::cout << "Buffer: " << msg.buffer << std::endl;
    std::cout << "Size: " << msg.params->size() << std::endl;

    user = this->get_user_by_socket(msg.client_socket);
    if (user and user->get_notices())
        return ;

    unva_user = this->unvalidated_users.find(msg.client_socket);
    if (unva_user == this->unvalidated_users.end())
    {
        close(msg.client_socket);
        return ;
    }
    created_user = unva_user->second;

    //Si el usuario hace user antes de nick es expulsado
    if (created_user->nickname.size() == 0) {
        std::cout << "Close 4\n";
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

    this->motd_command(msg);
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

    if (!msg.user)
        return ;

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

    if (!msg.user)
        return ;
    if (msg.user->get_operator_status() == false) {
        msg.res.str("");
        msg.res << "481 :Permission Denied- You're not an IRC operator";
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }
    std::cout << "Kick command:\n - nickname: " << nickname << std::endl;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    msg.user->set_notices(DISCONNECTED);
    close(msg.client_socket);
}

void    Server::part_command(Message &msg) {

    std::deque<std::string> *params;
    std::string token;
    std::string topic = ":without reason";
    Channel *channel;

    if (!msg.user)
        return ;

    //if (msg.user->get_operator_status() == false) {
   //     msg.res.str("");
   //     msg.res << "481 :Permission Denied- You're not an IRC operator";
  //      send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
   //     return ;
   // }

    if (!msg.user)
        return ;

    if (msg.params->size() < 1) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS;
        send(msg.user->get_socket(), msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }

    token = msg.get_params_front();
    params = msg.split(token, CSV);
    if (msg.params->size()) {
        topic.clear();
        while (msg.params->size())
            topic += msg.get_params_front() + " ";
        topic[topic.size() - 1] = 0;
    }

    for (size_t i = 0; i < params->size(); i++) {

        token = params->front();
        params->pop_front();

        channel = this->get_channel_by_name(token);
        if (channel == NULL) {
            msg.res.str("");
            msg.res << ERR_NOSUCHCHANNEL << token << " " << NOSUCHCHANNEL;
            continue ;
        }

        channel->notice_part(msg, topic);
        channel->delete_user(msg.user->get_nickname());

        if (channel->get_users_size() == 0) {
            std::cout << "Empty channel!" << std::endl;
            this->delete_channel(token);
        }
    }
    msg.user->set_notices(DISCONNECTED);
}

void    Server::nick_command(Message &msg) {

    UnvalidatedUser *unvalid_user;
    std::string     nickname;

    if (msg.params->size() == 0) {// or nickname.size() == 0) {

        msg.res << ERR_NONICKNAMEGIVEN << NONICKNAMEGIVEN;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

        return ;
    }

    nickname = msg.get_params_front();
    msg.res.str("");
    if (this->is_already(nickname)) {
	if (msg.user) {
		msg.res << ERR_NICKNAMEINUSE << msg.user->get_nickname() << NICKNAMEINUSE;
	}
        nickname += '_';
    }

    if (this->check_name(nickname)) {

        msg.res << ERR_ERRONEUSNICKNAME << nickname << ERRONEUSNICKNAME;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);

        if (this->find_unva_user_by_socket(msg.client_socket)) {
            std::cout << "close 1\n";
            msg.user->set_notices(DISCONNECTED);
            close(msg.client_socket);
        }
        return ;
    }
    //TODO avisar al restor del cambio de nick
    else if (this->find_unva_user_by_socket(msg.client_socket) == false) {
	    msg.res << ":" << msg.user->get_nickname() << " NICK :" << nickname << MSG_END;
	    msg.user->set_nickname(nickname);
	    std::cout << msg.get_res_str();
	    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
	    return ;
    }

    unvalid_user = this->unvalidated_users[msg.client_socket];
    if (unvalid_user->password == 0) {
        this->delete_unvalidated_user(msg.client_socket);
        close(msg.client_socket);
        return ;
    }
    unvalid_user->nickname = nickname;
}

void    Server::manage_response(int client_index) {
    Message msg;
    User    *user;

    msg.client_index = client_index;
    msg.client_socket = this->fds[client_index].fd;

    std::cout << "Manage response\n";
    if (this->read_socket(msg))
        return ;

    //TODO asegurar flujo de codigo
    user = this->get_user_by_socket(msg.client_socket);
    if (user != NULL)
        msg.set_user(user);
    this->tokenizer(msg);
}

void Server::topic_command(Message& msg)
{
    bool channel = false;
    int index= 0;
    bool complete_execution = false;
    int channel_pos = 1;

    msg.holder = msg.split(msg.buffer," ");
    std::cout << msg.holder->size() << std::endl;
    if (msg.holder->size() == 1)
        error_return(ERR_NEEDMOREPARAMS,NEEDMOREPARAMS,msg);
    erase_back_match(msg.holder->at(1),MSG_END);
    if (msg.holder->size() > 2)
    {
        channel_pos = 1;
        std::cout << msg.holder->at(1) << std::endl;
        complete_execution = true;
        erase_back_match(msg.holder->at(1),MSG_END);
    }
    for (index = 0; index < this->channels.size(); index++)
    {
        std::cout << this->channels.at(index)->get_name() << " " << msg.holder->at(channel_pos)<< std::endl;
        if (this->channels.at(index)->get_name() == msg.holder->at(channel_pos))
        {
            channel = true;
            break;
        }
    }
    if (channel && !complete_execution)
    {
        if (this->channels.at(index)->get_topic() == "")
        {
            msg.res.str("");
            msg.res << RPL_NOTOPIC << " " << msg.user->get_nickname() << " " << msg.holder->at(1) << " " << NOTOPIC;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        }
        else
        {
            msg.res.str("");
            msg.res << RPL_TOPIC << " " << this->channels.at(index)->get_name() << " : ";
            msg.res << this->channels.at(index)->get_topic() <<MSG_END;
            std::cout << msg.get_res_str() << std::endl;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        }
    }
    else if (complete_execution && channel)
    {

        for(int i = 3; i < msg.holder->size(); i++)
            msg.holder->at(2) += " " + msg.holder->at(i);
        this->channels.at(index)->set_topic(msg.holder->at(2));
        msg.res << RPL_TOPIC << " " << this->channels.at(index)->get_name() << " : ";
        msg.res << this->channels.at(index)->get_topic() <<MSG_END;
        std::cout << msg.get_res_str() << std::endl;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
    else
    {
        error_return(ERR_NOSUCHCHANNEL,NOSUCHCHANNEL,msg);
        return;
    }
}


void    Server::invite_command(Message &msg) {

    int channel_index = 0;
    bool exist = false;
    bool channel_exist = false;
    bool user_exist = false;
    int real_index = 0;

    if (!msg.user)
        return ;
    msg.holder = msg.split(msg.buffer," ");
    if (msg.holder->size() != 3)
        error_return(ERR_NEEDMOREPARAMS,NEEDMOREPARAMS,msg);
    else
    {
        erase_back_match(msg.holder->at(2),MSG_END);
        for (int i = 0; i < this->users.size(); i++){
            std::cout << this->users.at(i)->get_nickname() << " y " << msg.holder->at(2) <<std::endl;
                if (this->users.at(i)->get_nickname() == msg.holder->at(2)) {
                    exist = true;
                    break;
                }
        }
        if (!exist && error_return(ERR_NOSUCHNICK,NOSUCHNICK,msg))
            return;
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
        {
            std::cout << this->channels.at(real_index)->get_users().at(i)->get_nickname() << " y " << msg.user->get_nickname() << std::endl;
            if (this->channels.at(real_index)->get_users().at(i)->get_nickname() == msg.user->get_nickname() && !user_exist)
                user_exist = true;
        }

        if (!user_exist && error_return(ERR_NOTONCHANNEL,NOTONCHANNEL,msg))
            return;
        std::map<const User*, std::vector<char> > *user_permissions =  this->channels.at(real_index)->get_user_permissions();
        if (!msg.user->get_operator_status() && (*user_permissions)[msg.user].at(2) == 0)
        {
            error_return(ERR_CHANOPRIVSNEEDED,CHANOPRIVSNEEDED,msg);
            return;
        }
        msg.res.str(":Server NOTICE " + msg.user->get_nickname() + " :Inviting " + msg.holder->at(2) + " to channel " + this->channels.at(real_index)->get_name() + MSG_END);
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

        msg.res.str(":Server NOTICE " + msg.holder->at(2) + " :" + msg.user->get_nickname() + " invited you to channel " + this->channels.at(real_index)->get_name() + MSG_END);
        for(int i = 0; i < this->users.size(); i++)
            if (this->users.at(i)->get_nickname() == msg.holder->at(2))
                send(this->users.at(i)->get_socket(), msg.res.str().c_str(), msg.res.str().size(), 0);
    }

}
