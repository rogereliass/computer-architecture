CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
SRCS = src/main.c src/parser.c src/alu.c src/memory.c src/pipeline.c src/utils.c
TARGET = simulator3

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
	
test: $(TARGET)
	./$(TARGET) tests/sample1.asm > out1.txt
	diff -u tests/expected1.txt out1.txt
	./$(TARGET) tests/sample2.asm > out2.txt
	diff -u tests/expected2.txt out2.txt
	
.PHONY: all clean test 