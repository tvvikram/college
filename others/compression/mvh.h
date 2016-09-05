#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

// types of characters present
int types;
// number of characters
int noc;
// source and target file length
long int slen,tlen;

char *treearr;

// dictionary structure
// contains character and codeword
struct dict
{
	char c,*code;
};
struct dict **codes;

// minimum variance huffman tree structure
// contains character, frequency and links to
// parent and left and right children
struct mvhtree
{
	unsigned char ch;
	int n;
	struct mvhtree *parent,*left,*right	;
};

typedef struct mvhtree * NODE;

// returns a node of minimum variance huffman tree structure type
NODE getnode()
{
	NODE t;
	t=(NODE)malloc(sizeof(struct mvhtree));
	if(t==NULL)
		return NULL;
	t->ch='\0';
	t->n=0;
	t->parent=NULL;
	t->left=NULL;
	t->right=NULL;
	return t;
}

// calculates frequency of each count
int readfile(char *filename,int *chcnt)
{
	FILE *fp;
	char ch;
	int i,j,t;
	fp=fopen(filename,"rb");
	if(fp==NULL) return -1;
	ch=fgetc(fp);
	
	// increments frequency of each character on each of its occurence
	while(!feof(fp))
	{
		noc++;
		ch=(ch+256)%256;
		if(chcnt[ch]==0)types++;
		chcnt[ch]++;
		ch=fgetc(fp);
	}
}

// prints the stack that contains the frequency of characters
void printstack(NODE *stack,int n)
{
	int i;
	for(i=0;i<n;i++)
		printf("%c%d",stack[i]->ch,stack[i]->n);
	printf(";\n"); 
}

// builds the huffman tree
NODE buildtree(int *chcnt)
{
	int i,j=0;
	NODE *stack,tmp;
	stack=(NODE *)malloc(types * sizeof(NODE));
	for(j=0;j<types;j++)
		stack[j]=(NODE)malloc(sizeof(struct mvhtree));
	j=0;
	// inserts all ascii characters into the stack
	for(i=0;i<256;i++)
		if(chcnt[i]>0)
		{
			stack[j]->ch=i;
			stack[j]->n=chcnt[i];
			j++;
		}
	
	// insertion sort is performed on the stack
	// in non-increasing order	
	for(i=0;i<types;i++)
		for(j=i+1;j<types;j++)
			if(stack[i]->n<stack[j]->n)
			{
				tmp=stack[i];
				stack[i]=stack[j];
				stack[j]=tmp;
			}
	i=types-1;
	
	// gets the least two frequency nodes and builds a binary tree
	// then the new value is reordered in the stack
	// the stack size is decreased
	// the next two least values are taken and the operation is repeated
	// the newly formed tree is attached to its parent
	while(i>0)
	{
		tmp=getnode();
		if(tmp==NULL)
			exit(-1);
	
		tmp->n=stack[i]->n+stack[i-1]->n;
		tmp->left=stack[i-1];
		tmp->right=stack[i];
		stack[i-1]->parent=tmp;
		stack[i]->parent=tmp;

//		printstack(stack,i+1);

		j=i--;
		// to reorder the stack
		while(j>0 && tmp->n >= stack[j]->n )
		{
			j--;
			stack[j]=stack[j-1];
			
		}

		stack[j]=tmp;

	}
	return stack[0];
};

// prints a codeword
void disp(char *s)
{	
	int i,val=0;
	char *t;
	t=(char *)malloc(strlen(s)+1);
	strcpy(t,s);
	for(i=0;t[i]!='\0';i++)
		val=val*2+1+t[i]-'0';
	printf(" %d %s\n",val,t);
}

// to print codebook
void small()
{
	int i;
	char s[255];
	for(i=0;i<types;i++)
	{	printf("%c",codes[i]->c);disp(codes[i]->code);}

}

// to display in dot format
void printdotty(FILE *fp,NODE root,int p)
{
	static int c=1;
	int tc;
	if(p==0)
		c=0;
	if(root==NULL)
		return;
	tc=++c;
	
	if(root->ch == '\n')
			fprintf(fp,"%d[label=\"\\n\\n%d\" color=\"red\"];\n",c,root->n);
	else if(root->ch=='\"')
			fprintf(fp,"%d[label=\"\\%c\\n%d\" color=\"red\"];\n",c,root->ch,root->n);
	else if(root->ch!=0 && root->ch!='\n')
		fprintf(fp,"%d[label=\"%c\\n%d\" color=\"red\"];\n",c,root->ch,root->n);
	else 
		fprintf(fp,"%d[label=\"NT\\n%d\" color=\"blue\"];\n",c,root->n);

	if(p!=0)
	{
		fprintf(fp,"%d->%d",p,tc);
		if(root==root->parent->left)
			fprintf(fp,"[label=\"0\"];\n");
		else
			fprintf(fp,"[label=\"1\"];\n");
	}
//	printf("%d;\n",root->n);
	printdotty(fp,root->left,tc);
	printdotty(fp,root->right,tc);
	return;
}
void dotty(NODE root,char *filename)
{
	FILE *fp;
	fp=fopen(filename,"wb");
	if(fp==NULL)return;
	fprintf(fp,"digraph mvh{\n");
	printdotty(fp,root,0);
	fprintf(fp,"};\n");
	fclose(fp);
	return;
}

// to generate the dictionary
void gendict(NODE root,char *s)
{
	char *ns;
	int n;
	static int now;
	if(root==NULL)
		return;
	n=strlen(s);	
	if(n==0)now=0;
	// if root is leaf node, assign the code to root
	if(root->left==NULL && root->right==NULL)
	{
		n++;
		codes[now]=(struct dict *)malloc(sizeof(struct dict));
		// assign the character to the codebook
		codes[now]->c=root->ch;
		codes[now]->code=(char *)malloc(n*sizeof(char));
		// assign the codeword to codebook
		strcpy(codes[now]->code,s);
		now++;
	}
	// else if root is a non leaf node
	else
	{
		// allocate an extra space for new code and \0
		ns=(char *)malloc((n+2)*sizeof(char));
		
		strcpy(ns,s);
		ns[n+1]='\0';
		// assign 0 and move to left child
		ns[n]='0';
		gendict(root->left,ns);
		// assign 1 and move to right child
		ns[n]='1';
		// and recursively generate dictionary
		gendict(root->right,ns);
	}
	free(root);
	return;
}

// calculates a power n
int power(int a,int n)
{
	int p=1,i;
	for(i=0;i<n;i++)
		p*=a;
	return p;
}

// to write dictionary into a file
void printdict(char *filename)
{
	int i;
	char *dicfile;
	FILE *fp;
	dicfile=(char *)malloc(sizeof(char)*(strlen(filename)+5));
	strcpy(dicfile,filename);
	strcat(dicfile,".dic");
	fp=fopen(dicfile,"wb");
	if(fp==NULL)
		return;

	fprintf(fp,"%d\t%d\n",types,noc);
	for(i=0;i<types;i++)
		fprintf(fp,"%c %s\n",codes[i]->c,codes[i]->code);
}

// to convert bytes to bits
// decreases space occupied
int convert(char *buffer)
{
	int c=0,t=0;
	int i;
	// and operation to convert bytes to bits
	for(i=7;i>=0;i--)
	{
		c += (buffer[7-i]-'0')*pow(2,i);
	}
	return c;
}

// to compress the file
void compress(char *src,char *tgt)
{
	FILE *fs,*ft;
	char ch,buffer[50],cbuffer[9],t;
	int i,codeloc,x;
	fs=fopen(src,"rb");
	ft=fopen(tgt,"wb");
	if(fs==NULL || ft==NULL)return;
	ch=fgetc(fs);
	i=0;
	strcpy(buffer,"");
	while(!feof(fs))
	{
		// copy codeword of a character to a buffer
		for(i=0;i<types;i++)
			if(ch==codes[i]->c)
			{
				strcat(buffer,codes[i]->code);
//				printf("%s",codes[i]->code);		//d
				break;
			}
		// if buffer length is > 8, then
		// convert bytes to bits and write to file
		while(strlen(buffer)>=8)
		{
			strncpy(cbuffer,buffer,8);
			cbuffer[8]='\0';
			// copy back the exceed bits > 8
			for(i=8;buffer[i]!='\0';i++)
				buffer[i-8]=buffer[i];
			buffer[i-8]='\0';
			// convert from bytes to bits
			x=convert(cbuffer);
			fprintf(ft,"%c",x);
		}			
		ch=fgetc(fs);		
	}
	// padding - if last character is left out,
	// it is padded with zeroes
	if(strlen(buffer)>=1)
	{
	strncpy(cbuffer,buffer,8);
	for(i=strlen(cbuffer);i<8;i++)
		cbuffer[i]='0';
	// convert from bytes to bits
	x=convert(cbuffer);
	fprintf(ft,"%c",x);
	}
	slen=ftell(fs);	
	fclose(fs);
	tlen=ftell(ft);
	fclose(ft);
	return;
}

// calculate compression ratio
void cr(char *src,char *tgt)
{
	printf("\n%s : %ld Bytes\n",src,slen);
	printf("%s : %ld Bytes\n",tgt,tlen);
	printf("Compression Ratio = %ld %c\n",100*(slen-tlen)/slen,'%');	
	return;	
}
