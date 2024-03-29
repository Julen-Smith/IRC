#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"

void Server::generate_default_channels()
{
    Channel * lobby = new Channel("#Lobby","Sala principal del servidor");
    Channel * lukas = new Channel("#La_Guarida_de_Lukas","El mismo la ha denominado asi quien sabe que podrias encontrarte dentro.");
    Channel * test_room = new Channel("#Test_Room","Todas las salas son de prueba pero ponerle un nombre a una hace que lo sea mas.");
    Channel * AAAA = new Channel("#AAAA","Asii es AAAA.");
    Channel * LockedChannel = new Channel("#locked","locked room access denied");
    Channel * inviteOnly = new Channel("#invite","invite only channel");

    LockedChannel->set_key("admin");
    inviteOnly->set_invite(INVITE_ONLY);
    ////test_room->set_user_ban("nicknametest");
    this->channels.push_back(lobby);
    this->channels.at(0)->add_user(new User("Dummy1",999));
    this->channels.at(0)->add_user(new User("Dummy2",998));
    this->channels.at(0)->add_user(new User("Dummy3",997));
    this->channels.at(0)->add_user(new User("Dummy4",996));
    this->channels.push_back(lukas);
    this->channels.at(1)->add_user(new User("Dummy1",995));
    this->channels.at(1)->add_user(new User("Dummy2",994));
    this->channels.at(1)->add_user(new User("Dummy3",993));
    this->channels.at(1)->add_user(new User("Dummy4",992));
    this->channels.push_back(AAAA);
    this->channels.at(2)->add_user(new User("Dummy1",991));
    this->channels.at(2)->add_user(new User("Dummy2",990));
    this->channels.at(2)->add_user(new User("Dummy3",989));
    this->channels.at(2)->add_user(new User("Dummy4",988));
    this->channels.push_back(test_room);
    this->channels.at(3)->add_user(new User("Dummy1",987));
    this->channels.at(3)->add_user(new User("Dummy2",986));
    this->channels.at(3)->add_user(new User("Dummy3",985));
    this->channels.at(3)->add_user(new User("Dummy4",984));
    this->channels.push_back(LockedChannel);
    this->channels.push_back(inviteOnly);
}

Channel *Server::get_channel_by_name(const std::string& name)
{
    std::vector<Channel *>::iterator   it;

    it = this->channels.begin();
    for (; it != this->channels.end(); it++)
    {
        if ((*it)->get_name() == name)
            return (*it);
    }
    return NULL;
}

int Server::check_channel(std::string& validation)
{
    //erase_match(validation, MSG_END_SPACE);
    for(size_t i = 0; i < this->channels.size(); i++)
    {
        std::string placeholder(this->channels[i]->get_name());
        if (validation == placeholder)
            return (i);
    }
    return BAD_ROOM;
}
