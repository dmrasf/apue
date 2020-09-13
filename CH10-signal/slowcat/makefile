CC=gcc
OBJS=slowcat.o mytbf.o

slowcat:$(OBJS)
	$(CC) $^ -o $@

slowcat.o:slowcat.c
	$(CC) $^ -c -o $@

mytbf.o:mytbf.c
	$(CC) mytbf.c -c -o mytbf.o

clean:
	$(RM) *.o slowcat
