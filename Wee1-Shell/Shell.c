// C Program to design a shell in Linux
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<ncurses.h>
#include<term.h>
#include<fcntl.h>
#include<dirent.h>
#include<signal.h>
#include <pwd.h>
#include <grp.h>
//Clearing the shell using escape sequences
#define clearScreen() printf("\033[H\033[J")
#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
int background=0,redir=0,check=100;
// Greeting shell during startup
void init_shell()
{
    clearScreen();
    char* username = getenv("USER");
    printf("USER is: @%s\n", username);
    sleep(1);
    clearScreen();
}
int remove_directory(const char *path)//More powerful than basic rmdir, but not necessary for the assignment
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;
   if (d)
   {
      struct dirent *p;
      r = 0;
      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;
          /* Skipping the names "." and ".." as we don't want to recurse on them. Will delete whole system potentially */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }
          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);
          if (buf)
          {
             struct stat statbuf;
             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }
   if (!r)
   {
      r = rmdir(path);
   }
   return r;
}
// Function to take input and sanitize it
int takeInput(char* str)
{
    char* buf;
    buf = readline("\n>>> ");
    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}
// Function to print Current Directory.
void printDir()
{
    char cwd[1024];//Max assumed size of path
    getcwd(cwd, sizeof(cwd));
    printf("\nDir: %s", cwd);
} 
static void child_handler(int sig)//Kills zombie processes
{
    pid_t pid;
    int status;
    waitpid(-1, &status, WNOHANG);
}
// Function where the system command is executed
void execArgs(char** parsed)
{
    // Forking a child
    signal(SIGCHLD, SIG_IGN);//Signal to signal handler
    pid_t pid = fork(); 
    int status=0;
    if (pid == -1) {
        perror("\nFailed forking child..");
        return;
    } else if (pid == 0) {
	char modify[512];//To add ./ extension for files in current directory
	int i,count1=-1,count2=-1;
    for(i=0;i<100;i++)
        if(parsed[i]==NULL)
            break;
        else if(parsed[i][0]=='>')//To understand where the redirection file name starts
            count1=i+1;
        else if(parsed[i][0]=='<')
            count2=i+1;
    //printf("Counts: %d %d\n",count1,count2);
	if(redir==1)
	{
		freopen(parsed[count1], "a+", stdout); //dup2 can also be used
	}
	else if(redir==2)
	{
        int x=open(parsed[count2],O_RDONLY);
		dup2(x, STDIN_FILENO); //freopen can also be used
	}
	else if (redir==3)
	{
		/*if(strcmp(parsed[1],">")==0)
		{
			freopen(parsed[2], "a+", stdout); //Both redirections open; assumption is that no options are supplied to the program
			freopen(parsed[4], "r", stdin);
		}
		else
		{
			freopen(parsed[4], "a+", stdout); //Both redirections open
			freopen(parsed[2], "r", stdin);
		}*/
        freopen(parsed[count1], "a+", stdout); //Both redirections open
		freopen(parsed[count2], "r", stdin);
	}
        if (redir!=0)
        {
            for(i=0;i<100;i++)
            {
                if(parsed[i][0]=='>'||parsed[i][0]=='<')
                {
                    parsed[i]=NULL;//NULL it as execvp can't be used on text files
                    break;
                }
            }
        }
        if (execvp(parsed[0], parsed) < 0)
        {
            modify[0]='.';
            modify[1]='/';
            for(i=2;i<512;i++)
            if(parsed[0][i-2]=='\0')
            break;
            else
            modify[i]=parsed[0][i-2];
            modify[i]='\0';	
            //printf("%s\n",parsed[0]);
            if (execvp(modify, parsed) < 0)
                    perror("Could not execute command..");
        }
        background=0;
        exit(0);
        } 
        else
        {
        // waiting for child to terminate
        if (background==0)
        {
            waitpid(pid,NULL,0); 
            redir=0;
        }
        else
        {
            waitpid(-1,&status,WNOHANG);
            redir=0;
        }
            return;
        }
} 
int copyFile(char **parsed,int tokens)
{
   if (tokens!=3) //As per specification, only 3 arguments necessary
   return -1;
   char ch;
   FILE *source, *target;
   struct stat sb,sb2;
   stat(parsed[1],&sb);
   stat(parsed[2],&sb2);
   if(difftime(sb.st_mtime,sb2.st_mtime)<0)
   {
       perror("Modification time was less recent for source");
       return 1;
   }
   source = fopen(parsed[1], "r");
   if( source == NULL )
   {
      perror("Can't open source");
      return -1;
   }
   target = fopen(parsed[2], "w");
   if( target == NULL )
   {
      fclose(source);
      perror("Can't open target");
      return -1;
   }
   while( ( ch = fgetc(source) ) != EOF )
      fputc(ch, target);
   fclose(source);
   fclose(target);
   return 0;
}
// Help command builtin
void openHelp()
{
    puts("Welcome to shell help"
        "\nList of Commands supported:"
        "\n>cd"
        "\n>ls"
        "\n>exit"
        "\nmkdir"
        "\nchdir"
        "\ncp"
        "\n>running an executable file/system command in the PATH"
        "\n>piping"
        "\n>input-output redirection"); 
    return;
}
int is_symlink(const char *filename)//Check for symbolic links
{
    struct stat p_statbuf;

    if (lstat(filename, &p_statbuf) < 0) {  /* if error occured */
        perror("calling stat()");
        exit(1);  /* end progam here */
    }
    if (S_ISLNK(p_statbuf.st_mode) == 1)
    {
	    //printf("Link\n");
        return 1;
    } 
    else 
    {
        return 0;
    }
}
int implementLS(int tokens)
{
    char *curr_dir = NULL;
    DIR *dp = NULL;
    struct dirent *dptr = NULL;
    unsigned int count = 0;
    struct stat sb;
    curr_dir = getenv("PWD");
    if(curr_dir == NULL)
    {
        perror("Could not get the working directory\n");
        return -1;
    }
    dp = opendir((const char*)curr_dir);
    if(dp == NULL)
    {
        perror("Could not open the working directory\n");
        return -1;
    }
    int number=0,executable=0;
    for(count = 0; (dptr = readdir(dp)) != NULL ; count++)
    {
	if(is_symlink(dptr->d_name)==0&&strcmp(dptr->d_name,".")!=0&&strcmp(dptr->d_name,"..")!=0)
        {
		executable=0;
		if (stat(dptr->d_name, &sb) != -1 &&tokens==2)
		{

			//printf("File Permissions: \t");
    		printf( (S_ISDIR(sb.st_mode)) ? "d" : "-");
			printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
		    printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
	        printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
			printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
			printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
			printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
			printf( (sb.st_mode & S_IROTH) ? "r" : "-");
			printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
			printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
			//printf("I-node number:            %ld\n", (long) sb.st_ino);
   			//printf("Mode:                     %lo (octal)\n",(unsigned long) sb.st_mode);
			printf("%ld   ", (long) sb.st_nlink);
    		//printf("Ownership:                UID=%ld   GID=%ld\n",(long) sb.st_uid, (long) sb.st_gid);
			struct passwd *pw = getpwuid(sb.st_uid);
			struct group  *gr = getgrgid(sb.st_gid);
			if(pw!=0)
				printf("%s ",pw->pw_name);
			if(gr!=0)
				printf("%s ",gr->gr_name);
			//printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
    		printf("%lld\t  ",(long long int) sb.st_size);
			//printf("Blocks allocated:         %lld\n",(long long) sb.st_blocks);
			number+=(int)sb.st_blocks;
			//printf("Last status change:       %s", ctime(&sb.st_ctime));
    			//printf("Last file access:         %s", ctime(&sb.st_atime));
			int j=0;			
			while(1)//Deleting extra newline
			{
				if (ctime(&sb.st_mtime)[j]!='\n')
					printf("%c",ctime(&sb.st_mtime)[j]);
				else
					break;
				j++;
			}
    		//printf("%s  ", ctime(&sb.st_mtime));	
			//return 0;			
        }
		if (stat(dptr->d_name, &sb) == 0 && sb.st_mode & S_IXUSR)
			executable=1;
		if (executable==1)
		{
		    printf(ANSI_COLOR_GREEN " %s  ",dptr->d_name);//Green colour for executables
	        printf(ANSI_COLOR_RESET);
		}
		else
		printf(" %s  ",dptr->d_name);
		if (tokens==2)
		printf("\n");
	}
	else
	    count--;
    }
    if (tokens==2)
	    printf("Total count: %d\n",number/2);
    else
    	printf("\n %u", count);

}
// Function to execute builtin commands
int ownCmdHandler(char** parsed,int tokens)
{
    int NoOfOwnCmds = 9, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds];
    char* username;
    ListOfOwnCmds[0] = "exit";
    ListOfOwnCmds[1] = "cd";
    ListOfOwnCmds[2] = "help";
    ListOfOwnCmds[3] = "hello";
    ListOfOwnCmds[4] = "pwd";
    ListOfOwnCmds[5] = "mkdir";
    ListOfOwnCmds[6] = "rmdir"; 
    ListOfOwnCmds[7] = "ls";
    ListOfOwnCmds[8] = "cp";
    //printf("%s\n", parsed[0]);
    for (i = 0; i < NoOfOwnCmds; i++) {
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) {
            switchOwnArg = i + 1;
            break;
        }
    }
    switch (switchOwnArg) {
    case 1:
        printf("Goodbye, %s\n",getenv("USER"));
        exit(0);
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        openHelp();
        return 1;
    case 4:
        username = getenv("USER");
        printf("Hello %s. Use help to know more about the shell...\n",username);
        return 1;
    case 5:
        printDir();
        return 1;
    case 6:
        mkdir(parsed[1],S_IRWXU | S_IRWXG | S_IRWXO);//Giving all permissions
        return 1;
    case 7:
        //remove_directory(parsed[1]);
        //can also use system ("rm -rf ...")
        rmdir(parsed[1]);
        return 1;
    case 8:
        implementLS(tokens);
        return 1;
    case 9:
        copyFile(parsed,tokens);
        return 1;
    default:
        break;
    }
    return 0;
}
// function for finding pipe
int parsePipe(char* str)
{
    int i,piped=0;
    for (i = 0; i < 1000; i++)
    {
        if (str [i]=='\0')
        {
            break;
        }
        if (str [i]=='|')
        {
            piped=1;
        }
    }
    if (piped== 1)
    {
        return 1; 
    }
    else
    {
        return 0;// returns zero if no pipe is found.
    }
} 
// function for parsing command words and return count of tokens
int parseSpace(char* str, char** parsed)
{
    int i,flag=0;
    for(i=0;i<1000;i++)
    {
        if(str[i]=='\0')
        break;
        if(str[i]=='>')
        {
            redir=1;
            flag++;
        }
        if(str[i]=='<')
        {
            redir=2;
            flag++;
        }
        if(str[i]=='&'&&str[i+1]=='\0')
        {
            background=1;
            str[i]='\0';
            printf("Will execute in background...");	
        }
    }
	if (flag==2)
		redir=3; 
    for (i = 0; i < 100; i++) {
        parsed[i] = strsep(&str, " ");
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
	return i;
}
int runit(char **args,char str[])//Runs piped commands
{
    char foo[4096];
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe
    pid_t p;
    if (pipe(fd1)==-1)
    {
        fprintf(stderr, "Pipe Failed" ); //equivalent to perror
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
    else if (p > 0)// Parent process
    {
        close(fd1[0]);  // Close reading end of first pipe
        // Write input string and close writing end of first pipe
        write(fd1[1], str, strlen(str)+1);
        close(fd1[1]);
        close(fd2[1]); // Close writing end of second pipe
        read(fd2[0], foo, sizeof(foo));
    	wait(NULL);
        close(fd2[0]); 
        strcpy(str,foo);//copy it back to feed it to next program
        return 0;
    }
    else// child process
    {
        close(fd1[1]);  // Close writing end of first pipe
	    dup2(fd2[1],STDOUT_FILENO);
	    dup2(fd1[0],STDIN_FILENO);
    	 // Close both reading ends
        close(fd1[0]);
        close(fd2[0]);
    	if(execvp(args[0],args)<0)
        {
            char modify[512];//Need to modify with ./
            modify[0]='.';
            modify[1]='/';
            int i;
            for(i=2;i<512;i++)
            if(args[0][i-2]=='\0')
                break;
            else
                modify[i]=args[0][i-2];
                modify[i]='\0';
            execvp(modify,args);
    	    die("execvp later");
        }
        close(fd2[1]);
        exit(0);
    }
}
void piping(char umm[][10][100], int b,char str[])
{
	int i;
	for (i=1;i<=b;i++)
	{
        char **arg = (char **)malloc((10 + 1) * sizeof(char*));//type casting the array of string to const char * for execvp
        size_t ii=0;
        for (ii = 0; ii < 10; ii++)
        {
            if(umm[i][ii][0]=='\0')
            break;
            arg[ii] = umm[i][ii];
        }
        arg[ii] = NULL;
        runit(arg,str);
        //strcpy(str,x);
	}
	printf("%s\n",str);
}
int test(char *ultimate, char umm[][10][100])
{
    int b=0,c=0,flag=0,i;
    char *buf;
    buf=strtok(ultimate," ");
    while(buf!=NULL)
    {
        if (buf[0]=='|')
        {
            b++;
            c=0;
        }
        else
        {
            strcpy(umm[b][c++],buf);
        }
        buf=strtok(NULL," ");
    }
    //Support redirection with pipes
    /*for (i=0;i<=b;i++)
    {
    if (umm[i][0][0]=='>')
    {
    redir=1;
    flag++;
    }
    if(umm[i][0][0]=='<')
    {
    redir=2;
    flag++;
    }
    }
    if (flag==2)
    redir=3;*/
    return b;
}
void handleSpecially(char *str)
{
  char umm[10][10][100];//3-D character array or 2-D array of strings
  int i,j;
  for(i=0;i<10;i++)
  for(j=0;j<10;j++)
  umm[i][j][0]='\0';	
  int b=test(str,umm);
  int link[2];//Parent->child
  pid_t pid;
  char foo[4096];
  if (pipe(link)==-1)
    die("pipe");
  if ((pid = fork()) == -1)
    die("fork");
  if(pid == 0) 
  {
    dup2 (link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    char **arg = (char **)malloc((10 + 1) * sizeof(char*));//type casting for execvp
	size_t ii=0;
    for (ii = 0; ii < 10; ++ii) {
    if(umm[0][ii][0]=='\0')
	break;
    arg[ii] = umm[0][ii];
    }
	arg[ii] = NULL;
    if (execvp(arg[0],arg)<0)
    {
        char modify[512];//Need to modify with ./
        modify[0]='.';
        modify[1]='/';
        for(i=2;i<512;i++)
        if(arg[0][i-2]=='\0')
            break;
        else
            modify[i]=arg[0][i-2];
            modify[i]='\0';
        execvp(modify,arg);
        die("execvp");
    }
  } 
  else
  {
    close(link[1]);
    read(link[0], foo, sizeof(foo));
    wait(NULL);
    piping(umm,b,foo);
  }
}
int processString(char* str, char** parsed, char** parsedpipe)
{
    int piped = 0;
    int tokens=0;
    piped = parsePipe(str); 
    if (piped==1) {
        handleSpecially(str);
 	return 2;
    } 
    else 
    {
        tokens=parseSpace(str, parsed);
        //printf("%d\n",tokens);
    }
    if (ownCmdHandler(parsed,tokens))
    {
	    background=0;
        return 0;
    }
    else
        return 1 + piped;
}
int readDirectories()//redundant, utilised before understanding fully how execvp worked
{
    char str[2000];
    strcpy(str,getenv("PATH"));
    char dir_names[50][50];
    char temp_buf[50];
    int i=0,a=0,b=0;
    for(i=0;i<2000;i++)
    {
	if(str[i]!=':'&&str[i]!='\0')
	   temp_buf[a++]=str[i];
	else
	{
		temp_buf[a]='\0';
	  	strcpy(dir_names[b],temp_buf);
		b++;
		a=0;
		if(str[i]=='\0')
		break;
	}
    }
    for(i=0;i<b;i++)
	printf("%s\n",dir_names[i]);
    char curr_dir [50];
    DIR *dp = NULL;
    struct dirent *dptr = NULL;
    unsigned int count = 0;
    struct stat sb;
    strcpy(curr_dir,dir_names[4]);
    if(curr_dir == NULL)
    {
        printf("\n ERROR : Could not get the working directory\n");
        return -1;
    }
    dp = opendir(curr_dir);
    if(dp == NULL)
    {
        printf("\n ERROR : Could not open the working directory\n");
        return -1;
    }
    int number=0;
	char *argvs[]={"./depmod",NULL};//Example
    for(count = 0; (dptr = readdir(dp)) != NULL ; count++)
    {
	printf("%s\n",dptr->d_name);
	if(strcmp(dptr->d_name,"depmod")==0)
	{		
		printf("%d",execvp(argvs[0],argvs));
	}
    }
}
int main()
{
        char inputString[1000];//maximum possible length of input string
        //We can have at max 100 consecutive commands
        char *parsedArgs[100];
        char *parsedArgsPiped[100];
        int execFlag = 0;
        init_shell();
        int status;
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = child_handler;
        //Loop and wait
        while (1) {
            printDir();// print shell line
        //sigaction(SIGCHLD, &sa, NULL);
        if(background==1)
        waitpid(-1,&status,WNOHANG);
            // take input
        if (takeInput(inputString))
                continue;
            // process
        execFlag = processString(inputString, parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command or it is a builtin command,1 if it is a simple command, 2 if it is including a pipe.
        // execute
        if (execFlag == 1)
            execArgs(parsedArgs);
        //background=0;
        if(redir==1||redir==3)
            freopen("/dev/tty", "w", stdout);
        if(redir==2||redir==3)
            freopen("/dev/tty","r",stdin);
        redir=0;
    }
    exit(0);
}