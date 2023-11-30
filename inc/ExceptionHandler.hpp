#ifndef EXCEPTIONHANDLER_HPP
#define EXCEPTIONHANDLER_HPP

#include <exception>

class SocketException : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Ha ocurrido un error al inicializar el socket";
    }
};

class BindingException : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Ha ocurrido un error al enlazar el socket";
    }
};

class ListenException : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Ha ocurrido un error en la espera";
    }
};

class MsgAcceptException : public std::exception
{
    public:
    const char* what() const noexcept override
    {
        return "Ha ocurrido un error en la gestión del mensaje";
    }
};

#endif