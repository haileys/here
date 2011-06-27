CFLAGS=-g --std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wextra

all:
	gcc $(CFLAGS) -o here -iquote inc/ src/*.c