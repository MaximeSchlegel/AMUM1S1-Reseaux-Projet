# Compiler
CC       = gcc
CCFLAGS   = -Wall -g

# Linker
LINKER   = gcc
LFLAGS   = -Wall -lm

# Src directories
SRCDIR = code/src
MNSDIR = code/mains
INCDIR = code/include

# Out directory
OBJDIR = out/obj
BINDIR = out
REMOTE = ./infra/partage/tunnel/

# Files
SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)


###################################################################################################
# Make all
.PHONY: all
all: folders tunnel test_iftun test_extremite send

###################################################################################################
# Create the output folder for the obj and binary
.PHONY: folders
folders :
	@mkdir -p $(BINDIR) $(OBJDIR) $(REMOTE)

###################################################################################################
# Compile all the sources
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@
	@echo "Compiled "$<" successfully!"

###################################################################################################
# Linked the main
.PHONY: tunnel iftun_test extremite_test
tunnel: folders $(OBJECTS)
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/tunnel $(OBJECTS) $(MNSDIR)/tunnel.c
	@echo "tunnel created successfully!"

test_iftun: folders $(OBJECTS)
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/test_iftun $(OBJECTS) $(MNSDIR)/test_iftun.c
	@echo "test_iftun created successfully!"

test_extremite: folders $(OBJECTS)
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/test_extremite_in  $(OBJECTS) $(MNSDIR)/test_extremite_in.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/test_extremite_out $(OBJECTS) $(MNSDIR)/test_extremite_out.c
	@echo "test_extremite created successfully!"

###################################################################################################
# Send the binaries and scripts to the partage folder
.PHONY: send
send:
	@find ./out -maxdepth 1 -type f -executable -exec cp '{}' $(REMOTE) \;
	@find ./scripts -maxdepth 1 -type f -executable -exec cp '{}' $(REMOTE) \;
	@echo "Files sent to "$(REMOTE)

###################################################################################################
# Remove all the generated files
.PHONY: clean remove
clean:
	@rm -rf $(BINDIR)/$(OBJDIR)
	@echo "Cleanup complete!"
remove: clean
	@rm -rf $(BINDIR) $(REMOTE)
	@echo "Executable removed!"