CFLAGS=-rdynamic -shared -fPIC -g --std=c99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wextra \
		-o ../../heretime.so -iquote ../ -iquote ../../inc/ -Iphp-5.3.6/ -Iphp-5.3.6/main -Iphp-5.3.6/Zend -Iphp-5.3.6/TSRM 

all: php.c heresapi.c  php-5.3.6/libs/libphp5.so
	gcc $(CFLAGS) $^