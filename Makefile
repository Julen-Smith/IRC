CC				= c++
SERVER			= ircserv
FLAGS			= -Wall -Werror -Wextra -std=c++98 -g3 #-gdwarf-2 #-fsanitize=address -g3


SERVER_SRCS =   source/Server/Server.cpp \
				source/Server/ServerCommandManager.cpp \
				source/Server/ServerUserManager.cpp \
				source/Server/ServerChannelManager.cpp \
				source/Server/ServerUtils.cpp \
			 	source/Server/Message.cpp \
				source/Server/Commands/Mode.cpp \
				source/Server/Commands/Flags.cpp \
			 	source/main.cpp \
			 	source/Channel.cpp \
			 	source/User.cpp


CLIENT_SRCS = source/Cliente.cpp \
			 	source/Channel.cpp  \
			 	source/User.cpp

INC = -I ./inc

SERVER_OBJ = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o)


all : $(SERVER)
#./ircserv 1234 a
	#./ircserv 1234 a

%.o : %.cpp
	${CC} $(INC) ${FLAGS} -c $< -o $@

$(SERVER) : $(SERVER_OBJ)
	$(CC) $(FLAGS) -o $(SERVER) $(SERVER_OBJ)

clean:
	$(RM) $(SERVER_OBJ)
	$(RM) $(CLIENT_OBJ)
	rm -rf .DS_Store
	rm -rf .vscode
	echo "clean done"

fclean: clean
	$(RM) $(CLIENT)
	$(RM) $(SERVER)

re: clean all

.PHONY: all clean fclean re
