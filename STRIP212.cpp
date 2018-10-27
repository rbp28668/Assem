/* Program that strips any occurance of character \212 from the input */
#include <stdio.h>
int main(int argc,char *argv[])
{
int ch;
FILE *infp,*outfp;
char tfn[L_tmpnam];
int i;

for(i=1;i<argc;++i)
	{
	printf("%s\n",argv[i]);
	infp=fopen(argv[i],"r");
	tmpnam(tfn);
	outfp=fopen(tfn,"w");

	while((ch=fgetc(infp))!=EOF)
		{
		if(ch==138)
			{
			putchar('*');
			continue;
			}
		fputc(ch,outfp);
		}
	putchar('\n');
	
	fclose(infp);
	fclose(outfp);
	unlink(argv[i]);
	if(rename(tfn,argv[i])) printf("Cannot rename\n");
	}
return(0);
}
