# project name (generate executable with this name)
TARGET   = trinityenabler

# compiling flags here
CFLAGS   = -std=c99 -Wall -I. -pedantic -Werror -O3

# linking flags here
LFLAGS   = -Wall -I. -lm -framework IOKit -framework Foundation

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(rm) $(OBJECTS)

.PHONY: remove
remove: clean
	$(rm) $(BINDIR)/$(TARGET)
