#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"16CS01017.h"
void trial()//Does random opening and closing
{
	FILE *fp=myfopen("abc.txt");
	char *x="UmmUmm";
	fprintf(fp,"%s\n",x);
	myfclose(fp);
	FILE *fp2=myfopen("willitwork.txt");
	char *y="It worked, yaay";
	char *z="Appended :)";
	fprintf(fp2,"%s\n",y);
	fprintf(fp2,"%s\n",z);
	fprintf(fp2,"%s\n",z);
	myfclose(fp2);
	FILE *fp3=myfopen("justforsafety.txt");
	char *w="Halleluyah";
	fprintf(fp3,"%s\n",w);
	myfclose(fp3);
	FILE *fp5=myfopen("offo.txt");
	char *ww=":(";
	fprintf(fp5,"%s\n",ww);
	myfclose(fp5);
}
int main()
{
	add(2,3);
	//trial()
	FILE *fp=myfopen("abc.txt");
	myfwrite("UmmUmm\n",7,fp);
	myfclose(fp);
	fp=myfopen("willitwork.txt");
	myfwrite("HmmHmm\n",7,fp);
	myfwrite("Huhu\n",5,fp);
	myfwrite("Please\n",7,fp);
	myfclose(fp);
	fp=myfopen("abc.txt");
	char out[10];
	myfread(out,10,fp);
	myfread(out,7,fp);
	printf("%s\n",out);
	myfclose(fp);
	fp=myfopen("abc.txt");
	myfwrite("Offo\n",5,fp);
	myfseek(fp,5);
	myfwrite("a\n",2,fp);
	myfclose(fp);
	fp=myfopen("abc.txt");
	myfclose(fp);
	fp=myfopen("abc.txt");
	myfseek(fp,6);
	myfclose(fp);
	return 0;
}