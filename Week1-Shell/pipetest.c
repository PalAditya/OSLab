#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
	int i;
	char s[100];
	scanf("%s",s);
	for(i=0;i<100;i++)
	if(s[i]=='\0')
	{
		s[i]='l';
		break;
	}
	s[i+1]='\0';
	printf("%s\n",s);
	return 0;
}
