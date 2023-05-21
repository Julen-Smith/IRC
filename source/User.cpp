#include <iostream>

#include "User.hpp"

User::User(const std::string &nickname) : _nickname(nickname), _notices(false)
{
    std::cout << "User(nickname,channel) constructor" << std::endl;
}

User::User(const User &obj) // : rooms(obj.rooms)
{
    //Cambiar más adelante
    this->_notices = obj.get_notices();
    this->_nickname = obj.get_nickname();
    std::cout << "Guest has join the channel [ " << "Lobby"<< " ]" << std::endl;
}

User::~User()
{
    std::cout << "User has been deleted." << std::endl;
}

User& User::operator=(User &user)
{
     std::cout << "Operator placeholder" << std::endl;
     return *this;
}

void    User::set_pollfd(int socket, int events)
{
    this->fd = socket;
    this->events = events;
}

const std::string&  User::get_nickname(void) const {return this->_nickname;}
bool                User::get_notices(void) const {return this->_notices;}

void                User::set_notices() {this->_notices = true; }
