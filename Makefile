SRCS   = $(wildcard *.c)
PROGS  = $(patsubst %.c,%,$(SRCS))
CC     = gcc
CFLAGS = -Wall -g

.PHONY: all clean
all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS)  -o $@ $<

clean:
	rm -f $(PROGS)
