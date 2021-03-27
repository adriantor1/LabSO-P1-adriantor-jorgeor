CC=gcc

all: main.o split.o
	$(CC) -o main main.o split.o

split.o: split.c split.h list.h
	$(CC) -c -o split.o split.c

main.o: main.c split.h list.h
	$(CC) -c -o main.o main.c

clean:
	rm -f main.o split.o main
