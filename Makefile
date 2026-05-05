

CC = gcc

TARGET = bin/lexer

# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD

CFLAGS=-Wall -Wextra

CFILES = src/main.c src/syntactical.c src/utils.c

OBJECTS = $(patsubst %.c,%.o,$(CFILES))
DEPFILES = $(patsubst %.c,%.d,$(CFILES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

# only want the .c file dependency here, thus $< instead of $^.
#
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(TARGET) $(OBJECTS) $(DEPFILES)