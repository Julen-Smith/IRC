#include <iostream>

#include "User.hpp"

User::User(const std::string &nickname, const pollfd fd) _nickname(nickname), _fd(fd)
{
    std::cout << "User(nickname,channel) constructor" << std::endl;
}

User::User(const User &obj) // : rooms(obj.rooms)
{
    //Cambiar más adelante
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

const std::string& User::getName(void) const
{
    return this->nickname;
}
