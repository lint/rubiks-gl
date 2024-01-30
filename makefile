
TARGET   = rubiks-gl

CC       = gcc
CFLAGS   = -O3 -Wall -Wno-unused-result -Wno-maybe-uninitialized
LIBS     = -lXi -lXmu -lglut -lGLEW -lGLU -lm -lGL

SRCDIR   = src
BINDIR   = bin

SOURCES=$(shell find src -type f -iname '*.c')
OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS) $(LIBS)

run:
	$(BINDIR)/$(TARGET)

clean:
	rm -f $(BINDIR)/$(TARGET) $(OBJECTS)
