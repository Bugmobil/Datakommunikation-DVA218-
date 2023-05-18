CC = gcc
CFLAGS = -Wall
OBJ = Setup.o Teardown.o udp_transport.o Utils.o
PROGRAMS = client server


all: ${PROGRAMS}

client: Client.o $(OBJ)
	$(CC) $(CFLAGS) -pthread -o client Client.c $(OBJ)

server: Server.o $(OBJ)
	$(CC) $(CFLAGS) -pthread -o server Server.c $(OBJ)

.PHONY: clean
clean:
	rm -f  *.o ${PROGRAMS}