

CC = gcc

TARGET = bin/syntact

# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD

CFLAGS=-Wall -Wextra -g 

CFILES = src/main.c src/lexer.c src/utils.c src/syntactical.c src/semantic_domain_w_vector.c

#patsub - pattern substitution it helps naming the .o and .d files ( ex : main.c -> main.o ) 
OBJECTS = $(patsubst src/%.c,bin/%.o,$(CFILES))
DEPFILES = $(patsubst src/%.c,bin/%.d,$(CFILES))

all: $(TARGET)

#here we use $^ because we combine all .o files at once into the binary file
$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

# here we need $< because we compile one .o file at a time
bin/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf bin/*

gtstat: 
	git status
	git diff --stat

# include the dependencies
-include $(DEPFILES)



