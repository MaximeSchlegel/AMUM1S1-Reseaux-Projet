TARGET   = tunalloc

# Compiler
CC       = gcc
CFLAGS   = -Wall -I.

# Linker
LINKER   = gcc
LFLAGS   = -Wall -I. -lm

# Directories
SRCDIR   = src
OBJDIR   = out/obj
BINDIR   = out

# Files
SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

.PHONY: all
all: folders $(BINDIR)/$(TARGET) 

.PHONY: folders
folders : $(BINDIR) $(OBJDIR)
$(OBJDIR) :
	mkdir -p $(OBJDIR)
$(BINDIR) :
	mkdir -p $(BINDIR)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"



$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) -r $(OBJDIR)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"