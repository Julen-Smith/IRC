#include "User.hpp"

User::User()
{

}


User::User(char * nickname, Channel &channel)
{
    std::cout << "Nickname, channel constructor" << std::endl;
}

User::User(Channel &channel)
{   
    this->rooms = &channel;
    //Cambiar más adelante
    std::cout << "Guest has join the channel [ " << "Lobby"<< " ]" << std::endl;
}

User::User(std::string nickname)
{
    // this->rooms = &channel;
    this->nickname = nickname;
    //Cambiar más adelante
}

User::~User()
{
    std::cout << "User has been deleted." << std::endl;
}

User::User(User& user)
{
    std::cout << "Constructor placeholder" << std::endl;
}

User& User::operator=(User &user)
{
     std::cout << "Operator placeholder" << std::endl;
}

std::string& User::getName(void)
{
    return this->nickname;
}
