

	// Sorting utilities to generate Random Numbers, to print array, check the sorted elements


//Pre - an array and its size to fill with random numbers
#include"sortdefinitions.h"

using namespace std;

class SORT : public sortdefinitions
{
	int *a;			// pointer to array
	static int length;
	public:
	
	int genRand(int);
	int checkSort();
	int * getarray()
	{
	    return a;
	}
	int getlength()
	{
	    return length;
	}
	friend int printArray(SORT);
};



	
int SORT::length = 0;			// static initialization

//Pre - uninitialized array
//Post- random numbers in the array
int SORT::genRand(int OFFSET)	{
	int i;
	int seed;

	srand(2);			/* default seed - 2 */	
	/* Release the underlying comment to start entering your own 'seed' */
//	printf("\nEnter a seed to generate random numbers: "); scanf("%d", &seed); srand(seed);
	

	length = length + OFFSET;
	a = new int[length];
//	printf("Random numbers generated...\n");
	for (i = 0; i < length; i++)
		a[i] = rand();		/* random numbers generating to the array */
	return 0;			/* on successful generation */
}

//Pre - an integer array with its size
//Post- formatted print
int printArray(SORT array)	{
	int i;
	cout << endl;
	for (i = 0; i < array.length; i++)
		printf("%d\t", array.a[i]);	/* printing the contents of the array */
	printf("\n");
	return 0;				/* on successful printing of array elements */
}

//Pre - an array with its size
//Post- checks whether the array is sorted
int SORT::checkSort()	{
	int i;
	for (i = 0; i < length - 2; i++)
		if (a[i] > a[i + 1])
			return 1;	/* on unsuccessful test - array being unsorted */
	return 0;			/* on success of sort test */
}

