#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>
#include<string.h>
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

int runit(char **args,char str[])
{
    char foo[4096];
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe
    pid_t p;
 
    if (pipe(fd1)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    if (pipe(fd2)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    p = fork(); 
    if (p < 0)
    {
        fprintf(stderr, "fork Failed" );
        return 1;
    }
 
    // Parent process
    else if (p > 0)
    {
        //char concat_str[100];
 
        close(fd1[0]);  // Close reading end of first pipe
 	
        // Write input string and close writing end of first
        // pipe.
        write(fd1[1], str, strlen(str)+1);
        close(fd1[1]);
 
        // Wait for child to send a string
        //wait(NULL);
 
        close(fd2[1]); // Close writing end of second pipe
 
        // Read string from child, print it and close
        // reading end.
        read(fd2[0], foo, sizeof(foo));
    	//printf("Output: (\n%.*s)\n", nbytes, foo);
    	wait(NULL);
        close(fd2[0]); 
	strcpy(str,foo);
	//printf("Foo is %s and Str is %s\n",foo,str);
	return 0;
    }
 
    // child process
    else
    {
        close(fd1[1]);  // Close writing end of first pipe
 
        // Read a string using first pipe
        //char concat_str[100];
        //read(fd1[0], concat_str, 100);
	//printf("%s \n",concat_str);
	dup2(fd2[1],STDOUT_FILENO);
	dup2(fd1[0],STDIN_FILENO);
    	 // Close both reading ends
        close(fd1[0]);
        close(fd2[0]);
    	execvp(args[0],args);
    	die("execvp later");
 
       
        // Write concatenated string and close writing end
        //write(fd2[1], concat_str, strlen(concat_str)+1);
        close(fd2[1]);
 
        exit(0);
    }
}

void piping(char umm[][10][100], int b,char str[])
{
	//printf("%s\n",str);
	int i;
	//str[0]='a';
	//str[1]='\0';
	//printf("str now is %s\n",str);
	//char x[4096];
	//char *args[]={"/usr/bin/head","head","/usr/bin/head","head","./pipetest.out","pipetest","./pipetest.out","pipetest"};
	//char *args[]={"head","-3",0};
	for (i=1;i<=b;i++)
	{
	char **arg = (char **)malloc((10 + 1) * sizeof(char*));
	size_t ii=0;
        for (ii = 0; ii < 10; ++ii) {
        if(umm[i][ii][0]=='\0')
	break;
        arg[ii] = umm[i][ii];
        }
	arg[ii] = NULL;
	    //runit(args[i],args[i+1],str,str);
	    runit(arg,str);
	    //strcpy(str,x);
	}
	printf("%s\n",str);
}

int test(char *ultimate, char umm[][10][100])
{
int b=0,c=0;

char *buf;
buf=strtok(ultimate," ");
while(buf!=NULL)
{
//printf("--%s\n",buf);
if (buf[0]=='|')
{
//umm[b][c++]=NULL;
b++;
c=0;
}
else
{
strcpy(umm[b][c++],buf);
}
buf=strtok(NULL," ");
}

return b;
}

int main() {
  char ultimate[1000];
  char umm[10][10][100];
	int i,j;
  for(i=0;i<10;i++)
  for(j=0;j<10;j++)
  umm[i][j][0]='\0';	
  scanf("%[^\n]",ultimate);
  //printf("%s ",ultimte);
  int b=test(ultimate,umm);
	/*for (i=0;i<=b;i++)
for(j=0;j<10;j++)
{
printf("%s ", umm[i][j]);
}
printf("\n");
}*/
  int link[2];//Parent->child
  pid_t pid;
  char foo[4096];

  if (pipe(link)==-1)
    die("pipe");

  if ((pid = fork()) == -1)
    die("fork");

  if(pid == 0) {

    dup2 (link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    //char *so[]={"ls","-l",NULL};
    //execvp("/bin/ls","ls",(char *)0);
    char **arg = (char **)malloc((10 + 1) * sizeof(char*));
	size_t ii=0;
    for (ii = 0; ii < 10; ++ii) {
    if(umm[0][ii][0]=='\0')
	break;
    arg[ii] = umm[0][ii];
    }
	arg[ii] = NULL;
    execvp(arg[0],arg);
    die("execvp");

  } else {

    close(link[1]);
    read(link[0], foo, sizeof(foo));
    //printf("Output: (\n%.*s)\n", nbytes, foo);
    wait(NULL);
    piping(umm,b,foo);
  }
  return 0;
}
