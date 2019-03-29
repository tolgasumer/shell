CC=gcc
CFLAGS=-lpthread

shell: shell.c
	$(CC) -o shell $(CFLAGS) shell.c

clean:
	$(RM) shell 
run:
	./shell