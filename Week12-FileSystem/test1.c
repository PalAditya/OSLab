#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"16CS01017.h"
int main()
{
	/*Write a file on the filesystem and read it back*/
	char *x="Hi, my name is Aditya Pal\n";
	char *y="I am a third year Computer Science student of IIT Bhubaneswar\n";
	char *z="limits.h includes definitions of the characteristics of common variable types. The values are implementation specific, but may not be of lower magnitude than certain specified values in a conforming C implementation.\n";
	printf("Writing...\n");
	FILE *fp=myfopen("Me.txt");
	myfwrite(x,strlen(x),fp);
	myfwrite(y,strlen(y),fp);
	myfwrite(z,strlen(z),fp);
	myfclose(fp);
	printf("Reading...\n");
	fp=myfopen("Me.txt");
	char buf[1000]={'\0'};
	myfread(buf,1000,fp);
	int i=0;
	for(i=0;i<1000;i++)
	{
		if(buf[i]=='\0')
			break;
		printf("%c",buf[i]);
	}
	return 0;
}