CC=gcc
OTHERFLAGs=-O3
CFLAGS=-Wall -Werror -ggdb3 -std=gnu99 -pedantic $(OTHERFLAGS)

poker: cards.c  deck.c  eval.c  future.c  input.c  main.c $(DEPS)
		gcc -o $@ $(CFLAGS) $^
clean:
		rm -rf *.o poker *~

