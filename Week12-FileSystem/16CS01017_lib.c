#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"16CS01017.h"
int start=0,offset=0,size=0,open=0,read=0,write=0,oldsize=0,seeked=0;
char name[80];
FILE *x=NULL,*orig=NULL;
void add(int a,int b)
{
	printf("%d\n", a+b);
}
FILE *myfopen(char str[])
{
	FILE *f=NULL,*f2=NULL;
	strcpy(name,str);
	f=fopen("MyFS.txt","r+");
	f2=fopen("fat.txt","r");
	if(f==NULL||f2==NULL)
	{
		perror("Format the filesystem first please\n");
		return NULL;
	}
	char buf[100];
	offset=0;
	int success=0;
	while(fgets(buf,100,f2)!=NULL)
	{
		char *token=strtok(buf," ");
		if(strcmp(token,str)==0)
			break;
		token=strtok(NULL," ");
		token=strtok(NULL," ");
		offset+=atoi(token);
		success++;
	}
	start=offset;
	//printf("Start is %d\n",start);
	fseek(f,offset,SEEK_SET);
	size=0;//Because if you write, you overwrite :)
	return f;
}
void set_params()
{
	FILE *fp=fopen("fat.txt","r");
	char buf[100];
	char *token=NULL;
	size=-1;
	while(fgets(buf,100,fp)!=NULL)
	{
		token=strtok(buf," ");
		if(strcmp(token,name)==0)
		{
			token=strtok(NULL," ");
			size=atoi(strtok(NULL," "));
			break;
		}
	}
	//printf("Size in set_params is: %d\n",size);
	fclose(fp);
}
int myfread(char *out,int bytes, FILE *fp)
{	
	if(size==0)
	set_params();
	orig=fp;
	if(size==0)
	{
		printf("No such file exists\n");
		return -1;
	}
	if(read+bytes>size)
	{
		char c=' ';
		printf("Out-of-bounds access attempted. Do you wish to read upto End of file only? Y/n?\n");
		scanf("%c",&c);
		if(c=='n')
			return -1;
		else
			bytes=size-read;
	}
	fread(out,bytes,1,fp);
	read+=bytes;
	return 0;
}
int myfwrite(char *in,int bytes,FILE *fp)
{
	write=1;
	//printf("Entered myfwrite\n");
	if(!open)
	{
		x=fopen("temp3.txt","w");
		open=1;
	}
	if(x==NULL)
	{
		return -1;
		printf("Couldn't open file\n");
	}
	if(fputs(in,x)==-1)
	{
		printf("Couldn't write file\n");
		return -1;
	}
	if(!seeked)
		size+=bytes;
	else
		size=seeked+bytes;
	seeked=0;
	return 0;
}
int myfseek(FILE *fp, int off)
{
	if(read>0)
	{
		//printf("Read is %d\n",read);
		fp=orig;
		fseek(fp,off,SEEK_CUR);
		read=0;
		return 0;
	}
	if(fp==NULL)
	{
		printf("Must open file before calling fseek()\n");
		return -1;
	}
	if(orig==NULL)
	{
		orig=fp;
	}
	if(!write)
	{
		//printf("Must open file before calling fseek()\n");
		if(orig!=NULL)
			fp=orig;
		char buf[100];
		x=fopen("temp3.txt","w");
		while(off>100)
		{
			fread(buf,100,1,fp);
			fwrite(buf,1,100,x);
			off-=100;
		}
		fread(buf,off,1,fp);
		fwrite(buf,1,off,x);
		fclose(x);
		return 0;
	}
	if(off<0)
	{
		printf("Can't seek past file start\n");
		return -1;
	}
	/*FILE *f=fopen("temp4.txt","w");
	FILE *f2=fopen("temp3.txt","r");*/
	fseek(x,off,SEEK_CUR);
	printf("I'm executed\n");
	seeked=off;
	return 0;
}
void myfwrite2(FILE *fp)
{
	FILE *f=fopen("fat.txt","r+");
	FILE *f2=fopen("temp2.txt","w");
	fseek(fp,0,SEEK_SET);
	char buf[100],buf2[100];
	char *token=NULL;
	int found=0,toread=0;
	while(fgets(buf,100,f)!=NULL)
	{
		//printf("Entered loop of myfwrite2\n");
		token=strtok(buf," ");
		toread=0;
		//printf("%s %s\n", token,name);
		//printf("WTF %d\n",strcmp(token,name));
		if(strcmp(token,name)==0)
		{
			found=1;
			//printf("Comparison success. %s,%s\n",token,name);
			FILE *f3=fopen("temp3.txt","r");
			if(f3==NULL)
				printf("F3 failed to open 2nd time\n");
			while(fgets(buf2,100,f3)!=NULL)
			{
				toread+=strlen(buf2);
				//printf("Inside strcmp: %d %s\n",toread,buf2);
				fputs(buf2,f2);
				//fputs("\n",f2);
			}
			fclose(f3);
			token=strtok(NULL," ");
			toread=atoi(strtok(NULL," "));
			//printf("%d\n",oldsize);
			fseek(fp,oldsize,SEEK_CUR);
			continue;
		}	
		token=strtok(NULL," ");
		toread=atoi(strtok(NULL," "));
		while(toread>100)
		{
			fread(buf2,100,1,fp);
			fwrite(buf2,1,100,f2);
			toread-=100;
		}
		fread(buf2,toread,1,fp);
		//printf("I read at last: %s\n",buf2);
		fwrite(buf2,1,toread,f2);
	}
	if(!found)
	{
		FILE *f3=fopen("temp3.txt","r");
		while(fgets(buf2,100,f3)!=NULL)
		{
			toread+=strlen(buf2);
			//printf("Inside strcmp: %d\n",toread);
			fputs(buf,f2);
		}
		fclose(f3);
	}
	rename("temp2.txt","temp3.txt");
	fclose(f2);
	//return 1;
}
void myfclose(FILE *fp)
{
	if(write==0)
	{
		open=0;
		size=0;
		read=0;
		orig=NULL;
		remove("temp3.txt"); 
		return;
	}
	FILE *f=fopen("fat.txt","r+");
	FILE *f2=fopen("temp.txt","w");
	char buf[100];
	int exist=0,y,z;
	oldsize=0;
	char *token=NULL;
	while(fgets(buf,100,f)!=NULL)
	{
		token=strtok(buf," ");
		if(strcmp(token,name)==0)
		{	
			token=strtok(NULL," ");
			oldsize=atoi(strtok(NULL," "));
			fprintf(f2,"%s %d %d\n",name,start,size);
			exist=1;
			continue;
		}
		if(!exist)
		{
			//printf("Should come once\n");
			y=atoi(strtok(NULL," "));
			z=atoi(strtok(NULL," "));
			//printf("WTF? %d %d\n",y,z);
			fprintf(f2,"%s %d %d\n",token,y,z);
		}
		else
		{
			y=atoi(strtok(NULL," "))+size-oldsize;
			z=atoi(strtok(NULL," "));
			fprintf(f2,"%s %d %d\n",token,y,z);
		}
	}
	if(!exist)
	{
		//printf("Not exist: %s %d %d\n",token,start,size);
    	fprintf(f2,"%s %d %d\n",name,start,size);
	}
	fclose(f2);
	fclose(f);
	fclose(x);
	rename("temp.txt","fat.txt");
	myfwrite2(fp);
	fclose(fp);
	rename("temp3.txt","MyFS.txt");
	open=0;
	size=0;
	read=0;
	write=0;
	orig=NULL;
}