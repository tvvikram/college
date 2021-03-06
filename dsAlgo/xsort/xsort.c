
	/* Program to 'x' sort an array */

#include<stdlib.h>
#include<stdio.h>
#define SIZE 100000			/* Maximum size of the array */

//Pre - an array and its size to fill with random numbers
//Post- random numbers in the array
int genRand(int a[], int n)	{
	int i;
	int seed;

	srand(2);			/* default seed - 2 */	
	/* Release the underlying comment to start entering your own 'seed' */
//printf("\nEnter a seed to generate random numbers: "); scanf("%d", &seed); srand(seed);
	
	printf("Random numbers generated... ");
	for (i = 0; i < n; i++)
		a[i] = rand();		/* random numbers generating to the array */
	return 0;			/* on successful generation */
}

//Pre - an integer array with its size
//Post- formatted print
int printArray(int a[], int n)	{
	int i;

	for (i = 0; i < n; i++)
		printf("%d\t", a[i]);	/* printing the contents of the array */
	printf("\n");
	return 0;			/* on successful printing of array elements */
}

//Pre - an array with its size
//Post- checks whether the array is sorted
int checkSort(int a[], int n)	{
	int i;
	for (i = 0; i < n; i++)
		if (a[i] > a[i + 1])
			return 1;	/* on unsuccessful test - array being unsorted */
		else
			return 0;	/* on success of sort test */
}

//Pre - an array with its size
//Post- 'x' sorts an array -- 'x' being a unnamed sort
int xSort(int a[], int n)	{
	int count = 0, i, k;

	for (k = 0; k < n/2; k++)	{
		if (count % 2 == 0)
			i = 0;				/* assign 0 to i, to start an even sort */
		else
			i = 1;				/* assign 1 to i, to start an odd sort  */
	
		for ( ; i < n - 1; i = i + 2)/* increment i by 2, to move to next two elements */
		{
			if (a[i] > a[i + 1])	{	/* swapping of elements */
				int temp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = temp;
			}
		}
		count = count + 1;	/* increment count to alternate between even and odd sorts */
	}
	return 0;
}

//Pre - main with the calls to sort an array of maximum size 'SIZE'
//Post- 'x' sorting of an array
int main()	{
	int a[SIZE], i;
	
	genRand(a, SIZE);		/* call to generate random numbers */

	xSort(a, SIZE);			/* call to xsort the elements */
	
	if (!checkSort(a, SIZE))	/* success of sort */
		printf("\nNon - descendingly sorted.");
	else
		printf("\nNot non - descendingly sorted.");
	printf("\n");
	return 0;
}
