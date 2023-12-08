#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include <string.h>
#include <stdlib.h>

static int error_return(const std::string error_code,const std::string error_string, Message &msg)
{
    msg.res.str("");
    msg.res << error_code << " " << error_string;
    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
    return (1);
}

static bool erase_back_match(std::string &source, const std::string &to_erase) {

    size_t  erase_len = 0;
    size_t  source_len = source.size();

    for (size_t i = 0; i < to_erase.size(); i++) {
        if (source.find(to_erase[i]) != static_cast<size_t>(-1))
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
        msg.res << ERR_NOMOTD << NOMOTD;
    } else if (msg.params->size() or !msg.user) {
        return ;
    } else {
        msg.res << RPL_MOTDSTART << msg.user->get_nickname() << MOTDSTART;

        for (std::string line; std::getline(inputFile, line);)
            msg.res << RPL_MOTD << msg.user->get_nickname() << " " << line << MSG_END;
        msg.res << RPL_ENDOFMOTD << msg.user->get_nickname() << " " << ENDOFMOTD;
    }
    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
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

    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

}

void    Server::prvmsg_command(Message& msg) {
    Channel *channel;
    User    *user;
    std::string name;

    if (msg.params->size() == 0 or !msg.user)
        return ;

    name = msg.get_params_front();
    channel = this->get_channel_by_name(name);
    if (channel != NULL and channel->is_already(msg.user->get_nickname()))
        channel->send_msg(msg);
    else {
        user = this->get_user_by_nickname(name);
        if (!user) {
            return ;
        }
        msg.res.str("");
        msg.res << ":" << msg.user->get_nickname() << " " << PRIVMSG << " " << user->get_nickname();
        while (msg.params->size() > 0) {
         msg.res << " " << msg.get_params_front();
        }
        msg.res << MSG_END;
        send(user->get_socket(), msg.res.str().c_str(), msg.res.str().size(), 0);
    }
}


//TODO revisar funcionallidad de list
void    Server::list_command(Message &msg) {

    if (!msg.user)
        return ;

    msg.res.str("");
    for (size_t i = 0; i != this->channels.size(); i++)
    {
        msg.res << RPL_LIST << this->channels[i]->get_name() << " " << this->channels[i]->get_users_size();
        msg.res << " :" << this->channels[i]->get_topic() << MSG_END;
    }
    msg.res << RPL_LISTEND << LISTEND;
    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
}

void    Server::pass_command(Message &msg) {

    UnvalidatedUser *unvalid_user;
    std::string     nickname;

    if (msg.params->size() == 0)
    {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS << MSG_END;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }
    if (msg.params->size() != 1) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << "Too many arguments" << MSG_END;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }
    if (msg.user and msg.user->get_notices())
        return ;

    std::string password = msg.get_params_front();
    if (msg.user and msg.user->get_notices()) {

        msg.res.str("");
        msg.res << ERR_ALREADYREGISTRED << ALREADYREGISTRED;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;

    } else if (this->_password != password) {

        close(msg.client_socket);
        this->delete_unvalidated_user(msg.client_socket);
        return ;
    }

    std::cout << "Socket(pass_command): " << msg.client_socket << std::endl;
    unvalid_user = this->unvalidated_users[msg.client_socket];
    if (!unvalid_user)
        return ;

    std::cout << "Unvalid user: " << unvalid_user << std::endl;
    unvalid_user->password = 1;
}

void    Server::ping_command(Message &msg) {

    if (!msg.user)
        return ;

    msg.res << ":juluk.org PONG " << msg.user->get_nickname() << " :juluk.org\r\n";
    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

}
//TODO crear canal solo con invitación.
//TODO nick, usuario, host no puede estar en lista ban.
//TODO en caso de tener contraseña, que sea correcta.
void    Server::join_command(Message &msg) {

    std::deque<std::string> *rooms = NULL;
    std::deque<std::string> *keys = NULL;

    std::string room_name;
    std::string key;
    Channel     *channel;

    if (!msg.user)
        return ;

    msg.res.str("");
    if (msg.params->size() == 0 or msg.params->size() > 2 or msg.params->front().size() < 2) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << "JOIN " << NEEDMOREPARAMS;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

    } else {
        //TODO hay que hacer split de los canales

        room_name = msg.get_params_front();
        if (room_name.empty() || room_name[0] != '#')
        {
            msg.res << ERR_BADCHANMASK << msg.user->get_nickname() << " #" << BADCHANMASK;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return;
        }
        if (msg.params->size() == 1)
            key = msg.get_params_front();

        rooms = msg.split(room_name, CSV);
        if (key.size())
            keys = msg.split(key, CSV);

        for (std::deque<std::string>::iterator it = rooms->begin(); it != rooms->end(); it++) {
            room_name = *it;

            channel = this->get_channel_by_name(room_name);

            //TODO create new channel
            //el canal no existe
            if (channel == NULL and msg.user->get_count() >= MAX_CHANNELS) {

                msg.res << ERR_TOOMANYCHANNELS << msg.user->get_nickname() << " " << room_name << TOOMANYCHANNELS;

            } else if (channel == NULL) {

                channel = this->create_channel(msg.user, room_name);

                msg.res << channel->get_topic_msg(msg.user) << channel->get_user_list_msg(msg.user);
                msg.res << "juluk.org MODE " << room_name << " +nt\r\n";
            }
            //el canal existe
            else {
                //canal solo para invitados
                if (channel->is_already(msg.user->get_nickname()))
                {
                    delete rooms;
                    delete keys;
                    return ;
                }
                else if (channel->is_flag(INVITE) == INVITE_ONLY && msg.user->get_operator_status() == false && !channel->is_invited(msg.user)) {
                    msg.res << ERR_INVITEONLYCHAN << msg.user->get_nickname() << room_name << " " << INVITEONLYCHAN;
                //canal con limite de usuarios superado
                } else if (channel->is_limit_raised()) {
                    msg.res << ERR_CHANNELISFULL << msg.user->get_nickname() << " " << room_name << CHANNELISFULL;
                //usuario baneado
                } else if (channel->is_banned(msg.user->get_nickname())) {
                    msg.res << ERR_BANNEDFROMCHAN << msg.user->get_nickname() << " " << room_name << BANNEDFROMCHAN;
                } else {
                    if (channel->enter_key(key) == INCORRECT_KEY) {
                        msg.res << ERR_BADCHANNELKEY << msg.user->get_nickname() << " " << room_name << BADCHANNELKEY;
                    } else {
                        if (msg.user->get_count() >= MAX_CHANNELS) {
                            msg.res << ERR_TOOMANYCHANNELS << msg.user->get_nickname() << " " << room_name << TOOMANYCHANNELS;
                        } else {
                            channel->add_user(msg.user);
                            msg.res << channel->get_topic_msg(msg.user) << channel->get_user_list_msg(msg.user);
                            channel->notice_join(msg);
                        }
                    }
                }
            }
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        }
        delete rooms;
        delete keys;
    }
}

# define EXIT ":exited without saying bye :("

//TODO revisar funcionalidad del comando quit
void    Server::quit_command(Message &msg) {
    std::vector<Channel *>::iterator it;
    std::deque<std::string>::iterator param;
    if (!msg.user)
        return ;

    it = this->channels.begin();
    for (; it != this->channels.end();) {

        if ((*it)->is_already(msg.user->get_nickname())) {
            (*it)->delete_user(msg.user->get_nickname());
            (*it)->notice_part(msg, EXIT);

            if ((*it)->get_users_size() == 0) {
                delete (*it);
                this->channels.erase(it);
            } else
                it++;
        } else
            it++;
    }
    close(msg.client_socket);
    this->delete_user_by_socket(msg.client_socket);
    //msg.user->set_notices(DISCONNECTED);
    msg.user = NULL;

    param = msg.commands->begin();
    for (; param != msg.commands->end(); ) {
        std::cout << "Commands size: " << msg.commands->size() << std::endl;
        std::cout << "Commands cnt: " << *param << std::endl;
        param = msg.commands->erase(param);
    }
}

//Mode.cpp

void    Server::user_command(Message &msg) {
    unvalidated_user    unva_user;
    UnvalidatedUser     *created_user = NULL;
    User                *user = NULL;

    if (msg.params->size() != 4) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }

    user = this->get_user_by_socket(msg.client_socket);
    if (user and user->get_notices())
        return ;

    unva_user = this->unvalidated_users.find(msg.client_socket);
    if (!unva_user->second)
        return ;

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

    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

    this->motd_command(msg);
    this->delete_unvalidated_user(msg.client_socket);
}

void    Server::close_command(Message& msg) {
    (void)msg;
    this->loop = 0;
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
        for (size_t i = 0; i < this->users.size(); i++){
            if (this->users.at(i)->get_nickname() == msg.holder->at(1) && this->users.at(i)->get_nickname() == msg.user->get_nickname())
            {
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
                send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
                msg.user->set_operator_status(false);
            }
            else
            {
                msg.res.str("");
                msg.res << RPL_YOUREOPER << YOUREOPER;
                send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
                msg.user->set_operator_status(true);
            }
        }
        else
          error_return(ERR_PASSWDMISMATCH,PASSWDMISMATCH,msg);
    }
}

//TODO revisar funcionalidad de kick
void    Server::kick_command(Message &msg) {

    Channel     *channel;
    std::string nickname;
    std::string channel_name;
    std::string reason;

    if (!msg.user)
        return ;

    msg.res.str("");
    if (msg.params->size() < 2) {

        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }

    if (0 and msg.user->get_operator_status() == false) {
        msg.res.str("");
        msg.res << "481 :Permission Denied- You're not an IRC operator";
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }
    std::cout << "Cantidad de parametros " << msg.params->size() << std::endl;

    channel_name = msg.get_params_front();
    nickname     = msg.get_params_front();
    if (msg.params->size() > 1)
        reason       = msg.get_params_front();

    channel = this->get_channel_by_name(channel_name);
    if (!channel) {
        msg.res << ERR_NOSUCHCHANNEL << NOSUCHCHANNEL;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }

    if (channel->is_operator(msg.user) == false) {

        msg.res << ERR_CHANOPRIVSNEEDED << CHANOPRIVSNEEDED;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }

    if (!channel->is_already(nickname)) {
        msg.res << ERR_NOTONCHANNEL << NOTONCHANNEL;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return ;
    }

    msg.res << ":juluk.org KICK " << channel_name << " " << nickname << " " << reason << MSG_END;
    channel->broadcast_msg(msg);
    channel->delete_user(nickname);
}

void    Server::part_command(Message &msg) {

    std::deque<std::string> *params;
    std::string token;
    std::string topic = ":without reason";
    Channel *channel;

    if (!msg.user)
        return ;

    if (msg.params->size() < 1) {
        msg.res.str("");
        msg.res << ERR_NEEDMOREPARAMS << NEEDMOREPARAMS;
        send(msg.user->get_socket(), msg.res.str().c_str(), msg.res.str().size(), 0);
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

        msg.res.str("");
        token = params->front();
        params->pop_front();

        channel = this->get_channel_by_name(token);
        if (channel == NULL) {
            msg.res << ERR_NOSUCHCHANNEL << token << " " << NOSUCHCHANNEL;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            continue ;
        }

        else if (!channel->is_already(msg.user->get_nickname())) {
            msg.res << ERR_NOTONCHANNEL << NOTONCHANNEL;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            continue;
        }

        msg.user->delete_channel(channel);
        channel->notice_part(msg, topic);
        channel->delete_user(msg.user->get_nickname());

        if (channel->get_users_size() == 0)
            this->delete_channel(token);
    }
    //msg.user->set_notices(DISCONNECTED);
    delete params;
}

void    Server::nick_command(Message &msg) {

    UnvalidatedUser *unvalid_user;
    std::string     nickname;

    if (this->find_unva_user_by_socket(msg.client_socket) == false and msg.user == NULL)
        return ;

    if (msg.params->size() == 0) {// or nickname.size() == 0) {

        msg.res << ERR_NONICKNAMEGIVEN << NONICKNAMEGIVEN;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

        return ;
    }

    nickname = msg.get_params_front();
    msg.res.str("");
    while (this->is_already(nickname)) {
        //if (msg.user) {
        //    msg.res << ERR_NICKNAMEINUSE << msg.user->get_nickname() << NICKNAMEINUSE;
        //}
        nickname += '_';
    }

    if (this->check_name(nickname)) {

        msg.res << ERR_ERRONEUSNICKNAME << nickname << ERRONEUSNICKNAME;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);

        if (this->find_unva_user_by_socket(msg.client_socket)) {
            close(msg.client_socket);
        }
        return ;
    }
    //TODO avisar al restor del cambio de nick
    else if (this->find_unva_user_by_socket(msg.client_socket) == false) {
	    msg.res << ":" << msg.user->get_nickname() << " NICK :" << nickname << MSG_END;
	    msg.user->set_nickname(nickname);
	    send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
	    return ;
    }

    unvalid_user = this->unvalidated_users[msg.client_socket];
    if (!unvalid_user)
        return ;

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

    if (this->read_socket(msg))
        return ;

    user = this->get_user_by_socket(msg.client_socket);
    if (user != NULL)
        msg.set_user(user);
    std::cout <<msg.buffer << std::endl;
    this->tokenizer(msg);
}

void Server::topic_command(Message& msg)
{
    Channel *ch;
    bool channel = false;
    size_t index= 0;
    bool complete_execution = false;
    int channel_pos = 1;

    if (!msg.user)
        return ;

    msg.holder = msg.split(msg.buffer," ");
    if (msg.holder->size() == 1) {
        error_return(ERR_NEEDMOREPARAMS,NEEDMOREPARAMS,msg);
        return ;
    }

    erase_back_match(msg.holder->at(1),MSG_END);
    if (msg.holder->size() > 2)
    {
        channel_pos = 1;
        ch = this->get_channel_by_name(msg.holder->at(1));
        if (ch == NULL) {

            msg.res << ERR_NOSUCHCHANNEL << NOSUCHCHANNEL;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return ;
        }
        else if (!ch->is_already(msg.user->get_nickname())) {

            msg.res << ERR_NOTONCHANNEL << NOTONCHANNEL;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return ;

        }
        else if (ch->is_flag(TOPIC) && !ch->is_operator(msg.user) && !msg.user->get_operator_status()) {

            msg.res << ERR_CHANOPRIVSNEEDED << CHANOPRIVSNEEDED;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return ;

        }

        complete_execution = true;
        erase_back_match(msg.holder->at(1),MSG_END);
    }

    for (index = 0; index < this->channels.size(); index++)
    {
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
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        }
        else
        {
            msg.res.str("");
            msg.res << RPL_TOPIC << msg.user->get_nickname() << " " << this->channels.at(index)->get_name() << " :";
            msg.res << this->channels.at(index)->get_topic() << MSG_END;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        }
    }
    else if (complete_execution && channel)
    {

        for(size_t i = 3; i < msg.holder->size(); i++)
            msg.holder->at(2) += " " + msg.holder->at(i);
        this->channels.at(index)->set_topic(msg.holder->at(2));
        msg.res << RPL_TOPIC << msg.user->get_nickname() << " " << this->channels.at(index)->get_name() << " ";
        msg.res << this->channels.at(index)->get_topic() << MSG_END;
        msg.res << ":juluk.org 333 " << msg.user->get_nickname() << " " << this->channels.at(index)->get_name() << " * " << MSG_END;
        ch->broadcast_msg(msg);
    }
    else
    {
        error_return(ERR_NOSUCHCHANNEL,NOSUCHCHANNEL,msg);
        return;
    }
}


void    Server::invite_command(Message &msg) {

    size_t channel_index = 0;
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
        for (size_t i = 0; i < this->users.size(); i++){
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
        for (size_t i = 0; i < this->channels.at(real_index)->get_users().size();i++)
        {
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
        for(size_t i = 0; i < this->users.size(); i++)
            if (this->users.at(i)->get_nickname() == msg.holder->at(2))
            {
                send(this->users.at(i)->get_socket(), msg.res.str().c_str(), msg.res.str().size(), 0);
                this->channels.at(real_index)->add_invited_user(this->users.at(i));
            }
    }

}
