# Assignment 7

This is a firewall wich filters out badspeak and suggests better words.

banhammer.c	Contains the main program

bf.c		Contains bloomfilter ADT

bv.c		Contains bitvector ADT

ht.c		Contains hashtable ADT

ll.c		Contains linkedlist ADT

node.c		Contains node ADT

parser.c	Input parser

speck.c		Hash implementation

The command line arguments when running banhammer:

  -h           Program usage and help.

  -s           Print program statistics.

  -m           Enable move-to-front rule.

  -t size      Specify hash table size (default: 10000).

  -f size      Specify Bloom filter size (default: 2^20).

## Build

	$ make
	$ make all
	$ make banhamer

## Running

	$ ./banhammer -h -s -m -t <size> -f <size>

## Cleaning

	$ make clean
