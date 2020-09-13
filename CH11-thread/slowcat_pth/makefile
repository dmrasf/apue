CC=gcc
OBJS=slowcat.o mytbf.o
CFLAGS+=-pthread
LDFLAGS+=-pthread

slowcat:$(OBJS)
	$(CC) $^ $(CFLAGS) -o $@

slowcat.o:slowcat.c
	$(CC) $^ $(CFLAGS) -c -o $@
mytbf.o:mytbf.c
	$(CC) mytbf.c $(CFLAGS) -c -o mytbf.o

clean:
	$(RM) *.o slowcat
