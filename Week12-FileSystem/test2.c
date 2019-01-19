#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"16CS01017.h"
int main()
{
	/*repeatedly read a file using myfseek()*/
	FILE *fp=myfopen("Me.txt");
	char buf[1000]={'\0'};
	myfread(buf,1000,fp);//Read first time to get the size and print;
	int size=0,i=0;
	for(i=0;i<1000;i++)
	{
		if(buf[i]=='\0')
			break;
		printf("%c",buf[i]);
		size++;
	}
	//Now len has the size, repeatedly read same amount of bytes 9 more times
	for(i=0;i<9;i++)
	{
		myfseek(fp,0);
		myfread(buf,size,fp);
		printf("%s",buf);
	}
	printf("%p\n",fp);
	myfseek(fp,12);
	printf("%p\n",fp);
	myfread(buf,size,fp);
	printf("%s",buf);
	return 0;
}