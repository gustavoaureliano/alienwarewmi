CC=gcc
CFLAGS= -Wall -Werror -pedantic -g -fsanitize=address,undefined

all: inspiron-cli

inspiron-cli: inspiron-cli.o libalienwarewmi.o
	$(CC) $(CFLAGS) $^ -o $@

inspiron-cli.o: inspiron-cli.c
	$(CC) $(CFLAGS) -c inspiron-cli.c -o $@

libalienwarewmi.o: libalienwarewmi.c libalienwarewmi.h
	$(CC) $(CFLAGS) -c libalienwarewmi.c -o $@

clean:
	rm inspiron-cli inspiron-cli.o libalienwarewmi.o
