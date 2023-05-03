CC				= clang++
SERVER			= Server
CLIENT			= Client
FLAGS			=  #-Wall -Wextra -Werror -g -fsanitize=address -g3


SERVER_SRCS = source/Server.cpp \
			 source/Channel.cpp \
			 source/User.cpp


CLIENT_SRCS = source/Cliente.cpp \
			 source/Channel.cpp  \
			 source/User.cpp

INC = -I ./inc

SERVER_OBJ = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o)


all : $(SERVER) $(CLIENT)

%.o : %.cpp
	${CC} $(INC) ${FLAGS} -c $< -o $@

$(SERVER) : $(SERVER_OBJ)
	$(CC) $(FLAGS) -o $(SERVER) $(SERVER_OBJ)

$(CLIENT) : $(CLIENT_OBJ)
	$(CC) $(FLAGS) -o $(CLIENT) $(CLIENT_OBJ)


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
