# IRC
Program name : ircserv

Turn in files Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp, an optional configuration file

Makefile NAME, all, clean, fclean, re

Arguments port: The listening port

password: The connection password

External functs. Everything in C++ 98.

socket, setsockopt, getsockname, getprotobyname,
gethostbyname, getaddrinfo, freeaddrinfo, bind,
connect, listen, accept, htons, htonl, ntohs,
ntohl, inet_addr, inet_ntoa, send, recv, signal,
lseek, fstat, fcntl, poll (or equivalent)

Libft authorized n/a
Description An IRC server in C++ 98


Idea principal de gestión de proyecto :

Sólo son objetos las partes replicables que se diferencian.
Todos los objetos heredan o implementan. 
Todos los strings del programa están indexados en algún fichero de configuración.
Errores de preprocesado. Por ejemplo al introducir mal los parametros.

Servidor - Rooms/Channels - Comandos por channel, Perfiles de usuarios especiales Admin/Bot - File transfer - UTF8
