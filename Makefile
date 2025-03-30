TARGET = sTree
CC = gcc
CFLAGS = -std=c11
LDFLAGS =
SRCS = src/main.c
PREFIX ?= /usr/local

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRCS) -o bin/$(TARGET)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f bin/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	chmod 755 $(DESTDIR)$(PREFIX)/bin/$(TARGET)

unistall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)


.PHONY: all install unistall
