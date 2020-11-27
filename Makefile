TARGET = iftun

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
SOURCES  := $(wildcard $(SRCDIR)/$(TARGET).c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

.PHONY: all
all: folders $(BINDIR)/$(TARGET) 

# Create the output folder for the obj and binary
.PHONY: folders
folders : $(BINDIR) $(OBJDIR)
$(OBJDIR) :
	mkdir -p $(OBJDIR)
$(BINDIR) :
	mkdir -p $(BINDIR)

# Compile the source
$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

# Send the binary to the VM1
.PHONY: send
send: $(BINDIR)/$(TARGET)
	cp $(BINDIR)/$(TARGET) ./infra/VM1

# Remove oject file
.PHONY: clean
clean:
	@$(rm) -r $(BINDIR)/$(OBJDIR)
	@echo "Cleanup complete!"

# Remove all output
.PHONY: remove
remove: clean
	@$(rm) -r $(BINDIR)
	@echo "Executable removed!"