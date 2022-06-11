cc = gcc
source = main.c matching.c parse.c
header = matching.h parse.h fls.h ils.h log.h
outfile = department_result.csv participant_result.csv
cflags = -Wall

.PHONY: run format clean windows

run: main
	./$<

main: $(source) $(header)
	$(cc) -o $@ $(source) $(cflags)

windows: $(source) $(header)
	x86_64-w64-mingw32-gcc -o team_matching $(source) $(cflags)

format: $(source) $(header)
	clang-format -i $^

clean:
	-rm main
	-rm $(outfile)