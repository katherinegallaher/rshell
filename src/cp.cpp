#include <iostream>
#include <istream>
#include <fstream>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include "Timer.h"
using namespace std;

void cp1(char* source, char* dest);
void cp2(char* source, char* dest);
void cp3(char* source, char* dest);

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Not enough parameters." << endl;
        exit(1);
    }

    else
    {
        ifstream in(argv[2]);
        if(in.good())
        {
            cerr<<"file already exists"<<endl;
            exit(1);
        }
        if(argc == 4)
        {
        if(!strcmp(argv[3],"1"))
            cp1(argv[1], argv[2]);
        else if(!strcmp(argv[3],"2"))
            cp2(argv[1],argv[2]);
        else
            cp3(argv[1], argv[2]);
        }
        else
            cp3(argv[1],argv[2]);
    }
    return 0;
}


void cp1(char *source, char* dest)
{
    Timer t;
    double eTime;
    t.start();

    ifstream in(source);
    ofstream out(dest);
    
    
    while(in.good())
    {
        char ch = in.get();
        out.put(ch);
    }
    
    in.close();
    out.close();
    
    t.elapsedUserTime(eTime);
    
    cout << 
    cout << eTime << endl;

}
void cp2(char* source, char* dest)
{
    clock_t start;
    double duration;
    start = clock();
    
    int fd = open(source, O_RDONLY);
    int fdout = open(dest, O_RDWR | O_CREAT);
    char buf[1];
    while( read(fd, buf,1) != 0 )
    {
        write(fdout,buf,1);
    }
    
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout<<"the duration is: "<<duration<<endl;


}

void cp3(char* source, char* dest)
{
    clock_t start;
    double duration;
    start = clock();

    int fd = open(source, O_RDONLY);
    int fdout = open(dest, O_RDWR | O_CREAT);
    char buf[BUFSIZ];

    for (int i = 0; i < BUFSIZ; ++i)
        buf[i] = '\0';

    while( read(fd, buf,BUFSIZ) != 0 )
    {
        write(fdout,buf,BUFSIZ);
    }
    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout<<"the duration is: "<<duration<<endl;

}


