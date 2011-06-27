CFLAGS=-ldl -g --std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wmissing-prototypes -Wextra

all:
	gcc $(CFLAGS) -o here -iquote inc/ src/*.c src/platform/_autodetect.c