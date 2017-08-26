CC     = gcc
CFLAGS = -Wall -pedantic -std=c99 -g

simcpu: main.c schedule.c mode.c simulation.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o simcpu
