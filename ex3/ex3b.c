/* ex3b.c : communication between processes through pipes
 * ===========================================================================
 * Author: Rani Abu Raia
 * ID: 316396787
 * Login: abura
 * ===========================================================================
 * this program generates three child process and each of them generate random
 * numbers and the father process collect all the numbers in the array and print
 * the number of distinct numbers in the array
 * ===========================================================================
 * input: no input
 * output: number of distict numbers
 */

//--------------------include section--------------------------

#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>



//----------------------------constants--------------------------

#define ARR_SIZE 1000 // size of the array
#define NUM_OF_CHILDREN 3 // number of children the father creates
#define SEED 17

//------------------------prototypes------------------------------

void fill_array(int arr[]) ;
bool is_prime(int wanted) ;
void do_son(int arr[], int son_num,int pipefd[]) ;
void qs(int arr[],int l, int r) ;
int partition(int arr[], int l, int r) ;
void print_arr(int arr[],int size) ;
int countDistinct(int arr[], int n);


//-----------------------main----------------------------------

int main()
{
	
    srand(SEED) ; // using a random number generator
    pid_t son ; // son's pid number
    int arr[ARR_SIZE] , // the i/o array
     i, // array's cell number
     pipefd[2] ; // the pipe

    // check if the pipe was created properly
    if (pipe(pipefd) == -1)
    {
        perror("cannot open pipe") ;
        exit(EXIT_FAILURE) ;
    }

    fill_array(arr);
    int childPid[NUM_OF_CHILDREN];
	// create children and send them to their task
	for (i = 0; i < NUM_OF_CHILDREN; i++)
	{
		son = fork() ;
		
		if (son == 0)
		{
			do_son(arr,i,pipefd);
		}
		childPid[i] = son;
	}

	// father's task
	if (son > 0)
	{

		close(pipefd[1]) ; // close the write option - read only
		i = 0;
		ssize_t x ; // number of bytes red from the pipe
		
		// read prime numbers from the pipe
		while (i < ARR_SIZE)
		{
			// send an error message in case of read failure
			x = read(pipefd[0] , & arr[i], sizeof(int));
			if (x < 0)
			{
				perror("read failed") ;
				exit(EXIT_FAILURE) ;
			}
			i++ ;

		}
		
		//killing all child process
		for (int j = 0; j < NUM_OF_CHILDREN; j++)
		{
			int ret;
			ret = kill(childPid[j], SIGKILL);
			if(ret == -1) 
			{
				perror("kill");
				exit(EXIT_FAILURE);
			}
			
		}
		printf("Number of distinct elements: %d\n",countDistinct(arr, ARR_SIZE));
		
	}
    
    
    return 0 ;
}
//.............................................................................
// this function receives the array, son number, the pipe , each son searches
// prime numbers in a different part in the array and writes them to the pipe
// for the father
void do_son(int arr[],
            int son_num,
            int pipefd[])
{
	
    close(pipefd[0]) ; // close read option - only write
    int num;
    // each son search's a different part in the array
    while(1)
    {
    	num = rand() % 999 + 2;
		if (is_prime(num)) // if a prime number was found - write it
		{
			// to the pipe
			if (write(pipefd[1],& num, sizeof(int)) < 0)
			{
				// in case of a write failure send an error message
				//printf("%d ",arr[i]) ;
				perror("write failed") ;
				exit(EXIT_FAILURE) ;
			}
			
		}
    }
    exit(EXIT_SUCCESS) ; // if reached here finish task with success
}
//.............................................................................
// this function receives a wanted number and returns if it is a prime number
// or not
bool is_prime(int wanted)
{
    bool prime = true ;
    int i;
    for (i = 2; i < wanted/2 + 1; i++)
        if (wanted % i == 0)
        {
            prime = false ;
            break ;
        }
    return prime ;
}
//.............................................................................
// this function receives the array and fills it with random numbers
void fill_array(int arr[])
{
    int i;
    for (i = 0; i < ARR_SIZE; i++)
        arr[i] = rand() ;
}
//.............................................................................
// this function receives the array ,a start index, a finish index and
// sorts the array according to the quick sort algorithm
void qs(int arr[],
        int l,
        int r)
{
    int j;

    if (l < r)
    {
        j = partition(arr, l, r);
        qs(arr, l, j - 1);
        qs(arr, j + 1, r);
    }
}
//.............................................................................
// this function receives the array ,a start index, a finish index and
// returns the pivot according to the quick sort algorithm
int partition(int arr[],
              int l,
              int r)
{
    int pivot, i, j, t;
    pivot = arr[l];
    i = l;
    j = r + 1;

    while (1)
    {
        do ++i;
        while (arr[i] <= pivot && i <= r);
        do --j;
        while (arr[j] > pivot);

        if (i >= j)
            break;
        t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }

    t = arr[l];
    arr[l] = arr[j];
    arr[j] = t;
    return j;
}
//.............................................................................
// this function receives the array , a size (which signs the number of prime
// numbers) and prints the prime numbers in the array
void print_arr(int arr[],
               int size)
{
    int i;
    for (i = 0; i < size; ++i)
        printf("%d ",arr[i]) ;
    putchar('\n') ;
}
//.............................................................................
//this function o count distinct elements in a given array
int countDistinct(int arr[], int n)
{
    int res = 1;

    // Pick all elements one by one
    for (int i = 1; i < n; i++) {
        int j = 0;
        for (j = 0; j < i; j++)
            if (arr[i] == arr[j])
                break;
 
        // If not printed earlier, then print it
        if (i == j)
            res++;
    }
    return res;
}