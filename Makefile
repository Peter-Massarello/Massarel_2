CC = gcc
CFLAGS = -std=c99
master: master.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f master
