all: clean executable execute

executable: lista.o mem.o proc.o p0.o
	gcc -Wall -o p0

%.o: %.c
	gcc -c -g $<

execute:
	./p0

valgrind: clean executable
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-report.txt ./p0

clean:
	rm -f *.o *.txt p0
