all: rshell

rshell:
#	mkdir bin
#	chmod u+x ./bin
	g++ -Wall -Werror -ansi -pedantic src/rshell.cpp -o ./bin/rshell
