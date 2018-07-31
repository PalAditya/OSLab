#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
int main()
{
	/*int a,b;
	scanf("%d%d",&a,&b);
	printf("%d %d\n",a,b);
	printf("Umm\n");*/
    /*Spawn a child to run the program.*/
    pid_t pid=fork();
    if (pid==0) { // child process 
        static char *argv[]={"./Shell.out","Shell.out",NULL};
	execv(argv[0],argv);
        exit(127); // only if execv fails 
    }
    else {
        waitpid(pid,0,0); // wait for child to exit 
    }
    return 0;
}


