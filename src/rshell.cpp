#include <iostream>
#include <string>

#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h>
#include <stdlib.h> 
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> //strtok
#include <signal.h>

using namespace std; 

void my_execvp(char **, char **);
int parsepath(char*, char **);
int parse(char*, char **, bool &);//parses the input command into a char**
bool background(int, char **, bool);//checks to see if it should be a background proc
void useDup(char**);//checks for redirection 
void lookforpipes(char**,bool,char**);//checks if there's a piping cmd & splits 
void execute(char **, bool,char**);//do execvp without piping
void executepiping(char**, char**,bool,char**);//do execvp with piping
void checktokens(string&);//if there was no spaces between a token & a word it's split
void sig_handlerc(int signum);
void sig_handlerz(int signum);//doesn't wokr yet

pid_t handlerpid;

int main()
{
	//get the path and parse it 
	char *path = getenv("PATH");
	char *parsed_path[50];
	int parsednum = parsepath(path, parsed_path);

    //first get the username and hostname for the prompt
    char username[100] = {0};
    char hostname[100] = {0};

    if(getlogin_r(username,sizeof(username)-1))
    {
        perror("There was an error in getlogin_r. ");
    }
    if(-1 == gethostname(hostname,sizeof(hostname)-1))
    {
        perror("There was an error in gethostname. ");
    }
    for(int i=0;i<30;i++)
    {
        if(hostname[i] == '.')
        {
            hostname[i] = '\0';
        }
    } 
	//end username/hostname
    
    while(1)
    {
       //prompt
	   	char buf[1024];
		if(!getcwd(buf, 1024))
			perror("problem with getcwd. ");
		cout<<buf<<endl;

       cout<<username<<"@"<<hostname<<"$ ";

        //grab user input
        char input[1024];

  		string str;
		getline(cin,str);
		checktokens(str);
		
		strcpy(input, str.c_str());

		if(str[0] == '#')
			continue;
        //check if exit 
        if(!strcmp(input, "exit")) exit(1);

		//check for ctrlc
		signal(SIGINT, sig_handlerc);

		//check for ctrlz
//fix
	//	signal(SIGTSTP, sig_handlerz);
//fix

        //argv will hold all of the commands
        char **argv;
        argv = new char *[50];
        
        //check if user simply pressed enter(no input) 
        bool emptyinput = false;
        if(!strcmp(input,"")) emptyinput = true;
        
		//parse input
        int i = parse(input, argv, emptyinput);

		//check if you want to do cd
		if(!strcmp(argv[0], "cd"))
		{
			if(str.size() == 2)//you want to go to the home dir
			{
				char *home = getenv("HOME");
				if(-1 == chdir(home))
					perror("error in chdir. ");

			}
			else	
			{
				if(-1 == chdir(argv[1]))
					perror("error in chdir. ");
			}
			continue;
		}

        //check for background processes
        bool backgroundproc = false;
        backgroundproc = background(i, argv, emptyinput);

		//search for pipes 
		lookforpipes(argv,backgroundproc,parsed_path);

       delete[] argv;
	  // delete[] parsed_path;
    }
	parsednum=0;//bc i was getting error from unused var
    return 0;
}

void checktokens(string& str)
{//this function checks for >, <, | and >>  at beginning, middle & end of commands & separates them
	int size = str.size();
	for(int i=0; i<size; i++)
	{
		if(str[i] == '<' && i!=0 && str[i+1] != ' ' && str[i-1] != ' ')//its in the middle of a word
		{
			str.insert(i+1," ");
			str.insert(i," ");
		}
		else if(str[i] == '<' && str[i+1] != ' ')//its at the beginning of a word
			str.insert(i+1," ");
		
		else if(str[i] == '<' && i!=0 && str[i-1] != ' ')//its at the end of a word
			str.insert(i," ");
		

		if(str[i] == '|' && i!=0 && str[i+1] != ' ' && str[i-1] != ' ')
		{
			str.insert(i+1," ");
			str.insert(i," ");
		}
		else if(str[i] == '|' && str[i+1] != ' ')
			str.insert(i+1," ");
		
		else if(str[i] == '|' && i!=0 && str[i-1] != ' ')
			str.insert(i," ");
		
		
		if(str[i-1] != '>' && str[i+1] != '>')//it's only >, not >>
		{
			if(str[i] == '>'  && i!=0 && str[i+1] != ' ' && str[i-1] != ' ')
			{
				str.insert(i+1," ");
				str.insert(i," ");
			}
			else if(str[i] == '>' && str[i+1] != ' ')
				str.insert(i+1," ");

			else if(str[i] == '>' && i!=0 && str[i-1] != ' ')
				str.insert(i," ");
		}
		
		if(str[i-1] != '>' && str[i+1] == '>')//its >>
		{
			if(str[i] == '>'  && i!=0 && str[i+2] != ' ' && str[i-1] != ' ')
			{
				str.insert(i+2," ");
				str.insert(i," ");
			}
			else if(str[i] == '>' && str[i+2] != ' ')
				str.insert(i+2," ");

			else if(str[i] == '>' && i!=0 && str[i-1] != ' ')
				str.insert(i," ");
		}
	}
}

void lookforpipes(char** argv, bool background, char **parsedpath)
{
	bool pipe = false;
	int pipeloc = 0;//for splitting

	char** part1;
	part1 = new char*[50];
	char** part2;
	part2 = new char *[50];

	for(int i=0; argv[i] != '\0';i++)
	{
		if(!strcmp(argv[i], "|"))
		{
			pipe = true;
			pipeloc = i;
			break; 
		}
	}

	if(pipe)
	{
		for(int i=0; i<pipeloc; i++)
			part1[i] = argv[i];

		int pt2ind = 0;
		for(int i=pipeloc+1; argv[i] != '\0'; i++)
		{
			part2[pt2ind] = argv[i];
			pt2ind++;
		}
		part1[pipeloc] = NULL;
		part2[pt2ind] = NULL;

		executepiping(part1,part2,background,parsedpath);
	}
	else//it's a regular command, no pipe
		execute(argv,background,parsedpath);

	delete[] part1;
	delete[] part2;
}
void executepiping(char** part1, char** part2, bool background, char **parsedpath)
{
	int fd[2];
	if(pipe(fd) == -1)
		perror("There was an error with pipe(). ");

	pid_t pid = fork();	
//    int pid = fork();
    if(pid == -1)
    {
        perror("There was a error with fork(). ");
    }
    else if(pid == 0)
    {
		//write to pipe 
		if(-1 == dup2(fd[1],1))
			perror("There was an error with dup2. ");
		if(-1 == close(fd[0]))
			perror("There was a problem with close. ");
		my_execvp(parsedpath,part1);
        //if(-1 == execvp(part1[0], part1))
        //{
          //  perror("There was an error in execvp. ");
       // }
        exit(1);
    }
	//read from pipe

	int savestdin;
	if(-1 == (savestdin = dup(0)))//need to restore later or infinite loop
		perror("There was an error with dup. ");

	if(-1 == dup2(fd[0],0))
		perror("There was an error with dup2. ");
	if(-1 == close(fd[1]))
		perror("There was a problem with close. ");

	if(-1 == wait(0))
		perror("There was an error in wait. ");


	lookforpipes(part2,background, parsedpath);//in order to chain multiple pipes you have to check again if you have a pipe in the second half

	//restore stdin
	if(-1 == dup2(savestdin,0))
		perror("There is an error with dup2. ");
}
	
int parse(char userinput[], char **argv, bool &emptyinput)
{
    int numargs=0;
    argv[numargs] = strtok(userinput," \t\n");

    while(argv[numargs] != NULL)
    {

		//start check for comments
		char *copy = argv[numargs];
		int length = strlen(argv[numargs]);
		if(copy[0] == '#')//comment at beginning of word
		{
			argv[numargs] = NULL;
			if(!numargs) emptyinput=true;
			break;
		}
		if(copy[length-1] == '#')//comment at end of word
		{
			copy[length-1] = '\0';
			break;
		}
		bool shouldbreak=false;
		for(int i=0; i<length; i++) //if there's a comment in the middle
		{
			if(copy[i] == '#')
			{
				copy[i] = '\0';
				argv[numargs] =copy;
				shouldbreak=true;
			}
		}
		if(shouldbreak) break;
		//end check for comments

        numargs++;
        argv[numargs] = strtok(NULL, " \t\n");
    }
    return numargs;
}

bool background(int numargs, char **argv, bool emptyinput)
{
    bool backgroundproc;
    if(numargs > 0) numargs--; //check for &, background process
    if(!emptyinput && !strcmp(argv[numargs],"&"))//checks if last parse is & 
    {
        backgroundproc = true;
        argv[numargs] = NULL;
    }
    else if(!emptyinput) //check if last parse ends with &
    {
        char* copy = argv[numargs];
        int arglen = strlen(argv[numargs]) - 1;
        if(copy[arglen] == '&') 
        {
            backgroundproc=true;
            copy[arglen] = '\0';
            argv[numargs] = copy;
        }
    }
    return backgroundproc;
}

void useDup(char** argv)
{
	for(int i=0; argv[i] != '\0';i++)
	{
		int fd;
		if(!strcmp(argv[i], ">"))
		{
			argv[i] = NULL;
			if(-1== (fd = open(argv[i+1],O_CREAT | O_WRONLY | O_TRUNC, 0666 )))
				perror("There was an error with open. ");
			if( -1 == dup2(fd,1))
				perror("There was an error with dup2. ");
			break;
		}
		else if(!strcmp(argv[i], ">>"))
		{
			argv[i] = NULL;
			if(-1==(open(argv[i+1],O_CREAT | O_WRONLY | O_APPEND, 0666 )))
				perror("There was an error with open. ");
			if(-1 == dup2(fd,1))
				perror("There was an error with dup2. ");
			break;
		}
		else if(!strcmp(argv[i], "<"))
		{
			argv[i] = NULL;
			if(-1== (fd = open(argv[i+1],O_RDONLY )))
				perror("There was an error with open. ");
			if(-1 == dup2(fd,0))
				perror("There was an error with dup2. ");
			break;
		}
	}
}

void execute(char **argv, bool backgroundproc, char **parsedpath)
{
	pid_t pid = fork();
   // int pid = fork();
    if(pid == -1)
        perror("There was a error with fork(). ");

    else if(pid == 0)
    {
		//check for io redirection
		useDup(argv);

		my_execvp(parsedpath,argv);

      //  if(-1 == execvp(argv[0], argv))
        //    perror("There was an error in execvp. ");
		
        exit(1);
    }

//cout<<"pid t in execute is: "<<pid<<endl;
    if(!backgroundproc)
	{
		handlerpid = pid;
		if(-1 == wait(0)) perror("There was an error in wait. ");
	}
}
void sig_handlerc(int signum)//handles ctrl-c
{
	signal(SIGINT,SIG_IGN);
}
int parsepath(char *path, char **parsed_path)
{
    int numargs=0;
    parsed_path[numargs] = strtok(path,":");

    while(parsed_path[numargs] != NULL)
    {
        numargs++;
        parsed_path[numargs] = strtok(NULL, ":");
    }

    return numargs;
}
void my_execvp(char **parsedpath, char **argv)
{
	for(int i=0; parsedpath[i] != '\0'; i++)
	{
		char check[250] = {0};

		strcpy(check,parsedpath[i]);
		if(check[strlen(check)-1] != '/')
			strcat(check, "/");
		strcat(check,argv[0]);


		char *newargv[50] = {0};
		newargv[0] = check;
		for(int j=1; argv[j] != NULL; j++)
			newargv[j] = argv[j];

		if(-1 == execv(newargv[0], newargv)) ; 
		else
			return;
	}
	if(errno)
	{
		perror("problem with execv. " );
		exit(1);
	}
}
void sig_handlerz(int signum)
{
	kill(handlerpid, SIGSTOP);
}
