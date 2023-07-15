#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"

void Server::generate_default_channels()
{
    Channel * lobby = new Channel("#Lobby","Sala principal del servidor");
    Channel * lukas = new Channel("#La_Guarida_de_Lukas","El mismo la ha denominado asi quien sabe que podrias encontrarte dentro.");
    Channel * test_room = new Channel("#Test_Room","Todas las salas son de prueba, pero ponerle un nombre a una hace que lo sea mas.");
    Channel * AAAA = new Channel("#AAAA","Asii es, AAAA.");

    this->channels.push_back(lobby);
    this->channels.push_back(lukas);
    this->channels.push_back(AAAA);
    this->channels.push_back(test_room);
}

int Server::check_channel(std::string& validation)
{
    int room_index = -1;

    validation = response_cleaner(validation.erase(0,4));
    for(int i = 0; i < this->channels.size(); i++)
    {
        std::string placeholder(this->channels[i]->get_name());
        placeholder = response_cleaner(placeholder);
        std::cout << validation <<" : "<< placeholder << std::endl;
        if (validation == placeholder)
            return (i);
    }
    return room_index;
}