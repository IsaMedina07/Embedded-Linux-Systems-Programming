
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./sensor -I./actuators -I./controller -O2
LDFLAGS =

SRCDIRS = sensor actuators controller
SOURCES = $(wildcard sensor/*.c) $(wildcard actuators/*.c) controller/ctl.c
OBJS = $(SOURCES:.c=.o)

.PHONY: all ctl64 ctl32 clean

all: ctl64

ctl64: CFLAGS += -m64
ctl64: ctl

ctl32: CFLAGS += -m32
ctl32: ctl

ctl: $(OBJS)
	$(CC) $(CFLAGS) -o ctl $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) ctl

