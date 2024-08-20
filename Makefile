TARGET=mybash
CC=gcc
CPPFLAGS=`pkg-config --cflags glib-2.0`
CFLAGS=-std=gnu11 -Wall -Wextra -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes -Wno-unused-parameter -Werror -g -pedantic
LDFLAGS=`pkg-config --libs glib-2.0`

# Propagar entorno a make en tests/
export CC CPPFLAGS CFLAGS LDFLAGS

SOURCES=$(shell echo *.c)
SOURCES := $(filter-out obfuscated.c, $(SOURCES))
OBJECTS=$(SOURCES:.c=.o)
PRECOMPILED=parser.o lexer.o

# Agregar objects-arch a los directorios de busqueda de los .o precompilados
ARCHDIR=objects-$(shell uname -m)
vpath parser.o $(ARCHDIR)
vpath lexer.o $(ARCHDIR)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(PRECOMPILED) obfuscated.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

obfuscated.o: obfuscated.c
	$(CC) $(OBFUSCATED_CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) obfuscated.o .depend *~
	make -C tests clean

test: $(OBJECTS)
	make -C tests test

test-command: command.o
	make -C tests test-command

test-parsing: command.o parsing.o parser.o
	make -C tests test-parsing

memtest: $(OBJECTS)
	make -C tests memtest

.depend: $(SOURCES) obfuscated.c
	$(CC) $(CPPFLAGS) -MM $^ > $@

-include .depend

.PHONY: clean all test test-command test-parsing memtest