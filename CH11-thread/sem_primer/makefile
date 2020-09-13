CC=gcc
OBJS=main.o mysem.o
CFLAGS+=-pthread
LDFLAGS+=-pthread

sem_primer:$(OBJS)
	$(CC) $^ $(CFLAGS) -o $@

mysem.o:mysem.c
	$(CC) $^ $(CFLAGS) -c -o $@
main.o:main.c
	$(CC) $^ $(CFLAGS) -c -o $@

clean:
	$(RM) *.o sem_primer
