#include <iostream>

#include "../inc/Channel.hpp"
#include "../inc/User.hpp"


Channel::Channel(const std::string& room_name /*, const Server &server*/) //: server(server)
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

void Channel::join_channel(std::string buffer, User &user)
{
   // this->channel_users.push_back(&user);
   // this->channel_users.back().nickname = buffer;
   // std::cout << "User " << this->channel_users.back()->getName() << " has join the channel" << std::endl;
}
