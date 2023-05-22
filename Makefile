CCFLAGS=-g -Wall -Wextra

main.o: main.c
	cc ${CCFLAGS} $< -o $@
	./a.out

out/%.o: src/%.c
	cc ${CCFLAGS} $< -o $@

examples: out/examples/list.o
	mkdir -p out/examples

out/examples/%.o: examples/%.c
	cc ${CCFLAGS} $< -o $@

valgrind:
	valgrind ./a.out
