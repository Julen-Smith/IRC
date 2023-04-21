CC = g++
CFLAGS = -Wall
OBJDIR = obj

all: Server Cliente

Server: $(OBJDIR)/Server.o
	$(CC) $(CFLAGS) -o Server $(OBJDIR)/Server.o 

Cliente: $(OBJDIR)/Cliente.o
	$(CC) $(CFLAGS) -o Cliente $(OBJDIR)/Cliente.o 

$(OBJDIR)/Server.o: source/Server.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c source/Server.cpp -o $(OBJDIR)/Server.o 

$(OBJDIR)/Cliente.o: source/Cliente.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c source/Cliente.cpp -o $(OBJDIR)/Cliente.o

$(OBJDIR)/Cliente.o: source/Cliente.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c source/Cliente.cpp -o $(OBJDIR)/Cliente.o 

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o Server Cliente

fclean:
	make clean