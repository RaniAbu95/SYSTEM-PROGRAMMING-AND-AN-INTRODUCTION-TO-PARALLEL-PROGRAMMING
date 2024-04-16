/*
 * OS1, 2022
 * ex3a.c: Pipe
 *
 *  Created on: Nov 16, 2021
 *  	By: Rani Abu Raia
 *      Login: abura
 *      ID: 316396787
 *-----------------------------description------------------------------------
 * Father process create two child process, one uses Bubble sort and the   
 * second uses Quick sort to sort array of size 50,000 and repeats this 50 times.
 * each child write his result to the stdout and to the pipe. The father process
 * reads the messages that he recieved from the pipe and calculate the max,min
 * and average sorting time of each process.
 * In addition the father calculate its runtime of its self.
 */
 //-----------------------------include section--------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

//-----------------------------const section-----------------------------------
#define ARRAY_SIZE 50000
#define LOOP_TIME 50
#define CHILDREN 2

//-------------------------------prototypes------------------------------------
void swap(int* a, int* b);
void BubbleSort(int* arr, int n);
int partition(int array[],int low, int high);
void quickSort(int array [], int low,int high);

//------------------------------main section-----------------------------------
int main(int argc, char** argv)
{
	if(argc > 2)
	{
		perror ("bad input") ;
		exit(EXIT_FAILURE) ;
	}
	int input = atoi(argv[1]);
	if(input < 0)
	{
		perror ("bad input") ;
		exit(EXIT_FAILURE) ;
	}
	srand(input);
	struct timeval b0,b1,q0,q1,t0,t1;
	gettimeofday(&t0,NULL);
	double bubble_time;
	double quick_time;
	static int array[ARRAY_SIZE];
	float arrB[LOOP_TIME];
	float arrQ[LOOP_TIME];
	
	int indexB = 0;
	int indexQ = 0;
	for(int k = 0; k < LOOP_TIME; k ++)
	{
		int pipe_descs[2] ;
		if (pipe(pipe_descs) == -1) 
		{
			perror ("cannot open pipe") ;
			exit(EXIT_FAILURE) ;
		}
		pid_t pid[CHILDREN];
		for(int j = 0; j < ARRAY_SIZE; j++)
		{
			array[j] = rand();
			
		}
	    for (int i = 0; i < CHILDREN; i++)
	    {
	    	
			pid[i] = fork();// create child process
			if (pid[i] < 0)
			{
				perror("error in fork");
				exit(EXIT_FAILURE);
			}
			if (pid[i] == 0)
			{
				close(pipe_descs[0]);
				if(i==0)
				{
					gettimeofday(&b0,NULL);
					BubbleSort(array,ARRAY_SIZE);
					gettimeofday(&b1,NULL);
					bubble_time =  (double) (b1.tv_usec - b0.tv_usec)/1000000 + (double)(b1.tv_sec - b0.tv_sec);
					
					printf("b %lf\n",bubble_time);
					
					if(write(pipe_descs[1], &bubble_time, sizeof(double)) == -1)
					{
						perror("error in write");
						exit(EXIT_FAILURE) ;
					}
					
					
					close(pipe_descs[1]) ;

				}
				else
				{
					gettimeofday(&q0,NULL);
					quickSort(array,0,ARRAY_SIZE - 1);
					gettimeofday(&q1,NULL);
					quick_time =  (double) (q1.tv_usec - q0.tv_usec)/1000000 + (double)(q1.tv_sec - q0.tv_sec);
					printf("q %lf\n",quick_time);
					close(pipe_descs[0]);
					if(write(pipe_descs[1], &quick_time, sizeof(double)) == -1)
					{
						perror("error in write");
						exit(EXIT_FAILURE) ;
					}
					close(pipe_descs[1]) ;
					
				}
				exit(EXIT_SUCCESS) ;
				
			}
			
	    }
		int nbytes ;
		double Tq;
		double Tb;
		
		close(pipe_descs[1]);
		int fstatus;	// first child to finish
		fstatus = wait(NULL);	// wait for any child


		if(fstatus == pid[0])	// child 'x' finished first
		{
			nbytes = read(pipe_descs[0], &Tb, sizeof(double));
			if(nbytes == -1)
			{
				perror("error in read");
				exit(EXIT_FAILURE) ;
			}
			arrB[indexB] = Tb;
			indexB++;

			waitpid(pid[1], NULL, 0);	// wait for child 'y'
			nbytes = read(pipe_descs[0], &Tq, sizeof(double));
			if(nbytes == -1)
			{
				perror("error in read");
				exit(EXIT_FAILURE) ;
			}
			arrQ[indexQ] = Tq;
			indexQ++;
		}
		else	// child 'a' finished first
		{
			nbytes = read(pipe_descs[0], &Tq, sizeof(double));
			if(nbytes == -1)
			{
				perror("error in read");
				exit(EXIT_FAILURE) ;
			}
			arrQ[indexQ] = Tq;
			indexQ++;
			waitpid(pid[0], NULL, 0); // wait for child 'a'
			nbytes = read(pipe_descs[0], &Tb, sizeof(double));
			if(nbytes == -1)
			{
				perror("error in read");
				exit(EXIT_FAILURE) ;
			}
			arrB[indexB] = Tb;
			indexB++;
		}

		close(pipe_descs[0]) ;
	    
	}

	float minB = ARRAY_SIZE;
	float maxB = 0;
	double avgB = 0;
	float minQ = ARRAY_SIZE;
	float maxQ = 0;
	double avgQ = 0;
	
	for(int i = 0; i < LOOP_TIME; i++)
	{
		avgB += arrB[i];			
			
		if (minB >  arrB[i])
		{
			minB =  arrB[i];	
		}
		
		if (maxB <  arrB[i])
		{
			maxB =  arrB[i];	
		}
	}
	
	
	for(int i = 0; i < LOOP_TIME; i++)
	{
		avgQ += arrQ[i];			
			
		if (minQ >  arrQ[i])
		{
			minQ =  arrQ[i];	
		}
		
		if (maxQ <  arrQ[i])
		{
			maxQ =  arrQ[i];	
		}
	}
	
	// avarge time of each sort
	printf("%lf %lf\n", avgB/LOOP_TIME, avgQ/LOOP_TIME);
	
	// fastest time of each sort
	printf("%lf %lf\n", minB, minQ);
	
	// sloweset time of each sorts
	printf("%lf %lf\n", maxB, maxQ);	
	

	gettimeofday(&t1,NULL);
	double father_time;
	father_time =  (double) (t1.tv_usec - t0.tv_usec)/1000000 + (double)(t1.tv_sec - t0.tv_sec);
	// time that takes to father to run
	printf("%lf\n",father_time);
}


/*
 * swaps between two int variables
 */
void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

/*
 * Bubble sort algorithm
 */
void BubbleSort(int* arr, int n)
{
	for(int i=0; i< n-1; i++)
	{
		for(int j =0; j < n-i-1; j++)
		{
			if(arr[j] > arr[j+1])
			{
				swap(&arr[j],&arr[j+1]);
			}
		}
	}
}

/*
 * helper function for Quick sort algorithm
 */
int partition(int array[],int low, int high)
{
	int pivot = array[high];
	int i = (low-1);
	for(int j=low;j<high;j++)
	{
		if(array[j] <=pivot)
		{
			i++;
			swap(&array[i],&array[j]);
		}
	}
	swap(&array[i+1],&array[high]);
	return i + 1;
}

/*
 * Quick sort algorithm
 */
void quickSort(int array [], int low,int high)
{
	if(low < high)
	{
		int pi = partition(array,low,high);
		quickSort(array,low,pi-1);
		quickSort(array,pi+1,high);
	}
}


