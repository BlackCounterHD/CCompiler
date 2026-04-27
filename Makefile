

CC = gcc

TARGET = /header/lexer

OBJECTS = main.o lexer.o utils.o

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) -c $< -o $@