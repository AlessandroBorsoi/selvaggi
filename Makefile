CC=gcc
CFLAGS+=-Wall -Wextra -std=c11 -pedantic -g "-lpthread"

selvaggi: selvaggi.o

clean:
	rm selvaggi *.o