cc = gcc
header = matching.h
cflags = -Wall -g

.PHONY: run format clean

run: main
	./$<

main: main.c matching.c $(header)
	$(cc) -o $@ $^ $(cflags)

format: *.c *.h
	clang-format -i $^

clean:
	-rm main