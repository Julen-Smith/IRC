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

    Usuarios :
    +i (Modo invisible): Este modo permite que un usuario se vuelva invisible en la lista de usuarios de un canal o en la red. Otros usuarios no verán la presencia de un usuario invisible en la lista de quienes están en el canal o en la red.

    +w (Modo recibir wallops): El modo +w permite a un usuario recibir mensajes de tipo "wallop". Los wallops son mensajes especiales de información del servidor o de administradores de la red.

    +o (Modo operador): El modo +o generalmente otorga a un usuario permisos de operador en un canal específico. Los operadores pueden realizar ciertas acciones de administración en el canal.

    +s (Modo silencioso): El modo +s generalmente evita que otros usuarios no operadores hablen en el canal. Solo los operadores y aquellos con permisos específicos pueden enviar mensajes en el canal.

    +v (Modo voz): El modo +v otorga a un usuario "voz" en un canal moderado. Los usuarios con voz pueden hablar en el canal, incluso si el canal está moderado.

    +q (Modo de cuarentena): Algunos servidores IRC pueden implementar un modo +q que coloca a un usuario en cuarentena, lo que limita su capacidad para enviar mensajes en el canal.

    Canales :

    +t (Modo de solo temas): Este modo permite que solo los operadores del canal cambien el tema del canal. Los usuarios regulares no pueden modificar el tema.

    +k (Modo de clave): Con el modo de clave activado, se requiere que los usuarios proporcionen una clave para unirse al canal. Esto limita el acceso solo a aquellos que conocen la clave correcta.

    +l (Modo de límite de usuarios): Este modo permite establecer un límite en la cantidad máxima de usuarios que pueden unirse al canal. Por ejemplo, si se establece "+l 10", solo se permitirán hasta 10 usuarios en el canal.

    +b (Modo de ban): Con el modo de ban, los operadores del canal pueden prohibir a usuarios específicos, impidiendo que ingresen al canal.

    +m (Modo moderado): Cuando se activa el modo moderado, solo los usuarios con permisos especiales, como operadores del canal, pueden enviar mensajes en el canal. Los usuarios regulares no pueden hablar hasta que se les otorgue voz o permisos específicos.

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
      
