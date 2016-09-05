#include <stdio.h>
#include <string.h>

struct boolean {
	int t;
	int f;
	char *code;
};

struct statement {
	int next;
	char *code;
};

struct expression {
	char *addr;
	char *code;
};
