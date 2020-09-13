CC=gcc
OBJS=main.o anytime.o

main:$(OBJS)
	$(CC) $^ -o $@

main.o:main.c
	$(CC) $^ -c -o $@

anytime.o:anytime.c
	$(CC) anytime.c -c -o anytime.o

clean:
	$(RM) *.o main
