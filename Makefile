CC=gcc

CFLAGS=-O3 -MMD -MP
LDFLAGS=
LIBS=-lm enigma/libenigma.a

APPSRCS=main.c entropy.c

all:app

app:$(APPSRCS:.c=.o)
	make lib -C enigma
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

-include $(APPSRCS:.c=.d)

clean:
	make clean -C enigma
	rm -f app *.o *.d

