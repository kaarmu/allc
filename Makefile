run: main.c
	cc -Wall -Wextra main.c
	./a.out

val: main.c
	cc -Wall -Wextra main.c
	valgrind ./a.out
