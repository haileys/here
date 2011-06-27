CFLAGS=-rdynamic -shared -fPIC -g --std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wextra

%: %/*.c
	gcc $(CFLAGS) -o ../here$@.so -iquote . -iquote ../inc/ $@/*.c