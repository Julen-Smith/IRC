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


Hacer un Rakefile

## JOIN

  Comando utilizado para crear o escuchar en un canal.

  Condiciones:
    1. El usuario debe ser invitado en caso de ser solo por invitación.
    2. El usuario/canal no debe matchear ningún canal baneado.
    3. La contraseña correcta debe ser proporcionada.

  Errores:

    461 ERR_NEEDMOREPARAMS:
      Parametros insuficientes.
      - Mensaje: "<command> :Not enough parameters".
      
    474 ERR_BANNEDFROMCHAN:
      El usuario está baneado del canal.
      - Mensaje: "<channel> :Cannot join channel (+b)".
      
    ERR_INVITEONLYCHAN
    ERR_BADCHANNELKEY
    ERR_CHANNELISFULL
    ERR_BADCHANMASK
    ERR_NOSUCHCHANNEL
    ERR_TOOMANYCHANNELS
     
     RPL_TOPIC


## OPER

  Comando utilizado por los usuarios para ganar privilegios de operario.

  Prototipo: OPER <user> <password>

    OPER -> comando
    user -> tipo de usuario
    password -> contraseña asociada al tipo de usuario.

  Errores:

    461 ERR_NEEDMOREPARAMS:
      Parametros insuficientes.
      - Mensaje: "<command> :Not enough parameters".

    464 ERR_PASSWDMISMATCH:
      Contraseña incorrecta.
      . Mensaje: ":Password incorrect".

  Respuesta:

    381 RPL_YOUREOPER:
      Ejecución correcta.
      Mensaje: ":You are now an IRC operator".

  Tipos de usuario:

    1. admin
    2. moderador

## MODE

### User

  Tipo:

    + Añadir modo
    - Quitar modo

  Modo:

    i usuario invisible
    s recibe avisos del servidor
    w recibe wallops
    o operador

  Errores:

    461 ERR_NEEDMOREPARAMS:
      Parametros insuficientes.
      - Mensaje: "<command> :Not enough parameters".
    
    482 ERR_CHANOPRIVSNEEDED:
      El usuario no es operador. El comando requiere privilegios 'chanop'.
      - "<channel> :You're not channel operator".

    442 ERR_NOTONCHANNEL:
      El usuario no está en el canal.
      - Mensaje: "<channel> :You're not on that channel".

    472 ERR_UNKNOWNMODE:
      Caracter invalido.
      - Mensaje: "<char> :is unknown mode char to me".

    502 ERR_USERSDONTMATCH:
      El usuario no puede cambiar o ver privilegios de otros.
      - Mensaje: ":Cant change mode for other users"

    501 ERR_UMODEUNKNOWNFLAG:
      Flag desconocido.
      - Mensaje: ":Unknown MODE flag"

    401 ERR_NOSUCHNICK:
      El nickname o canal no existe.
      - Mensaje: "<nickname> :No such nick/channel".

    467 ERR_KEYSET:
      ??
      - Mensaje: "<channel> :Channel key already set".

    403 ERR_NOSUCHCHANNEL:
      El nombre del canal es invalido.
      - Mensaje: "<channel name> :No such channel".      
      
