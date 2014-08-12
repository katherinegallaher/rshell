rshell
======
WHAT IS RSHELL:
Rshell contains two programs in the src folder, rshell.cpp and ls.cpp.
Rshell is a program that recreates a simple bash command shell. 
ls is a program that recreates the ls command, with optional flags -l -a and -R as well as file/folder parameters.

DOCUMENTATION:
Rshell prints a command prompt, including the hostname and login name of the system. 
Rshell reads in bash commands and executes them as a regular bash shell would.
ls outputs just as the regular ls commands do, the flags -l -a -R can be called as well.

HOW TO BUILD:
Rshell contains a makefile to build it. In order to build rshell type make in the command line and the executables will be put in a new folder named bin, the executables will be named 'rshell' and 'ls'.

LIST OF BUGS:
-Rshell:Any files with spaces and quotes are not handled

-Rshell:the command 'echo' works, but it works differently than it should. Usually if you were to input 'echo "you"' then on the next line 'you' would be output however in this program "you", with the quotes, is output. Also it will output everything after echo, regardless of quotes and spaces.

-Rshell:There is a limitation for user input, the max user input is 1024 characters, if they enter more the program outputs randomly and you must exit manually. The program also has a limitation of the number of arguments, there is only room enough for the user to input 50 arguments. 

-Rshell:The program will only find '&' if it is the last input command or if the last input word ends with '&' (ex. 'ls&') 

-Rshell:If you input a '&' with a comment directly after it, the program does not recognize it. For example if you were to run 'geany &#comments' there would be an error. But if you run 'geany&#comments' it works.

-Rshell:Sometimes when you run a program in the background the prompt messes up. For example if I run 
'geany &' it will run correctly in the background however the prompt will be on the wrong like, if I were to put 'ls' as the next command it might show the prompt and then the files directly after it and the prompt will be off for the rest of the session

-Rshell:When a process is run in the background of a regular shell on the line directly below the command it will put something like '[1] 11330' to show that its in the backgound. With my shell this line does not come up when something runs in the background


-ls:When doing a -l command for many files and folders sometimes the names of the files not in the folders output incorrectly.

ls:If you use -l with different files from different folders the file size numbers do not output aligned.

ls:When doing the ls command on a file and a folder, if the folder is the first file/folder parameter and the file the second, the output is not divided between the two sections. 

-ls:If you enter a folder and end the input with '/' it will not recognize the folder, however it will recognize it otherwise.

-ls:Commands with -R do not work exactly as they are supposed to if the program encounters a folder parameter it will immediately output everything in that folder and then go back to outputting everything in the outter folder. However, the output format is correct for -R.

hw1 & hw2 for cs100
