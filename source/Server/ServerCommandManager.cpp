#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"
#include <string.h>

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

    std::cout << "Comando: " << cmd << " \nLongitud del comnado: " << strlen(cmd.c_str());
    return (0);
}

std::string& Server::response_cleaner(std::string& command)
{
    //command.erase(remove(command.begin(), command.end(), '\r'), command.end());
    //command.erase(remove(command.begin(), command.end(), '\n'), command.end());
    //command.erase(remove(command.begin(), command.end(), ' '), command.end());
    //command.erase(remove(command.begin(), command.end(), '#'), command.end());  
    
    //TODO check command length is != 0
    
    std::cout << "Command start: " << command << " Command length: " << strlen(command.c_str()) << std::endl; 
    std::string::reverse_iterator   it;
    for (it = command.rbegin(); it != command.rend(); it++) {
        if (*it == '\r' || *it == '\n')
            command.pop_back();
        else
            break ;
    }
    std::cout << "Command after: " << command << "Command length: " << strlen(command.c_str()) << std::endl;
    return (command);
}

void    Server::build_message_and_send(std::string code,int client,std::string user, std::string reply_mode,
std::string reply, int nbr_strings, ...)
{   
    va_list args;
    std::string builder = "";

    va_start(args, nbr_strings);
    if (reply_mode == USR_RPL)
        builder = ":" + user + " " + code + " ";
    else if (reply_mode == SRV_RPL)
        builder = ":"+ code + " " + reply + " ";
    for (int i = 0; i < nbr_strings; ++i)
    {
        std::string str = va_arg(args, const char*);
        builder += str + " ";
    }
    builder += MSG_END;
    send(fds[client].fd,builder.c_str(),builder.size(),0);
    va_end(args); 
}


void    Server::LIST(int clientIndex)
{
     build_message_and_send("321", clientIndex,"Server",USR_RPL, "", 0);       
            for (int channels = 0; channels != this->channels.size(); channels++)
            {
                build_message_and_send("322",clientIndex,"Server",USR_RPL,"",
                4,
                this->users.at(clientIndex)->get_nickname().c_str(),
                this->channels.at(channels)->get_name().c_str(),
                //std::to_string(this->channels.at(channels)->get_users_size()).c_str(),
                this->channels.at(channels)->get_topic().c_str()
                );
            }
    build_message_and_send("323",clientIndex,"Server",USR_RPL, "", 0);
}

void    Server::JOIN(int clientIndex, int room_index)
{
    std::string nickname = this->users.at(clientIndex)->get_nickname(); 
    build_message_and_send("JOIN",clientIndex,
    response_cleaner(nickname).c_str(),
    USR_RPL,
    "",1, this->channels[room_index]->get_name().c_str());
    build_message_and_send("332",clientIndex,"",SRV_RPL,
    "RPL_TOPIC", 1, this->channels[room_index]->get_name().c_str()," :Bienvenido");
    //hacer función que devuelva usuarios dentro del channel
    build_message_and_send("353",clientIndex,"",SRV_RPL,
    "RPL_TOPIC", 1, this->channels[room_index]->get_name().c_str()," :",
    this->channels.at(room_index)->get_user_list().c_str()); 
    build_message_and_send("366",clientIndex,"",SRV_RPL,
    "RPL_ENDOFNAMES", 1, this->channels[room_index]->get_name().c_str()," :Fin de la lista de nombres");
    this->channels.at(room_index)->add_user(this->users.at(clientIndex));
}

void    Server::manage_response(int clientIndex)
{
    char                buffer[BUFFER_SIZE];
    ssize_t             rd_size;
    std::stringstream   client_stream;
    std::string         client_msg;
    int                 room_index;

    //recibir el mensaje de un usuario
    rd_size = recv(this->fds[clientIndex].fd, buffer, BUFFER_SIZE, 0);
    if (rd_size == -1)
        return ;
    buffer[rd_size] = 0;

    std::string compare(buffer);
    int opt = command_checker(compare);
    switch(opt)
    {
        case 1: //LIST
            LIST(clientIndex);
        break;
        case 2: //JOIN
            room_index = check_channel(compare);
            if (room_index > 0)
                JOIN(clientIndex,room_index);
            else
            {
                std::string channel_name("#");
                channel_name+=compare;
                Channel *new_channel = new Channel(channel_name,"Default topic");
                this->channels.push_back(new_channel);
                JOIN(clientIndex,this->channels.size() - 1);
            }
            break;
        default: 
            std::cout << "Comando no encontrado option : "<< opt << std::endl; 
    }
}
