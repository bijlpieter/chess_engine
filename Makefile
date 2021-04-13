SDIR := src
IDIR := include
ODIR := obj

CC = g++
FLAGS = -Wall -mbmi2 -O3

TARGET := main
TAR_GZ := chess.tar.gz

.PHONY: all clean

SOURCES = $(shell find $(SDIR)/ -type f -name "*.cpp")
OBJECTS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SOURCES))

all: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -I $(IDIR) -o $(TARGET)

$(ODIR)/%.o: $(SDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -c $< -I $(IDIR) -o $@

tar: clean
	tar -cvzf $(TAR_GZ) $(IDIR) $(SDIR) Makefile README.md LICENSE .gitignore

clean:
	rm -rf $(ODIR)/
	rm -f $(TARGET)
	rm -f $(TAR_GZ)