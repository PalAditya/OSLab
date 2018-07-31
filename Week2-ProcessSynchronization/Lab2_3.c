#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
int main(int argc, char *argv[])
{
	/*char *buf;
	char ultimate[1000];
	scanf("%[^\n]",ultimate);
	printf("%s\n",ultimate);
	char umm[20][100];
	int i,a=0;
    buf=strtok(ultimate," ");
    while(buf!=NULL)
    {
        strcpy(umm[a++],buf);
        printf("%s\n",buf);
        buf=strtok(NULL," ");
    }
    
    for(i=0;i<a;i++)
    printf("%s\n",umm[a]);*
    int i;
    for(i=0;i<argc;i++)
    printf("%s\n",argv[i]);
    argv++;
    for(i=0;i<argc-1;i++)
    printf("%s\n",argv[i]);
    return 0;*/
	pid_t pid;
	if ((pid = getpid()) < 0)
	{
	  perror("unable to get pid");
	} 
	else
	{
	  printf("The process id is %d\n", pid);
	}
    argv++;
    int i;
    char modify[512];//Need to modify with ./
    modify[0]='.';
    modify[1]='/';
    for(i=2;i<512;i++)
        if(argv[0][i-2]=='\0')
            break;
        else
            modify[i]=argv[0][i-2];
    modify[i]='\0';
    execvp(modify,argv);
    return 0;
}