CFLAGS=-rdynamic -shared -fPIC -g --std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wextra \
		-o ../../heretime.so -iquote ../ -iquote ../../inc/

all: time.c
	gcc $(CFLAGS) $^