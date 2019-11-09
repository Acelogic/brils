LIBS=
CFLAGS= -Wall

all: clean
		gcc -Wall ls.c files.c util.c -o brils
		@printf "make: "
		@printf "\033[1;32m"
		@printf "Successfully created program(s)"
		@printf "\033[0m\n"

clean:
		rm -f brils
