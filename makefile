CFLAGS=-c -Wall -O2 
LIBS = -loled96 -larmbianio 

all: oe 

oe: main.o 
	$(CC) main.o $(LIBS) -o oe 

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *o oe
