#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// types of characters and number of characters
int types,noc;

// dictionary structure in receiving end
struct dict
{
	char c,*code;
};
struct dict **codes;

// reading dictionary at receiving end
void getdict(char *filename)
{
	FILE *fp;
	int i;
	char s[20],c,*dicfile;
	dicfile=(char *)malloc(sizeof(char)*(strlen(filename)+5));
	// read from .dic file
	strcpy(dicfile,filename);
	strcat(dicfile,".dic");
	fp=fopen(dicfile,"rb");
	if(fp==NULL)
	{
		printf("Failed to Open Dic : %s",dicfile);
		exit(-1);
	}
	fscanf(fp,"%d\t%d\n",&types,&noc);
	codes=(struct dict **)malloc(types*sizeof(struct dict*));
	// scanf from dictionary file and write to the structure
	// containing character and codeword
	for(i=0;i<types;i++)
	{
		c=fgetc(fp);
		fscanf(fp," %[^\n]s",s);
		fgetc(fp);
		codes[i]=(struct dict *)malloc(sizeof(struct dict));
		codes[i]->c=c;
		codes[i]->code=(char *)malloc(sizeof(char)*(strlen(s)+1));
		strcpy(codes[i]->code,s);
		if(feof(fp))break;
	}
}
/*
void printdict()
{
	int i;
	for(i=0;i<types;i++)
		printf("%d %s %c\n",i+1,codes[i]->code,codes[i]->c);
}*/

// decompression
decompress(char *src,char *tgt)
{
	char buffer[20];
	unsigned char j,ch;
	int i=0,k;
	FILE *fs,*ft;
	fs=fopen(src,"rb");
	ft=fopen(tgt,"wb");
	if(fs==NULL || ft==NULL)return;
	ch=fgetc(fs);
	// get code from the compressed file, compare
	// it with the dictionary and decode it
	while(!feof(fs) && noc)
	{				
//		printf("%d ",noc);
		// convert from bits to bytes
		for(j=128;j>0;j=j/2)
		{
			// if bit number > 2 power j, then
			// jth bit is set and decrement the
			// read character
			if(ch>=j)
			{
				buffer[i++]='1';
				ch-=j;
			}
			else
				buffer[i++]='0';
			
			buffer[i]='\0';
			// compare the generated codeword with the codes in dictionary
			for(k=0;k<types;k++)
				if(strcmp(buffer,codes[k]->code)==0)
				{
					// decrement number of characters
					noc--;
					fprintf(ft,"%c",codes[k]->c);
					i=0;
					if(noc==0)return 0;
				}
	//	printf("%c",buffer[i]);
		}//*/
		ch=fgetc(fs);		
	}
	fclose(fs);
	fclose(ft);
	return 0;
}
