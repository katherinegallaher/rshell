all: bin rshell ls

bin:
	[ ! -d $bin ] && mkdir bin
rshell: bin
	g++ -Wall -Werror -ansi -pedantic src/rshell.cpp -o ./bin/rshell
ls: bin
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o ./bin/ls
clean:
	rm -rf bin	
