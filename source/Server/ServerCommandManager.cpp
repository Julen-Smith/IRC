#include "Server.hpp"
#include "defs.hpp"
#include "Channel.hpp"

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


void    Server::LIST(int client)
{
     build_message_and_send("321", client,"Server",USR_RPL, "", 0);       
            for (int channels = 0; channels != this->channels.size(); channels++)
            {
                build_message_and_send("322",client,"Server",USR_RPL,"",
                4,
                this->users.at(client)->get_nickname().c_str(),
                this->channels.at(channels)->get_name().c_str(),
                std::to_string(this->channels.at(channels)->get_users_size()).c_str(),
                this->channels.at(channels)->get_topic().c_str()
                );
            }
    build_message_and_send("323",client,"Server",USR_RPL, "", 0);
}

void    Server::JOIN(int client, int room_index)
{
    std::string nickname = this->users.at(client)->get_nickname(); 
    build_message_and_send("JOIN",client,
    response_cleaner(nickname).c_str(),
    USR_RPL,
    "",1, this->channels[room_index]->get_name().c_str());
    build_message_and_send("332",client,"",SRV_RPL,
    "RPL_TOPIC", 1, this->channels[room_index]->get_name().c_str()," :Bienvenido");
    //hacer función que devuelva usuarios dentro del channel
    build_message_and_send("353",client,"",SRV_RPL,
    "RPL_TOPIC", 1, this->channels[room_index]->get_name().c_str()," :",
    this->channels.at(room_index)->get_user_list().c_str()); 
    build_message_and_send("366",client,"",SRV_RPL,
    "RPL_ENDOFNAMES", 1, this->channels[room_index]->get_name().c_str()," :Fin de la lista de nombres");
    this->channels.at(room_index)->add_user(this->users.at(client));
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
            LIST(client);
        break;
        case 2: //JOIN
            room_index = check_channel(compare);
            if (room_index > 0)
                JOIN(client,room_index);
            else
            {
                std::string channel_name("#");
                channel_name+=compare;
                Channel *new_channel = new Channel(channel_name,"Default topic");
                this->channels.push_back(new_channel);
                JOIN(client,this->channels.size() - 1);
            }
            break;
        default: 
            std::cout << "Comando no encontrado option : "<< opt << std::endl; 
    }
}