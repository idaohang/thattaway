CC = gcc -Wall -c
LD = gcc -o
SOURCES = coordreader.c
OBJECTS = coordreader.o
BIN = coordreader

all:
	$(CC) $(SOURCES)
	$(LD) $(BIN) $(OBJECTS)

clean:
	rm -rf $(OBJECTS)

realclean: clean
	rm -rf $(BIN)
