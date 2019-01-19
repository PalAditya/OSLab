#include<stdio.h>
#include<stdlib.h>
int main()
{
	FILE *fp=fopen("MyFS.txt","w");
	if(fp==NULL)
	{
		printf("Couldn't format\n");
		return -1;
	}
	fclose(fp);
	fp=fopen("fat.txt","w");
	if(fp==NULL)
	{
		printf("Couldn't format\n");
		return -1;
	}
	fclose(fp);
	return 0;
}