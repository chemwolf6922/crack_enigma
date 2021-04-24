CC=gcc

CFLAGS=-O3 -MMD -MP
LDFLAGS=
LIBS=-lm

APPSRCS=main.c entropy.c

all:app

app:$(APPSRCS:.c=.o)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

-include $(APPSRCS:.c=.d)

clean:
	rm -f app *.o *.d

