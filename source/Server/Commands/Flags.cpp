#include "Server.hpp"

int get_channel_index(std::vector<Channel *> channels, std::string channel)
{
    for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
        it->get_name();
    }
    return (1);
}


void i_flag(Message &msg,char impact,Server *serv)
{
    
    if(msg.holder->size() < 3)
    {
         std::cout << "Not implemmented" << std::endl;
         return ; 
    }
    std::cout << "Calling flag i" << std::endl;
    std::string channel = msg.holder->at(1);
    std::string user = msg.holder->at(3);
    get_channel_index(serv->channels,channel);



    std::cout <<  serv->users[0] << std::endl;
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
                do_flags[i](msg, impact, this);
        }
    }
}