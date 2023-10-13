#include <iostream>

#include "Channel.hpp"
#include "User.hpp"


Channel::Channel(const std::string& name, const std::string &topic) 
: _name(name), _topic(topic), _key(""), _invite(false), _user_limit(5)
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
    std::string user_list = "";
    
    for (int i = 0; i < this->_users.size() - 1; i++)
        user_list += this->_users.at(i)->get_nickname() + ",";
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

bool    Channel::get_invite() const { return this->_invite; }
const std::string&  Channel::get_name() const { return (this->_name );}
const std::string&  Channel::get_topic() const { return (this->_topic );}
int Channel::get_users_size() const { return (this->_users.size()); }

//is

bool    Channel::is_limit_raised() const {
    if (this->_users.size() == this->_user_limit; )
}

//void    Channel::send_msg(const std::string &msg, int client)
//{
    //TODO
//}

std::string Channel::get_topic_msg(const User *user) {
    std::stringstream res;

    //TODO hay que notificar al resto de una manera distinta
    res << user->get_nickname() << "!" << user->get_login_name() << "@localhost ";
    res << "JOIN " << this->_name << MSG_END;
    res << RPL_TOPIC << user->get_nickname() << " " << this->_name << ": " << this->get_topic() << MSG_END;
    return res.str();
}

std::string Channel::get_user_list_msg(const User *user) {
    std::stringstream res;

    //TODO comprobar el nivel de privilegios del canal
    res << RPL_NAMREPLY << user->get_nickname() << " = " << this->_name << " :" << this->get_user_list() << MSG_END;
    res << RPL_ENDOFNAMES << ENDOFNAMES << MSG_END;
    return res.str();
}

void    Channel::set_user_limit(int user_limit) { this->_user_limit = user_limit; }
void    Channel::set_key(const std::string &key) { this->_key = key; }
void    Channel::set_invite(const bool invite) { this->_invite = invite; }

User    *Server::add_validated_user(Server::unvalidated_user unva_user) {
    User    *user;

    user = new User(unva_user->second);
    this->users.push_back(user);
    return user;
}