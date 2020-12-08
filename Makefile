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
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)


###################################################################################################
# Make all
.PHONY: all
all: folders tunnel tunnel64d test_iftun test_extremite send clean

###################################################################################################
# Create the output folder for the obj and binary
.PHONY: folders
folders :
	@mkdir -p $(BINDIR) $(OBJDIR) $(REMOTE)
	@echo "Outout folders created!"

###################################################################################################
# Compile all the sources
$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(INCLUDES)
	@$(CC) $(CFLAGS) -I $(INCDIR) -c $< -o $@
	@echo "Compiled "$<" successfully!"

###################################################################################################
# Linked the main
.PHONY: tunnel tunnel64d test_iftun test_extremite
tunnel: folders $(OBJECTS) $(INCLUDES) $(MNSDIR)/tunnel.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/tunnel $(OBJECTS) $(MNSDIR)/tunnel.c
	@echo "Linked tunnel successfully!"

tunnel64d: folders $(OBJECTS) $(INCLUDES) $(MNSDIR)/tunnel64d.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/tunnel64d $(OBJECTS) $(MNSDIR)/tunnel64d.c
	@echo "Linked tunnel64d successfully!"

test_iftun: folders $(OBJECTS) $(INCLUDES) $(MNSDIR)/test_iftun.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/test_iftun $(OBJECTS) $(MNSDIR)/test_iftun.c
	@echo "Linked test_iftun successfully!"

test_extremite: folders $(OBJECTS) $(INCLUDES) $(MNSDIR)/test_extremite_in.c $(MNSDIR)/test_extremite_out.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/test_extremite_in  $(OBJECTS) $(MNSDIR)/test_extremite_in.c
	@$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/test_extremite_out $(OBJECTS) $(MNSDIR)/test_extremite_out.c
	@echo "Linked test_extremite successfully!"

###################################################################################################
# Send the binaries and scripts to the partage folder
.PHONY: send
send:
	@find ./out     -maxdepth 1 -type f -executable -exec cp '{}' $(REMOTE) \;
	@find ./scripts -maxdepth 1 -type f -executable -exec cp '{}' $(REMOTE) \;
	@echo "Files sent to "$(REMOTE)"!"

###################################################################################################
# Remove all the generated files
.PHONY: clean remove
clean:
	@rm -rf $(OBJDIR)
	@echo "Cleanup complete!"
remove: clean
	@rm -rf $(BINDIR) $(REMOTE)
	@echo "Executable removed!"