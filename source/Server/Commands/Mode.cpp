#include "Server.hpp"

static bool contains_invalid_mix(const std::string& input) {
    bool user = false;
    bool channel = false;

    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (std::string("tklbm").find(c) != std::string::npos) {
            user = true;
            if (channel)
                return true;
        } else if (std::string("iwsovq").find(c) != std::string::npos) {
            channel = true;
            if (user)
                return true;
        }else
            user = channel = false;
    }
    return false;
}

bool erase_back_match(std::string &source, const std::string &to_erase) {

    size_t  erase_len = 0;
    size_t  source_len = source.size();
    
    for (int i = 0; i < to_erase.size(); i++) {
        if (source.find(to_erase[i]) != -1)
            erase_len++;
    }
    if (erase_len)
    {
        source.erase(source_len - erase_len, source_len);
        return true;
    }
    return false;        
}

static bool is_repeated_char(const std::string& input) {
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '+' || c == '-')
            continue;
        for (size_t j = i + 1; j < input.length(); j++) {
            char d = input[j];
            if (d == '+' || d == '-') 
                continue;
            if (c == d) 
                return true;
        }
    }
    return false;
}

static bool twisted_input_validator(std::string input)
{
    bool isOperator = false;
    
    erase_back_match(input,MSG_END);
    if (input[0] != '+' || input[0] != '-' || input.size() < 2)
        return true;
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        std::cout << "Caracter : " << c << std::endl;
        if (c == '+' || c == '-') {
           isOperator = true;
        } else if (std::string("iwsovqtklbm").find(c) == std::string::npos && c != 'i') {
            return true;
        } else if (isOperator && (c != '+' && c != '-')) {
            isOperator = false;
        }
    }
    return (false);
}

bool is_duplicated_simbol(const std::string& input) {
    char last_char = '\0';

    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '+' || c == '-') {
            if (c == last_char) {
                return true;
            }
        }
        last_char = c;
    }
    return false;
}

static bool input_validator(std::string input)
{
    erase_back_match(input,MSG_END);
    if (input[0] == '+' || input[0] == '-' || input.size() < 2)
    {
        for (size_t i = 1; i < input.length(); i++) {
            char c = input[i];
            if (std::string("iwsovqtklbm").find(c) == std::string::npos)
                return true;
        }
    }else
        return true; 
    return (false);
}

static bool check_application_dest(Message &msg, std::vector<User *> users, std::vector<Channel *> channels)
{
    bool isNone = false;
    bool isUsercheck = false;

    erase_back_match(msg.holder->at(1),MSG_END); 
    for (std::vector<User *>::iterator it = users.begin(); it != users.end(); ++it) {
        std::cout <<" Comparation : 1 " << (*it)->get_nickname() <<" : " << msg.holder->at(1) << std::endl; 
        if ((*it)->get_nickname() == msg.holder->at(1))
        {
             isNone = true;
             isUsercheck = true;
        }         
    }
    for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
        std::cout <<" Comparation : 2" << (*it)->get_name() <<" : " << msg.holder->at(1) << std::endl; 
        if ((*it)->get_name() == msg.holder->at(1))
                isNone = true;
    }
    if (msg.holder->size() == 4)
    {
        erase_back_match(msg.holder->at(3),MSG_END);
        for (std::vector<User *>::iterator it = users.begin(); it != users.end(); ++it) {
            std::cout <<" Comparation : 3" << (*it)->get_nickname() << " : " << msg.holder->at(3) << std::endl; 
            if ((*it)->get_nickname() == msg.holder->at(3) && !isUsercheck)
                isNone = true;
        }
    }
    if (!isNone)
        return (true);
    return (false);
}


static void reply_channel_permissions(Message &msg, std::vector<Channel *> channels)
{
    erase_back_match(msg.holder->at(1),MSG_END); 
    for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
        std::cout <<" Comparation : 2" << (*it)->get_name() <<" : " << msg.holder->at(1) << std::endl; 
        if ((*it)->get_name() == msg.holder->at(1))
        {
            msg.res.str("");
            msg.res << RPL_CHANNELMODEIS << " :" \
            << (*it)->get_name()<<"[" <<
            (*it)->get_permissions_to_string() << "]" << MSG_END;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
            return ;
        }
    }
    msg.res.str("");
    msg.res << ERR_NOSUCHNICK << NOSUCHNICK;
    send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
}

int mode_input_manager(Message &msg, std::vector<User *> users, std::vector<Channel *> channels)
{
    msg.holder = msg.split(msg.buffer," ");

    if (msg.holder->size() == 2)
        reply_channel_permissions(msg,channels);
    else if (msg.holder->empty() || msg.holder->size() != 3 || msg.holder->size() > 4 || msg.holder->at(2).empty() ||  is_repeated_char(msg.holder->at(2)) || is_duplicated_simbol(msg.holder->at(2)) ||
    (input_validator(msg.holder->at(2)) && twisted_input_validator(msg.holder->at(2))) || contains_invalid_mix(msg.holder->at(2))){
        msg.res.str("");
        msg.res << ERR_UMODEUNKNOWNFLAG << UMODEUNKNOWNFLAG;
        send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
        std::cout << "Error de comando en comprobación basica" << std::endl;
        return 0;
    }
    else
    {   
        if (check_application_dest(msg,users,channels))
        {
            msg.res.str("");
            msg.res << ERR_NOSUCHNICK << NOSUCHNICK;
            send(msg.client_socket, msg.get_res_str(), msg.get_res_size(), 0);
            std::cout << "Error de comando en comprobación canal/user" << std::endl;
            return 0;
        }
        else
        {
            
            std::cout << "Input validated" << std::endl;
        }
        //Modificar las flags introducidas a true
        //Hacer las modificaciones pertinentes para cada flag
    }

    return (1);
}

#pragma message("En la parte de los modes generaba errores extraños a la hora de incluirle el header, pendiente de modificarlo.")
#pragma message("No debería haber tantas funciones en un fichero")
void    Server::mode_command(Message &msg)
{
   
    if (mode_input_manager(msg, this->users, this->channels))
        return ;
    //Gestionar comandos.

}
