#include <iostream>
#include <string>

#include <unistd.h> //for execv, write
#include <sys/types.h> //for wait
#include <sys/wait.h> //for wait
#include <stdlib.h> //for exit
#include <stdio.h>
#include <errno.h>
#include <string.h> //strtok

using namespace std; 

int parse(char userinput[], char **argv)
{
    int numargs=0;
    argv[numargs] = strtok(userinput," \t\n");
    while(argv[numargs] != NULL)
    {
        if(!strcmp(argv[numargs],"#"))//if there's a comment stop parsing and delete it from the command list
        {
            argv[numargs] = NULL;
            if(numargs > 0) numargs--;
            break;
        }   
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

void execute(char **argv, bool backgroundproc)
{
    int pid = fork();
    if(pid == -1)
    {//error check
        perror("There was a error with fork(). ");
    }
    else if(pid == 0)
    {
        if(-1 == execvp(argv[0], argv))
        {//error check
            perror("There was an error in execvp. ");
        }
        exit(1);
    }
        
    if(!backgroundproc) wait(0);
}

int main()
{
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
    
    
    while(1)
    {
       //prompt
//       cout<<username<<"@"<<hostname<<" $ ";
cout<<"$ ";
        //grab user input
        char input[1024];
        cin.getline(input, 1024);
        
        //check if exit 
        if(!strcmp(input, "exit")) exit(1);

        //argv will hold all of the commands
        char **argv;
        argv = new char *[50];
        
        //check if user simply pressed enter(no input) 
        bool emptyinput = false;
        if(!strcmp(input,"")) emptyinput = true;
        
        int i = parse(input, argv);//parse input

        //check for background processes
        bool backgroundproc = false;
        backgroundproc = background(i, argv, emptyinput);
        
        //execute the command
        execute(argv, backgroundproc);
        delete[] argv;
    }
    return 0;
}
