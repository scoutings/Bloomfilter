CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: banhammer

banhammer: speck.o ht.o ll.o node.o bf.o bv.o parser.o banhammer.o
	$(CC) $(CFLAGS) -o banhammer banhammer.o speck.o ht.o ll.o node.o bf.o bv.o parser.o

speck.o: speck.c
	$(CC) $(CFLAGS) -c speck.c

ht.o: ht.c
	$(CC) $(CFLAGS) -c ht.c

ll.o: ll.c
	$(CC) $(CFLAGS) -c ll.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

bf.o: bf.c
	$(CC) $(CFLAGS) -c bf.c

bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

banhammer.o: banhammer.c
	$(CC) $(CFLAGS) -c banhammer.c

clean:
	rm -f *.o banhammer

format:
	clang-format -i -style=file *.h *.c
