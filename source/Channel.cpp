#include "Channel.hpp"

Channel::Channel(Server *server)
{
    this->room_name = "Lobby";
    this->server = server;
    std::cout << "The channel " << room_name << "has been created.";
}

Channel::Channel(std::string room_name, Server *server)
{
    this->room_name = room_name;
    this->server = server;
    std::cout << "The channel " << room_name << "has been created.";
}

Channel::~Channel()
{
    std::cout << "The channel " << room_name << "has been deleted.";
}

Channel::Channel(Channel& new_channel)
{
    
}

Channel& Channel::operator=(Channel &new_channel)
{

}

void Channel::Welcome_message()
{

}