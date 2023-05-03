#include "Channel.hpp"
#include <vector>
#include "User.hpp"


class User;
Channel::Channel(Server *server)
{
    this->room_name = "Lobby";
    this->server = server;
    std::cout << "The channel " << room_name << " has been created." << std::endl;
}

Channel::Channel(std::string room_name, Server *server)
{
    this->room_name = room_name;
    this->server = server;
    std::cout << "The channel " << room_name << " has been created." << std::endl;
}

Channel::~Channel()
{
    std::cout << "The channel " << room_name << " has been deleted." << std::endl;
}

Channel::Channel(Channel& new_channel)
{

}

Channel& Channel::operator=(Channel &new_channel)
{

}

void Channel::Welcome_message()
{
    std::cout << "Welcome to " << this->room_name << std::endl;
}

void Channel::join_channel(std::string buffer) 
{

    this->channel_users.push_back(User());
   // this->channel_users.back().nickname = buffer;
   // std::cout << "User " << this->channel_usersback().getName() << " has join the channel" << std::endl;
}