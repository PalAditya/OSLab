#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
	pid_t pid;
	if ((pid = getpid()) < 0)
	{
	   perror("unable to get pid");
	} 
	else
	{
	  printf("The process id is %d\n", pid);
	}
	if(argc==2&&argv[0][0]=='.')
	{
		printf("%d\n",atoi(argv[argc-1])/2);
		return 0;
	}
	//execvp(argv[0],argv);
	argv++;
	argc--;
	//printf("Argc is: %d\n",argc);
	//printf("Argv[0] is %s\n",argv[0]);
	char modify[512];//Need to modify with ./
	int i;
    modify[0]='.';
    modify[1]='/';
    for(i=2;i<512;i++)
    if(argv[0][i-2]=='\0')
            break;
    else
       modify[i]=argv[0][i-2];
    modify[i]='\0';
	//printf("%d\n",atoi(argv[argc-1]));
	int x=atoi(argv[argc-1])/2;
	//printf("%d\n",x);
	sprintf(argv[argc-1], "%d",x);
	if(argc==1)
	{
		printf("%d\n",atoi(argv[argc-1]));
		exit(0);
	}
	//argv++;
	//printf("%s\n",argv[argc-1]);
	//argv++;
	execvp(modify,argv);
	return 0;
}
