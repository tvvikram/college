#include<stdio.h>
#include"dmvh.h"

int dmvh(char *src,char *tgt)
{
	// read dictionary for decompression
	getdict(src);
	// decompress the file to the target
	decompress(src,tgt);
	return 0;
}
