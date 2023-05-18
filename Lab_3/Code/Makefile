CC = gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
OBJ = $(addprefix $(BUILDDIR)/,Setup.o Teardown.o udp_transport.o Utils.o)
PROGRAMS = client server
BUILDDIR = build

all: directories ${PROGRAMS}

client: $(BUILDDIR)/client.o $(OBJ)
	$(CC) $(CFLAGS) -pthread -o $(BUILDDIR)/client client.c $(OBJ)

server: $(BUILDDIR)/server.o $(OBJ)
	$(CC) $(CFLAGS) -pthread -o $(BUILDDIR)/server server.c $(OBJ)
$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean directories
clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/${PROGRAMS}

directories: 
	mkdir -p $(BUILDDIR)

