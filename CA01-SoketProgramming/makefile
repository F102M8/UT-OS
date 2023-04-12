CC = gcc

SRC = ./src
INC = ./include
all: server client

server: $(SRC)/server.c  $(INC)/const.h
	$(CC) $(SRC)/server.c -o server

client: $(SRC)/client.c $(INC)/const.h
	$(CC) $(SRC)/client.c -o client

PHONY: all clean
clean:
	rm -rf client 
	rm -rf server
	
