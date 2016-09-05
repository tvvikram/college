#include<stdio.h>
#include<math.h>
#include"mvh.h"

int mvh(char *src,char *tgt,int dot)
{
	// character count
	int *chcnt;
	// huffman tree root
	NODE root;
	chcnt=(int *)calloc(256,sizeof(int));
	treearr=(unsigned char *)calloc(512,sizeof(char));
	// frequency for each character is counted from source file
	readfile(src,chcnt);
	// huffman tree is built
	root=buildtree(chcnt);
	// for dotty display
	if(dot)
		dotty(root,"dotty.dot");
	// contains codebook
	codes=(struct dict **)malloc(types*sizeof(struct dict*));
	// dictionary is generated
	gendict(root,"");
	// write to dictionary file
	printdict(tgt);
//	small();
	// compress and write to the target
	compress(src,tgt);
	// calculate compression ratio
	cr(src,tgt);
	return 0;
}
