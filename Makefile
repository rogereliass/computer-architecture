# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I./includes
LDFLAGS = 

# Source files and directories
SRC_DIR = src
INCLUDE_DIR = includes
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)

# Executable name
EXEC = processor

# Default target
all: $(EXEC)

# Linking
$(EXEC): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(EXEC) $(LDFLAGS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(EXEC)
	.\$(EXEC).exe

# Clean build files
clean:
	del /Q /F $(SRC_DIR)\*.o $(EXEC).exe

# Show help
help:
	@echo "Available targets:"
	@echo "  all    - Build the processor executable (default)"
	@echo "  run    - Build and run the processor"
	@echo "  clean  - Remove all build files"
	@echo "  help   - Show this help message"

# Declare phony targets
.PHONY: all run clean help 