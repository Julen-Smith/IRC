CC				= clang++
SERVER			= Server
CLIENT			= Client
FLAGS			=  #-Wall -Wextra -Werror -g -fsanitize=address -g3
	
				  	
SERVER_SRCS = source/Server.cpp \
			 source/Channel.cpp \
			 source/User.cpp \
			 source/serverUtils.cpp


CLIENT_SRCS = source/Cliente.cpp \
			 source/Channel.cpp  \
			 source/User.cpp

SERVER_OBJ = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o)


all : $(SERVER) $(CLIENT)
	./Server

%.o : %.cpp
	${CC} ${FLAGS} -c $< -o $@

$(SERVER) : $(SERVER_OBJ)
	$(CC) $(FLAGS) -o $(SERVER) $(SERVER_OBJ)

$(CLIENT) : $(CLIENT_OBJ)
	$(CC) $(FLAGS) -o $(CLIENT) $(CLIENT_OBJ)


clean:
	$(RM) $(SERVER_OBJ)
	$(RM) $(CLIENT_OBJ)
	echo "clean done"

fclean: clean
	$(RM) $(SERVER_OBJ)
	$(RM) $(CLIENT_OBJ)
	rm -rf .DS_Store
	rm -rf .vscode

re: clean all

.PHONY: all clean fclean re
