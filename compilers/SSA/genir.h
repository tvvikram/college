#include <stdio.h>

#define MAX_DEPTH 3
#define NEST_SIZE_MAX 2  // A random size b/w MAX and MIN is picked for the size of nested statements
#define NEST_SIZE_MIN 1

void genir_random (int current_depth, int required_generations, int blockNo);
