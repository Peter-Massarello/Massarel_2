CC = gcc
CFLAGS = -std=c99
master: master.c
	$(CC) -o $@ $^ $(CFLAGS)

bin_adder: bin_adder.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f master
