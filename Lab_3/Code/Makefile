CC = gcc
CFLAGS = -Wall
OBJ = Setup.o Teardown.o udp_transport.o Utils.o
PROGRAMS = Client Server


all: ${PROGRAMS}

client: Client.o $(OBJ)
	$(CC) $(CFLAGS) -pthread -o Client Client.c $(OBJ)

server: Server.o $(OBJ)
	$(CC) $(CFLAGS) -pthread -o Server Server.c $(OBJ)

.PHONY: clean
clean:
	rm -f *.o ${PROGRAMS}

