rshell
======
WHAT IS RSHELL:
Rshell is a program that recreates a simple bash command shell. 

DOCUMENTATION:
Rshell prints a command prompt, including the hostname and login name of the system. 
Rshell reads in bash commands and executes them as a regular bash shell would.

HOW TO BUILD:
Rshell contain a makefile to build it. In order to build rshell type make in the command line and the executable will be put in a new folder named bin, the executable will be named 'rshell'.

LIST OF BUGS:
-Any files with spaces and quotes are not handled

-the command 'echo' works, but it works differently than it should. Usually if you were to input 'echo "you"' then on the next line 'you' would be output however in this program "you", with the quotes, is output. Also it will output everything after echo, regardless of quotes and spaces.

-There is a limitation for user input, the max user input is 1024 letters, if they enter more the program outputs randomly and you must exit manually. The program also has a limitation of the number of arguments, there is only room enough for the user to input 50 arguments. 

-The program will only find '&' if it is the last input command or if the last input word ends with '&' (ex. 'ls&') 

-If you input a '&' with a comment directly after it, the program does not recognize it. For example if you were to run 'geany &#comments' there would be an error. But if you run 'geany&#comments' it works.

-Sometimes when you run a program in the background the prompt messes up. For example if I run 
'geany &' it will run correctly in the background however the prompt will be on the wrong like, if I were to put 'ls' as the next command it might show the prompt and then the files directly after it and the prompt will be off for the rest of the session

-When a process is run in the background of a regular shell on the line directly below the command it will put something like '[1] 11330' to show that its in the backgound. With my shell this line does not come up when something runs in the background


hw1 for cs100
