#include <iostream>

#include "Channel.hpp"
#include "User.hpp"

Channel::Channel(const Server &server) : server(server)
{
    this->room_name = "Lobby";
    std::cout << "The channel " << room_name << " has been created." << std::endl;
}

Channel::Channel(const std::string& room_name, const Server &server) : server(server)
{
    this->room_name = room_name;
    std::cout << "The channel " << room_name << " has been created." << std::endl;
}

Channel::~Channel()
{
    std::cout << "The channel " << room_name << " has been deleted." << std::endl;
}

Channel& Channel::operator=(const Channel &new_channel)
{
    return *this;
}

void Channel::Welcome_message() const
{
    std::cout << "Welcome to " << this->room_name << std::endl;
}

void Channel::join_channel(std::string buffer)
{
    User    *tmp;

    tmp = new User(buffer);

    this->channel_users.push_back(tmp);
   // this->channel_users.back().nickname = buffer;
   // std::cout << "User " << this->channel_usersback().getName() << " has join the channel" << std::endl;
}
