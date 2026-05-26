# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -g

# Target Executable
TARGET = minishell

# Source Files
SRC = mini.c

# Object Files
OBJ = $(SRC:.c=.o)

# Default Target
all: $(TARGET)

# Link Object Files
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile Source Files
%.o: %.c mini.h
	$(CC) $(CFLAGS) -c $<

# Run Program
run: $(TARGET)
	./$(TARGET)

# Clean Generated Files
clean:
	rm -f $(OBJ) $(TARGET)

# Rebuild
rebuild: clean all


//To run
//gcc -Wall -o minishell mini.c
//./minishell