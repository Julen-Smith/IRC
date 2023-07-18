#include <iostream>

#include "Channel.hpp"
#include "User.hpp"


Channel::Channel(const std::string& name, const std::string &topic) : _name(name), _topic(" :" + topic)
{
    std::cout << "The channel " << name << " has been created." << std::endl;
}

Channel::~Channel()
{
    std::cout << "The channel " << this->_name << " has been deleted." << std::endl;
}

Channel& Channel::operator=(const Channel &new_channel)
{
    return *this;
}

void Channel::welcome_msg() const
{
    std::cout << "Welcome to " << this->_name << std::endl;
}

Channel::Channel(){};

void Channel::join_channel(std::string buffer, User &user)
{
   // this->channel_users.push_back(&user);
   // this->channel_users.back().nickname = buffer;
   // std::cout << "User " << this->channel_users.back()->getName() << " has join the channel" << std::endl;
}

std::string Channel::get_user_list() const
{
    std::string user_list;
    
    for (int i = 0; i < this->_users.size() - 1; i++)
        user_list += this->_users.at(i)->get_nickname() + " ";
    return (user_list);
}

void    Channel::add_user(const User *user)
{
    this->_users.push_back(user);
}

void    Channel::delete_user(const std::string &name)
{
    for (std::vector<const User *>::iterator it = this->_users.begin(); it != this->_users.end();)
    {
        if ((*it)->get_nickname() == name)
        {
            this->_users.erase(it);
            return ;
        }
        it++;
    }
}

const std::string&  Channel::get_name() const { return (this->_name );}
const std::string&  Channel::get_topic() const { return (this->_topic );}
int Channel::get_users_size() const { return (this->_users.size()); }

//void    Channel::send_msg(const std::string &msg, int client)
//{
    //TODO
//}