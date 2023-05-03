CC				= clang++
SERVER			= Server
CLIENTE			= Client
FLAGS			=  #-Wall -Wextra -Werror -g -fsanitize=address -g3

SRCS			=	source/Channel.cpp	\
					source/Server.cpp		
				  	
SERVER_SRCS = source/Server.cpp \
			 source/Channel.cpp \

CLIENT_SRCS = source/Cliente.cpp \
			 source/Channel.cpp  \

#OBJS = $(SRCS:.cpp=.o)

SERVER_OBJ = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRCS:.cpp=.o)


all : $(SERVER) $(CLIENT) 

%.o : %.cpp
	${CC} ${FLAGS} -c $< -o $@

$(SERVER) : $(SERVER_OBJ)
	$(CC) $(FLAGS) -o $(SERVER) $(SERVER_OBJ)

$(CLIENT) : $(CLIENT_OBJS)
	$(CC) $(FLAGS) -o $(CLIENTE) $(CLIENT_OBJS)


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
