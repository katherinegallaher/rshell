all: rshell ls

rshell:
	mkdir bin
#	chmod u+x ./bin
	g++ -Wall -Werror -ansi -pedantic src/rshell.cpp -o ./bin/rshell
ls:
	g++ -Wall -Werror -ansi -pedantic src/ls.cpp -o ./bin/ls
clean:
	rm -rf bin	
