#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <cstdlib>
#include <iostream>
#include<iomanip>
using namespace std;

void permissions(struct stat);//print permissions,used in printallinfo
void printallinfo(struct stat, int, int,dirent*,int outside=0, char*outsidedir=NULL);//print all info, for -l
void myreaddir(char *, int);//where all the output happens depending on flags
void maxnumdigs(char*,int &, int&,int&, int);//used to align for -l
//void outputcolors(dirent*,struct stat);
void outputcolors(dirent*, struct stat, int outside =0, char* outsidedir=NULL);
int GetBit(int x, int k)
{
	return ((x & (0x01 << k)) !=0);
}

int main(int argc, char* argv[])
{
	char dirName[250];
	char dirName2[250];
	char dirName3[250];
	int filenum=1;
	strcpy(dirName, ".");
	int flags=0;//will be set to see what flags are input
	//bit 0 = a
	//bit 1 = l
	//bit 2 = R
	int i;
	for(i=1; i<argc; i++)
	{
		if(!strcmp(argv[i],"-a")) flags = flags | 0x01;
		else if(!strcmp(argv[i], "-l")) flags = flags | 0x02;
		else if(!strcmp(argv[i], "-R")) flags = flags | 0x04;
		else if(!strcmp(argv[i],"-al")) flags = flags | 0x03;
		else if(!strcmp(argv[i], "-aR")) flags = flags | 0x05;
		else if(!strcmp(argv[i],"-la")) flags = flags | 0x03;
		else if(!strcmp(argv[i],"-lR")) flags = flags | 0x06;
		else if(!strcmp(argv[i],"-Ra")) flags = flags | 0x05;
		else if(!strcmp(argv[i],"-Rl")) flags = flags | 0x06;
		else if(!strcmp(argv[i],"-alR")) flags = flags | 0x07;
		else if(!strcmp(argv[i], "-laR")) flags = flags | 0x07;
		else if(!strcmp(argv[i], "-Ral")) flags = flags | 0x07;
		else if(!strcmp(argv[i],"-aRl")) flags = flags | 0x07;
		else if(!strcmp(argv[i],"-lRa")) flags = flags | 0x07;
		else if(!strcmp(argv[i],"-Rla")) flags = flags | 0x07;
		else
		{//its a filename/folder
			if(filenum==1)
			{
				strcpy(dirName,argv[i]);
				filenum++;
			}
			else if(filenum==2)
			{
				strcpy(dirName2,argv[i]);
				filenum++;
			}
			else if(filenum==3)
			{
				strcpy(dirName3,argv[i]);
				filenum++;
			}
		}
	}

	if(filenum == 1 && dirName[0] !='.')
		 filenum++;
	else if(filenum == 1)
		myreaddir(dirName,flags);

	bool pastfile=false;
	//bool notfound = true;
	bool somewhereelse = false;	
	char* outsidefilen;
	outsidefilen = new char[512];
	char* outsidedir;
	outsidedir = new char[512];

	for(int i=1;i<filenum;i++)
	{
		bool notfound=true;
	START:
		//check if the dirname is a file, if it is just output contents, then endl
		//if it's a folder then just do readdir
		bool isfile= false;
		DIR *dirp;
		if(!somewhereelse)
		{
			if(!(dirp = opendir("."))) 
				perror("error with main opendir. "); 
		} 
		else
		{
			if(!(dirp = opendir(outsidedir)))
			perror("error with main opendir. ");
		}
		dirent *direntp;
		struct stat s;
		int num1=0;
		int num2=0;
		char newdirName[250];
		while((direntp = readdir(dirp))) 
		{
			if(errno != 0)
				perror("Error with main readdir. ");
			char newpath[1024];
			if(!somewhereelse)
			{
				strcpy(newpath,".");
				strcat(newpath,"/");
			}
			else
			{
				strcpy(newpath,outsidedir);
				strcat(newpath,"/");
			}
			strcat(newpath,direntp->d_name);

			if(-1 ==stat(newpath,&s))//reads in about the current file
				perror("There is an error with main stat. ");
		
			if(i==1)
				strcpy(newdirName,dirName);
			else if(i==2)
				strcpy(newdirName,dirName2);
			else if(i==3)
				strcpy(newdirName,dirName3);
			if(somewhereelse)
				strcpy(newdirName,outsidefilen);
			if(!strcmp(newdirName,direntp->d_name))
			{
				notfound = false;
				if(s.st_mode & S_IFREG)
				{
					isfile = true;
					pastfile=true;
					break;
				}
			}
		}	
		if(-1 == closedir(dirp))
			perror("There is an error with main closedir. ");

		char* finddot;
		if(notfound)
			finddot = strchr(newdirName,'.');
		if(finddot && notfound)//it's a file that isn't in the current directory	
		{
			isfile = true;
			somewhereelse = true;
			string newstring = newdirName;
			string thefilename = newstring.substr(newstring.rfind("/"));
			thefilename.erase(0,1);//now thefilename is the filename
			newstring = newstring.substr(0,newstring.size() - thefilename.size() - 1);
			outsidefilen = strcpy(outsidefilen,thefilename.c_str());
			outsidedir = strcpy(outsidedir,newstring.c_str());
			goto START;
		}

		if(isfile && !notfound)
		{
			if(somewhereelse) 
			{
				somewhereelse = false;
				if(!GetBit(flags,1))
					outputcolors(direntp,s,1,outsidedir);
				else
					printallinfo(s,num1,num2,direntp,1,outsidedir);//doesn't work right
			}

			else if(!GetBit(flags,1))
				outputcolors(direntp,s);
			else
				printallinfo(s,num1,num2,direntp);

		}
		else 
		{
			if(pastfile) cout<<endl<<endl;
			if(pastfile) cout<<newdirName<<":"<<endl;
			myreaddir(newdirName,flags);
		}
		//end check	
	}
	cout<<endl;
	delete[] outsidefilen;
	delete[] outsidedir;
}


void outputcolors(dirent *direntp, struct stat s, int outside, char* outsidedir)
{
	if(direntp->d_name[0] == '.' && (s.st_mode & S_IFDIR))//gray bkgrnd w/ bluewords
	{
		if(outside) cout<<"\033[47m\033[38;5;32m"<<outsidedir<<'/'<<"\033[0;00m";
		cout<<"\033[47m\033[38;5;32m"<<direntp->d_name<<"\033[0;00m";
	}

	else if(direntp->d_name[0] == '.' && (s.st_mode & S_IXUSR))//gray bkgrnd w/ greenword
	{
		if(outside) cout<<"\033[47m\033[38;5;34m"<<outsidedir<<'/'<<"\033[0;00m";
		cout<<"\033[47m\033[38;5;34m"<<direntp->d_name<<"\033[0;00m";
	}

	else if(direntp->d_name[0] == '.')
	{
		if(outside) cout<<"\033[47m"<<outsidedir<<'/'<<"\033[0;00m";
		cout<<"\033[47m"<<direntp->d_name<<"\033[0;00m";
	}

	else if(s.st_mode & S_IFDIR)//just a directory
	{
		if(outside) cout<<"\033[38;5;32m"<<outsidedir<<'/'<<"\033[0;00m";
		cout<<"\033[38;5;32m"<<direntp->d_name<<"\033[0;00m";
	}
	else if(s.st_mode & S_IXUSR)//just an executable
	{
		if(outside) cout<<"\033[38;5;34m"<<outsidedir<<'/'<<"\033[0;00m";
		cout<<"\033[38;5;34m"<<direntp->d_name<<"\033[0;00m";
	}
	else
	{
		if(outside) cout<<outsidedir<<'/';
		cout<<direntp->d_name;	
	}

	if(s.st_mode & S_IFDIR) cout<< '/';
	else if(s.st_mode & S_IXUSR) cout<< '*';
	cout<<"  ";
}
void myreaddir(char* dirName, int flags)
{
//first i need to find the # of digs of the largest size for -l
//skip if you're not doing -l
	int maxdigsize=0;
	int maxdignlink=0;
	int blocks=0;
	if(GetBit(flags,1)) //if -l
		maxnumdigs(dirName,maxdigsize,maxdignlink,blocks,flags);

	DIR *dirp2;

	if(GetBit(flags,2)) cout<<dirName<<':'<<endl;//for -R

	if(!(dirp2 = opendir(dirName)))
		perror("error with opendir2. ");

	dirent *direntp;

	if(GetBit(flags,1)) cout<<"Total: "<<(blocks/2)<<endl;
	while((direntp = readdir(dirp2)))//add perror
	{
		if(errno != 0)
			perror("Error with myreaddir readdir. ");
		struct stat s;
		char newpath[1024];
		strcpy(newpath,dirName);
		strcat(newpath,"/");
		strcat(newpath,direntp->d_name);

		if(-1 ==stat(newpath,&s))//reads in about the current file
			perror("There is an error with 2nd stat. ");
		
//now we'll check which flags we execute:

		if(!flags)//flags=0 so it's just ls
		{			
			if(direntp->d_name[0] != '.')
			{
				outputcolors(direntp,s);
			}
		}
		else if(GetBit(flags,0) && !GetBit(flags,1) && !GetBit(flags,2)) //if it's -a
		{
			outputcolors(direntp,s);
		} 

		else if(GetBit(flags,1) && !GetBit(flags,2))//if its -l but not -R
		{
			if(GetBit(flags,0))//-la
				printallinfo(s,maxdigsize,maxdignlink,direntp);
			else //just -l
			{
				if(direntp->d_name[0] != '.')
					printallinfo(s,maxdigsize,maxdignlink,direntp);
			}
		}
		
		else if(GetBit(flags,2))//it's -R
		{ 
			if(S_ISDIR(s.st_mode) && (direntp->d_name[0] != '.'))
			{
				if(flags == 4) //it's just -R
				{
					if(direntp->d_name[0] != '.') 
					{	
						outputcolors(direntp,s);
					}
				}
				else if(GetBit(flags,1) && !GetBit(flags,0))//it's -Rl
				{
					if(direntp->d_name[0] != '.')
						printallinfo(s,maxdigsize,maxdignlink,direntp);
				}
				else if(GetBit(flags,0) && !GetBit(flags,1))//its -Ra
				{
					outputcolors(direntp,s);
				}
				else if(GetBit(flags,1) && GetBit(flags,0))//it's -laR
					printallinfo(s,maxdigsize,maxdignlink,direntp);

				cout<<endl<<endl;
				myreaddir(newpath,flags);
			}

			else
			{
				if(flags == 4) //it's just -R
				{
					if(direntp->d_name[0] != '.') 
					{	
						outputcolors(direntp,s);
					}
				}
				else if(GetBit(flags,1) && !GetBit(flags,0))//it's -Rl
				{
					if(direntp->d_name[0] != '.')
						printallinfo(s,maxdigsize,maxdignlink,direntp);
				}
				else if(GetBit(flags,0) && !GetBit(flags,1))//its -Ra
				{
					outputcolors(direntp,s);
				}
				else if(GetBit(flags,1) && GetBit(flags,0))
					printallinfo(s,maxdigsize,maxdignlink,direntp);
			}	
		}

	}
	//if(!GetBit(flags,1)) cout<<endl;
	if(-1 ==closedir(dirp2))
		perror("Problem with closedir. ");
}
void maxnumdigs(char* dirName,int &maxdigsize,int &maxdignlink, int &blocks, int flags)
{	
		DIR *dirp;
		if(!(dirp = opendir(dirName)))
			perror("Error with opendir1. ");

		dirent *finddiglength;
		while((finddiglength = readdir(dirp)))
		{
			if(errno != 0)
				perror("Error with maxnumdigs readdir. ");
			struct stat dig;
	
			char newpath[1024];
			strcpy(newpath,dirName);
			strcat(newpath,"/");
			strcat(newpath,finddiglength->d_name);

			if(-1 == stat(newpath,&dig))
				perror("There is an error with 1st stat. ");

			if(GetBit(flags,0))
				blocks += dig.st_blocks;
			else
			{
				if(finddiglength->d_name[0] != '.')
					blocks += dig.st_blocks;
			}

			int length1 = 1;
			int length2 = 1;
			int currentdig1 = dig.st_size;
			int currentdig2 = dig.st_nlink;
			while(currentdig1 /= 10) length1++;
			if(length1 > maxdigsize) maxdigsize = length1;
			while(currentdig2 /= 10) length2++;	
			if(length2 > maxdignlink) maxdignlink = length2;
		}
		closedir(dirp);
}
void permissions(struct stat s)
{
	//all if else if
	(s.st_mode & S_IFREG) ? cout<< '-':
	(s.st_mode & S_IFDIR) ? cout<< 'd':
	(s.st_mode & S_IFCHR) ? cout<< 'c':
	(s.st_mode & S_IFBLK) ? cout<< 'b':
	(s.st_mode & S_IFIFO) ? cout<< 'f':
	(s.st_mode & S_IFSOCK) ? cout<<'s':
	(s.st_mode & S_IFLNK) ? cout<< 'l':
	cout<<'-';
	
	(s.st_mode & S_IRUSR) ? cout<< 'r' : cout<<'-';
	(s.st_mode & S_IWUSR) ? cout<< 'w' : cout<<'-';
	(s.st_mode & S_IXUSR) ? cout<< 'x' : cout<<'-';
	
	(s.st_mode & S_IRGRP) ? cout<< 'r' : cout<<'-';
	(s.st_mode & S_IWGRP) ? cout<< 'w' : cout<<'-';
	(s.st_mode & S_IXGRP) ? cout<< 'x' : cout<<'-';
	
	(s.st_mode & S_IROTH) ? cout<< 'r' : cout<<'-';
	(s.st_mode & S_IWOTH) ? cout<< 'w' : cout<<'-';
	(s.st_mode & S_IXOTH) ? cout<< 'x' : cout<<'-';

	cout<<" ";
}
void printallinfo(struct stat s, int maxdigsize, int maxdignlink, dirent *direntp,int outside, char*outsidedir)
{
	struct passwd *pw;
	if(!(pw = getpwuid(s.st_uid)))
		perror("there was an error in getpwuid. ");
	struct group *gp;
	if(!(gp = getgrgid(s.st_gid)))
		perror("There was an error in getgrgid. ");
			
	char buff[20];
	struct tm *timeinfo;
	timeinfo = localtime(&(s.st_mtime));
	strftime(buff,20,"%b %d %H:%M",timeinfo);

	permissions(s);
	cout<<setw(maxdignlink)<<right<<s.st_nlink<<' ';
	cout<<pw->pw_name<<' ';
	cout<<gp->gr_name<<' ';
	cout<<setw(maxdigsize)<<right<<s.st_size<<' ';
	cout<<buff<<' ';
	outputcolors(direntp,s,outside,outsidedir);
	cout<<endl; 
}
