CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pedantic -g
LIBS=-lpthread

all: selvaggi

selvaggi: selvaggi.o
	@echo "linking..."
	$(CC) $(CFLAGS) -o selvaggi selvaggi.o $(LIBS)

clean:
	rm -f selvaggi *.o