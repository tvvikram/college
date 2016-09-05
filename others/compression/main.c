#include<stdio.h>
#include<string.h>
#define DOTTY 1
#define COMPRESS 2
int main(int argc,char **argv)
{
	int i,option=0;
	if(argc < 4)
	{
		printf("Syntax:\n%s <options> <Source File> <Destination File>\nOptions: -c -d --dotty --compress --decompress\n",argv[0]);
		return 1; 
	}
	for(i=0;i<argc;i++)
	{
		if(strcmp(argv[i],"--compress")==0 || strcmp(argv[i],"-c")==0)option = option | COMPRESS;
		if(strcmp(argv[i],"--dotty")==0 )option = option | DOTTY;
	}
	switch(option & COMPRESS)
	{
		// penultimate argument is source and last argument is destination
		case COMPRESS: return mvh(argv[argc-2],argv[argc-1],option & DOTTY);
		default: return dmvh(argv[argc-2],argv[argc-1]);
	}
	return -1;
}
