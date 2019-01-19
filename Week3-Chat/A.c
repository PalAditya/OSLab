#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<pthread.h>
#include<stdlib.h>
char *myfifoA="/tmp/myfifoA";
char *myfifoB="/tmp/myfifoB";
void *readIt(void *vargp)
{
    char arr2[80];
    int fd;
    while (1)
    {
        fd = open(myfifoB, O_WRONLY);
        fgets(arr2, 80, stdin);
        write(fd, arr2, strlen(arr2)+1);
        close(fd);
    }
    pthread_exit(NULL);
}
void *writeIt(void *vargp)
{
    int fd;
    char arr1[80];
    while(1)
    {
        fd = open(myfifoA, O_RDONLY);
        read(fd, arr1, sizeof(arr1));
        printf("A sent me: %s", arr1);
        close(fd);
    }
    pthread_exit(NULL);
}
int main()
{
    int rc;
    mkfifo(myfifoA,0666);
    mkfifo(myfifoB,0666); 
    pthread_t tread,twrite;
    rc=pthread_create(&tread, NULL, readIt, NULL);
    if(rc<0)
        exit(0);
    rc=pthread_create(&twrite, NULL, writeIt, NULL);
    if(rc<0)
        exit(0);
    pthread_join(tread,NULL);
    pthread_join(twrite,NULL);
    return 0;
    /*while (1)
    {
        fd = open(myfifoB, O_WRONLY);
        fgets(arr2, 80, stdin);
        write(fd, arr2, strlen(arr2)+1);
        close(fd);
        fd = open(myfifoA, O_RDONLY);
        read(fd, arr1, sizeof(arr1));
        printf("B sent me: %s", arr1);
        close(fd);
    }
    return 0;*/
}