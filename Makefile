SDIR := src
ODIR := obj

LIBPATH :=
LIBS :=

CC = g++
FLAGS = -Wall -mbmi2

TARGET := main

.PHONY: all clean

SOURCES = $(shell find $(SDIR)/ -type f -name "*.cpp")
OBJECTS = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SOURCES))

all: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) $(LIBPATH) $(LIBS) -o $(TARGET)

$(ODIR)/%.o: $(SDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(ODIR)/
	rm -f $(TARGET)