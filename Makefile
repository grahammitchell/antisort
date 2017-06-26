CC=g++
ARGS=-Wall -s
EXT=

all:
	$(CC) -o antisort$(EXT) $(ARGS) antisort.cpp
