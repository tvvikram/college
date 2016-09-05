
	/* Codes for all the sorting algorithms - can be included with any file to sort numbers */

class sortdefinitions
{
	public:
	int left(int);
	int right(int);
	int maxHeapify(int , int, int *);
	int buildMaxHeap(int , int *);
	int heapSort(int, int *);
	void bubbleSort(int , int *);
	void selectionSort(int , int *);
	int partition(int , int , int *);
	void quickSort(int , int , int *);	
};	


			/*<<<<< Codes for 'heap' sorting integer elements >>>>>*/
//Returns the index of the left child of a node in a tree
inline int sortdefinitions::left(int i)
{
	return (2 * i + 1);
}

//Returns the index of the right child of a node in a tree
inline int sortdefinitions::right(int i)
{
	return (2 * i + 2);
}

//MaxHeapifies a given array of length 'n' below node 'i'
int sortdefinitions::maxHeapify(int i, int n, int *a)
{
	int l = left(i), r = right(i), large;
	if (l <= n && a[l] > a[i])	// setting largest element as parent node
		large = l;
	else
		large = i;
	if (r <= n && a[r] > a[large])
		large = r;
	if (large != i)
	{
		int temp = a[i];	// swapping
		a[i] = a[large];
		a[large] = temp;
		maxHeapify(large, n, a);
	}
	return 0;		// on success of maxHeap
}

//Builds maxHeap by successively calling maxHeapify()
int sortdefinitions::buildMaxHeap(int n, int *a)
{
	for (int i = n/2; i >= 0; i--)
		maxHeapify(i, n, a);
	return 0;		// on success of buildMaxHeap
}

//HeapSorts a given array of length 'n'
int sortdefinitions::heapSort(int n, int *a)
{	
	buildMaxHeap(n, a);
	for (int i = n; i >= 1; i--)
	{
		int temp = a[0];	// swapping first and last elements
		a[0] = a[i];
		a[i] = temp;
		n = n - 1;		// decrementing heap length
		maxHeapify(0, n, a);
	}
	return 0;		// on success of heapSort
}
			/*>>>>> End of codes for 'heap' sorting integer elements <<<<<*/


			/*<<<<< Codes for 'bubble' sorting integer elements >>>>>*/
//Bubble sorts elements of given array with 'n' elements
void sortdefinitions::bubbleSort(int n, int *a)
{
	int i, j, temp;
    	for(i = 1; i < n; i++)			
   	 for(j = 0; j < n - i; j++)
   		if(a[j] > a[j+1])
   		{				
   			temp = a[j];		// swapping
        		a[j] = a[j+1];
        		a[j+1] = temp;
       	 	}
}
			/*>>>>> End of codes for 'bubble' sorting integer elements <<<<<*/



			/*<<<<< Codes for 'select' sorting integer elements >>>>>*/
void sortdefinitions::selectionSort(int n, int *a)
{
	int i, j, pos, temp;
	for(i = 0;i < n - 1; i++)   		//Number of passes
    	{
		pos = i;            
       		for(j = i + 1; j < n; j++)  	//Number of comparisions
         		if(a[j] < a[pos])  
              			pos = j;  	//Position of smalles element
		if (pos != i)
		{
			temp = a[pos];    	//Swapping
	      		a[pos]=a[i];    
		      	a[i] = temp;
		}
	}
}
			/*>>>>> End of codes for 'select' sorting integer elements <<<<<*/



			/*<<<<< Codes for 'quick' sorting integer elements >>>>>*/
int sortdefinitions::partition(int p,int r, int *a)
{
	int x, i, j, t;
	x = a[(p + r) / 2];		// taking middle element as pivot
	i = p - 1;
	j = r + 1;
	while (1)
	{
		do
		{
			i++;
		}while(a[i] < x);	// compare till element to left is smaller than pivot
		do
		{
			j--;
		}while(a[j] > x);	// compare till element to right is greater than pivot
		if(i < j)
		{
			t = a[i];	// swapping
			a[i] = a[j];
			a[j] = t;
		}
		else
		 return j;		// returning pivot index
	}
}
void sortdefinitions::quickSort(int p,int r, int *a)
{
	int q;
	if(p < r)
	{
		q = partition(p, r, a);	// getting Pivot
		quickSort(p, q, a);	// Sorting again from  'p' to 'q'
		quickSort(q + 1, r, a); // Sorting again from 'q + 1' to 'r'
	}
	
}
			/*>>>>> End of codes for 'quick' sorting integer elements <<<<<*/
