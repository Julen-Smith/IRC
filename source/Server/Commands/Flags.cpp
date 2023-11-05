#include "Server.hpp"

int get_channel_index(std::vector<Channel *> channels, std::string channel)
{
    for (int i = 0; i < channels.size(); i++) {
        if(channels.at(i)->get_name() == channel)
            return (i);
    }
    return (1);
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
    if(msg.holder->size() == 3)
    {
         std::cout << "Not implemmented" << std::endl;
         return ; 
    }
    std::string channel = msg.holder->at(1);
    std::string user = msg.holder->at(3);
    int index = get_channel_index(serv->channels,channel);
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
    std::cout << permissions[0] << std::endl;
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
        for (int i = 0; i < serv->channels.at(index)->get_users().size(); i++)
        {
            std::map<const User*, std::vector<char> >::iterator iter;
            iter = serv->channels.at(index)->get_user_permissions()->find(serv->channels.at(index)->get_users().at(i));
            std::vector<char>& perm = iter->second;
           if (perm[0] != '1')
           {
             std::cout << "Añadido a usuario visible" << serv->channels.at(index)->get_users().at(i)->get_nickname() << std::endl;
             serv->channels.at(index)->get_visible_users().push_back(serv->channels.at(index)->get_users().at(i));
           }      
        }
        std::cout << "Size " << serv->channels.at(index)->get_visible_users().size() << std::endl;
        for (int i = 0; i < serv->channels.at(index)->get_visible_users().size(); i++)
        {
            std::cout << "Enviado la reply a " << serv->channels.at(index)->get_visible_users().at(i)->get_nickname() << std::endl;
            msg.res.str("");
            msg.res << RPL_NAMREPLY << serv->channels.at(index)->get_visible_users().at(i)->get_nickname() << " = " << channel << " : " << "" << MSG_END;
            msg.res << RPL_ENDOFNAMES << socket_to_nick << " " << channel << ENDOFNAMES;
        }

    }


  //  msg.res.str("");
    
   // send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
  //  msg.res.str("");
  //  
  //  send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
    //Necesito volver a enviar la lista de usuarios para el canal a todos los usuarios en base a las flags
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
    std::cout << "Calling flag o" << std::endl;
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
    std::cout << "Calling flag t" << std::endl;
}

void k_flag(Message &msg,char impact,Server *serv)
{
    std::cout << "Calling flag k" << std::endl;
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

    std::cout << "start" << std::endl;
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
    std::cout << "finish" << std::endl;
}