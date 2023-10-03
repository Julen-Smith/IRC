#include <iostream>

#include "User.hpp"

User::User(const std::string &nickname, int socket) : _nickname(nickname), _notices(false), _socket(socket)
{
    std::cout << "Nickname: " << nickname << std::endl;
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
    std::cout << " - User " << this->_nickname << " has been deleted." << std::endl;
}

User& User::operator=(User &user)
{
     std::cout << "Operator placeholder" << std::endl;
     return *this;
}

const std::string&  User::get_nickname(void) const {return this->_nickname;}
bool                User::get_notices(void) const {return this->_notices;}
int                User::get_socket(void) const {return this->_socket;}

void                User::set_notices() {this->_notices = true; }
void                User::set_nickname(std::string nick) { this->_nickname = nick; }
