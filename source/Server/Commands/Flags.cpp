#include "Server.hpp"
#include <exception>

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

int get_channel_index(std::vector<Channel *> channels, std::string channel)
{

    if (channels.empty())
        return (-1);
    for (int i = 0; i < channels.size(); i++) {
        if(channels.at(i)->get_name() == channel){
            return (i);
        }
    }
    return (-1);
}

User* get_user_index(std::vector<Channel *> channels, int channel_index, std::string user)
{
    for (int i = 0; i < channels.at(channel_index)->get_users().size(); i++) {
        if(channels.at(channel_index)->get_users().at(i)->get_nickname() == user)
            return (channels.at(channel_index)->get_users().at(i));
    }
    return (0);
}

void i_flag(Message &msg,char impact,Server *serv)
{
    std::map<const User*, std::vector<char> >::iterator it;
    std::vector<User *>         users;
    std::cout << msg.buffer << std::endl;
    User * user_obj;
    int ind = 0;
    bool finded;

    finded = false;
    if(msg.holder->size() == 3)
    {
        ind = get_channel_index(serv->channels,msg.holder->at(1));
        if (ind == -1)
            return ;

        for (int i = 0; i < serv->channels.at(ind)->get_users_size();i++)
            if (serv->channels.at(ind)->get_users().at(i)->get_nickname() == msg.user->get_nickname())
                finded = true;

        if (!finded)
        {
            msg.res.str(":Server 441 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :You aren't on that channel" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return;
        }
        std::map<const User*, std::vector<char> > *user_permissions = serv->channels.at(ind)->get_user_permissions();
        std::cout << "User " << msg.user->get_nickname() << msg.user->get_operator_status() << std::endl;
        std::cout << (*user_permissions)[msg.user].at(2) << " " << msg.user->get_operator_status() << std::endl;
        
        if ((*user_permissions)[msg.user].at(2) == '0' && !msg.user->get_operator_status())
        {
            msg.res.str(":Server 482 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :You're not channel operator" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return;
        }
        if (ind >= 0 && impact == '+')
        {
            std::string channelName = serv->channels.at(ind)->get_name();
            serv->channels.at(ind)->set_invite(true);
            msg.res.str(":Server MODE " + channelName + " +i" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            std::cout << "Nombre canal " <<serv->channels.at(ind)->get_name() << std::endl;
            serv->channels.at(ind)->get_channel_permissions()->at(5) = true;
            msg.res.str(":Server 324 " + msg.user->get_nickname() + " " + channelName + " +i" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return;
        }
        else if (impact == '-')
        {
            std::string channelName = serv->channels.at(ind)->get_name();
            serv->channels.at(ind)->set_invite(false);
            msg.res.str(":Server MODE " + channelName + " -i" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            serv->channels.at(ind)->get_channel_permissions()->at(5) = false;
            msg.res.str(":Server 324 " + msg.user->get_nickname() + " " + channelName + " -i" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return;
        }else
        {
            msg.res.str("");
            msg.res << ERR_NOSUCHCHANNEL << NOSUCHCHANNEL;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
            return ;
        }
    }
    std::string channel = msg.holder->at(1);
    std::string user = msg.holder->at(3);
    int index = get_channel_index(serv->channels,channel);
    if (index == -1)
    {
        std::cout << "no encuentro index del canal" << std::endl;
        return ;
    }
    if (get_user_index(serv->channels, index, user) == 0)
    {
        msg.res.str("");
        msg.res << ERR_NOSUCHNICK << NOSUCHNICK;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }
    user_obj = get_user_index(serv->channels, index, user);
    std::cout << user_obj->get_nickname() << std::endl;
    it = serv->channels.at(index)->get_user_permissions()->find(user_obj);
    std::vector<char>& permissions = it->second;
    if (permissions[0] == '1' && impact == '+')
    {
        msg.res.str("");
        msg.res << ERR_CUSTOM << INVISIBLE_ALR;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
    else if(permissions[0] == '0' && impact == '+')
    {
        permissions[0] = '1';
        msg.res.str("");
        msg.res << ERR_CUSTOM << INVISIBLE;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
    else if (permissions[0] == '0' && impact == '-')
    {
        msg.res.str("");
        msg.res << ERR_CUSTOM << VISIBLE_ALR;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
    else if (permissions[0] == '1' && impact == '-')
    {   
        permissions[0] = '0';
        msg.res.str("");
        msg.res << ERR_CUSTOM << VISIBLE;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
    std::string socket_to_nick;
    for (int i = 0; i < serv->users.size(); i++)
    {
        if (serv->users.at(i)->get_socket() == msg.client_socket)
            socket_to_nick = serv->users.at(i)->get_nickname();
    }
    if (permissions[0] == '1')
    {
        std::cout << "Si" << std::endl;
    }
}
   
void w_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag w" << std::endl;
}

void s_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag s" << std::endl;
}

void o_flag(Message &msg,char impact,Server *serv)
{
    int ind = 0;
    bool finded = false;
    bool user_exist = false;

    if(msg.holder->size() != 4)
        return;
    ind = get_channel_index(serv->channels,msg.holder->at(1));
    if (ind < 0)
        return; 
    for (int i = 0; i < serv->channels.at(ind)->get_users_size();i++)
        if (serv->channels.at(ind)->get_users().at(i)->get_nickname() == msg.user->get_nickname())
                finded = true;
    if (!finded)
    {
        msg.res.str(":Server 441 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :User or you aren't on that channel" + MSG_END);
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return;
    }
    for (int i = 0; i < serv->channels.at(ind)->get_users_size();i++)
        if (serv->channels.at(ind)->get_users().at(i)->get_nickname() == msg.holder->at(3))
                user_exist = true;
    if (!user_exist)
    {
        msg.res.str("");
        msg.res << ERR_NOSUCHNICK << NOSUCHNICK;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return ;
    }
    std::map<const User*, std::vector<char> > *user_permissions = serv->channels.at(ind)->get_user_permissions();
    serv->channels.at(ind)->stdout_channel__users_permissions(msg.user);
    if ((*user_permissions)[msg.user].at(2) == '0' && !msg.user->get_operator_status())
    {
        msg.res.str(":Server 482 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :You're not channel operator" + MSG_END);
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return;
    }
    if (impact == '+')
    {
       (*user_permissions)[msg.user].at(2) = '1';
        msg.res << ":juluk.org MODE " << msg.holder->at(1) << " +o " << msg.user->get_nickname() << MSG_END;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return;
    }
    else
    {
        (*user_permissions)[msg.user].at(2) = '0';
        msg.res << ":juluk.org MODE " << msg.holder->at(1) << " -o " << msg.user->get_nickname() << MSG_END;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return;
    }
}
void v_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag v" << std::endl;
}

void q_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag q" << std::endl;
}

void t_flag(Message &msg,char impact,Server *serv)
{
    int ind = 0;
    int finded = 0;

    if(msg.holder->size() == 3)
    {
        ind = get_channel_index(serv->channels,msg.holder->at(1));
        if (ind == -1)
        {
            msg.res.str("");
            msg.res << ERR_NOSUCHNICK << NOSUCHNICK;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return ;
        }
        for (int i = 0; i < serv->channels.at(ind)->get_users_size(); i++)
            if (serv->channels.at(ind)->get_users().at(i)->get_nickname() == msg.user->get_nickname())
                finded = true;
        if (!finded)
        {
            msg.res.str(":Server 441 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :You aren't on that channel" + MSG_END);
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
            return;
        }
        if (impact == '+')
        {
            serv->channels.at(ind)->get_channel_permissions()->at(0) = true;
            msg.res.str("");
            msg.res << ":juluk.org MODE " << msg.holder->at(1) << " +t " << msg.user->get_nickname() << MSG_END;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        }
        else if (impact == '-')
        {
            serv->channels.at(ind)->get_channel_permissions()->at(0) = false;
            msg.res.str("");
            msg.res << ":juluk.org MODE " << msg.holder->at(1) << " -t " << msg.user->get_nickname() << MSG_END;
            send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        }
    }
    else
    {
        msg.res.str("");
        msg.res << ERR_CUSTOM << "Invalid number of parameters" << MSG_END;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    }
}

void k_flag(Message &msg,char impact,Server *serv)
{
    int ind = 0;
    bool finded = false;

    if(msg.holder->size() != 4)
    {
        msg.res.str("");
        msg.res << ERR_CUSTOM << "Invalid number of parameters" << MSG_END;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        return;
    }
    ind = get_channel_index(serv->channels,msg.holder->at(1));
    for (int i = 0; i < serv->channels.at(ind)->get_users_size();i++)
        if (serv->channels.at(ind)->get_users().at(i)->get_nickname() == msg.user->get_nickname())
                finded = true;
    if (!finded)
    {
        msg.res.str(":Server 441 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :You aren't on that channel" + MSG_END);
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return;
    }
    std::map<const User*, std::vector<char> > *user_permissions = serv->channels.at(ind)->get_user_permissions();
    serv->channels.at(ind)->stdout_channel__users_permissions(msg.user);
    if ((*user_permissions)[msg.user].at(2) == '0' && !msg.user->get_operator_status())
    {
        msg.res.str(":Server 482 " + msg.user->get_nickname() + " " + serv->channels.at(ind)->get_name() + " :You're not channel operator" + MSG_END);
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
        return;
    }
    if (impact == '+')
    {
        erase_back_match(msg.holder->at(3),MSG_END);
        serv->channels.at(ind)->set_key(msg.holder->at(3));
        msg.res.str("");
        msg.res << ":juluk.org MODE " << msg.holder->at(1) << " +k " << msg.user->get_nickname() << MSG_END;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
    }
    else if (impact == '-')
    {
        msg.res.str("");
        msg.res << ":juluk.org MODE " << msg.holder->at(1) << " -k " << msg.user->get_nickname() << MSG_END;
        send(msg.client_socket, msg.res.str().c_str(), msg.res.str().size(), 0);
    }


}

void l_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag l" << std::endl;
}

void b_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag b" << std::endl;
}

void m_flag(Message &msg, char impact,Server *serv)
{
    std::cout << "Calling flag m" << std::endl;
}


void    Server::flag_manager(Message &msg)
{
    char impact;
    std::string flags;
    std::string compilation = "iwsovqtklbm";
    void (*do_flags[])(Message &msg, char impact,Server *serv) = \
    {i_flag,w_flag,s_flag,o_flag,v_flag,q_flag,t_flag,k_flag,l_flag,b_flag,m_flag};
    flags = msg.holder->at(2);
    try {
        for (int flag = 0; flag < flags.size(); flag++)
        {
            if (flags[flag] == '+' || flags[flag] == '-')
            {
                impact = flags[flag];
                continue;
            }
            for (int i = 0; i < compilation.size(); i++)
            {
                if (flags[flag] == compilation[i])
                {
                    std::cout << "Validated flag : " << flags[flag] << std::endl;
                    do_flags[i](msg, impact, this);
                } 
            }
        }
    } catch (std::exception &e)
    {
        std::cerr << "Error en la validacion de vectores" << std::endl;
    }
    std::cout << "finish" << std::endl;
}