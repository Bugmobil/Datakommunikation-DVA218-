CC = gcc
CFLAGS = -Wall -Wextra -Werror
 OBJ = Setup.o Teardown.o udp_transport.o Utils.o

all: client server

client: Client.o $(OBJ)
	$(CC) $(CFLAGS) -o client Client.o $(OBJ)

server: Server.o $(OBJ)
	$(CC) $(CFLAGS) -o server Server.o $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o client server
