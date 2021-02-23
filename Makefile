CC = gcc
CFLAGS = -std=c99 -g

all: master bin_adder

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

master: master.o
	$(CC) $(CFLAGS) $< -o $@ -lm

bin_adder: bin_adder.o
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o master bin_adder
