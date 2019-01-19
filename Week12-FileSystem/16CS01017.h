void add(int a,int b);
FILE *myfopen(char str[]);
int myfread(char *out,int bytes, FILE *fp);
int myfseek(FILE *fp, int off);
int myfwrite(char *in,int bytes,FILE *fp);
void myfclose(FILE *fp);