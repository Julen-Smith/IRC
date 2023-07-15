#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"


void    Server::build_message_and_send(std::string code,int client,int nbr_strings, ...)
{   
    va_list args;
    va_start(args, nbr_strings);
    std::string builder = ":Server " + code + " ";
    for (int i = 0; i < nbr_strings; ++i)
    {
        std::string str = va_arg(args, const char*);
        builder += str + " ";
    }
    builder += MSG_END;
    send(fds[client].fd,builder.c_str(),builder.size(),0);
    va_end(args); 
}

int Server::command_checker(std::string &cmd)
{
    //Aqui sería implementar la parte de maps de lukitas
    std::string commands[5]=
    {
        "INVITE",
        "NAMES",
        "MODE",
        "LIST",
        "JOIN",
    };
    cmd = response_cleaner(cmd);
    if (cmd == "LIST")
        return (1);
    if (contains(cmd,"JOIN"))
        return (2);
    return (0);
}

std::string& Server::response_cleaner(std::string& command)
{
    command.erase(remove(command.begin(), command.end(), '\r'), command.end());
    command.erase(remove(command.begin(), command.end(), '\n'), command.end());
    command.erase(remove(command.begin(), command.end(), ' '), command.end());
    command.erase(remove(command.begin(), command.end(), '#'), command.end());  
    return (command);
}


void    Server::manage_response(int client)
{
    ssize_t             rd_size;
    std::stringstream   client_stream;
    std::string         client_msg;
    int                 room_index;


    rd_size = recv(this->fds[client].fd, this->buffer, BUFFER_SIZE, 0);
    if (rd_size == -1)
        return ;
    this->buffer[rd_size] = 0;
    std::string compare(this->buffer);
    int opt = command_checker(compare);
    switch(opt)
    {
        case 1: //LIST
            build_message_and_send("321", client, 0);       
            for (int channels = 0; channels != this->channels.size(); channels++)
            {
                build_message_and_send("322",client,4,
                this->users.at(client)->get_nickname().c_str(),
                this->channels.at(channels)->get_name().c_str(),
                std::to_string(this->channels.at(channels)->get_users_size()).c_str(),
                this->channels.at(channels)->get_topic().c_str()
                );
            }
            build_message_and_send("323",client,0);
        break;
        case 2: //JOIN
            //Aqui debería haber dos casos? Existe channel y no existe? Se crea uno nuevo si no?
            room_index = check_channel(compare);
            std::cout << "Room index " << room_index << std::endl;
            if (room_index > 0)
            {
                std::cout << this->users.at(client)->get_nickname() << " is entering "
                << this->channels[room_index]->get_name() << std::endl;
                build_message_and_send("353",client,1,
                "patty = #irctoast :@patty");
                build_message_and_send("354",client,1,
                "patty = #irctoast :@patty");
                build_message_and_send("366",client,1,"End of /NAMES list");
            }
            else
                std::cout<< "Channel no existe"  << std::endl;






            break;
        /*  
            DISPLAY LIST O NICKNAMES IN THE RIGHT SECTION OF LIME
            respuesta1 = :Server
                    353     RPL_NAMREPLY "<canal> :[[@|+]<nick> [[@|+]<nick> [...]]]"
                    366     RPL_ENDOFNAMES "<canal> :Fin de la lista /NAMES"   
        */
       case 3:
            std::cout << "Case 3 placeholder" << std::endl;
            break;
            /*
                CHANNEL CONFIGS
            */
        case 4:
            std::cout << "Case 4 placeholder" << std::endl;
            break;
            /*
                MODE 
            */

        default: 
            std::cout << "Comando no encontrado option : "<< opt << std::endl; 
    }
}