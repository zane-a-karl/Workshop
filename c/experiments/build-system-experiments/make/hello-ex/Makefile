
CC=gcc
CFLAGS=-Wall -g

all: myapp

#  a pattern rule, ‘%.o : %.c’ says how to make any file stem.o from another file stem.c.
%.o: %.c %.h

# Automatic Vars: only have values within the recipe.
# $@: The filename representing the target.
# $<: The filename of the first prerequisite.
# $?: The names of all prerequisites that are newer than the target, separated by spaces.
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables
myapp: hello.c test.o
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm *.o
