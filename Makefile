CCFLAGS=-g -Wall -Wextra

main.o: main.c
	cc ${CCFLAGS} $< -o $@
	./a.out

examples: list.o cstr.o

%.o: examples/%.c
	cc ${CCFLAGS} $< -o $@

valgrind:
	valgrind ./a.out
