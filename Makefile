SRC=main.c

all: ${SRC}
	gcc -fsanitize=address -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Werror -std=c17 -Wpedantic -O0 -g ${SRC} -o hw2

clean:
	rm -rf *.o hw2
