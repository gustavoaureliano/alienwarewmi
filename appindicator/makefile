CC=gcc
# CFLAGS= -Wall -Werror -pedantic -g -fsanitize=address,undefined
CFLAGS= -Wall -g
# GTK_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
# GTK_LIBS=$(shell pkg-config --libs gtk+-3.0)
APPINDICATOR_CFLAGS=$(shell pkg-config --cflags appindicator3-0.1)
APPINDICATOR_LIBS=$(shell pkg-config --libs appindicator3-0.1)


all: inspiron-applet

inspiron-applet: inspiron-applet.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(GTK_LIBS) $(APPINDICATOR_CFLAGS) $(APPINDICATOR_LIBS) $^ -o $@

clean:
	rm inspiron-applet
