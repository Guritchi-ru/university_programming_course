.PHONY: clean

CC = gcc
CFLAGS = -Wall
INCLUDES = -I./include

TARGET = program
LIBRARY = libpcgame.a
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
LIBDIR = lib

TARGET_PATH = $(BINDIR)/$(TARGET)
LIBRARY_PATH = $(LIBDIR)/$(LIBRARY)


all: directories $(LIBRARY_PATH) $(TARGET_PATH)

directories:
	mkdir -p $(OBJDIR) $(BINDIR) $(LIBDIR)

$(OBJDIR)/compgame.o: $(SRCDIR)/compgame.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/compgame.c -o $(OBJDIR)/compgame.o

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRCDIR)/main.c -o $(OBJDIR)/main.o

$(LIBRARY_PATH): $(OBJDIR)/compgame.o
	ar rc $(LIBRARY_PATH) $(OBJDIR)/compgame.o

$(TARGET_PATH): $(OBJDIR)/main.o $(LIBRARY_PATH)
	$(CC) $(OBJDIR)/main.o -L$(LIBDIR) -lpcgame -o $(TARGET_PATH)

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(LIBRARY_PATH)
	rm -f $(TARGET_PATH)
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR)