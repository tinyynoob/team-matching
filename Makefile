cc = gcc
source = main.c matching.c parse.c
header = matching.h parse.h fls.h ils.h log.h
outfile = department_result.csv partcipant_result.csv
cflags = -Wall -g

.PHONY: run format clean

run: main
	./$<

main: $(source) $(header)
	$(cc) -o $@ $(source) $(cflags)

format: *.c *.h
	clang-format -i $^

clean:
	-rm main
	-rm $(outfile)