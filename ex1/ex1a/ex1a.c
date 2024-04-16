/*
 * ex1a.c
 *
 *  Created on: Oct 31, 2021
 *      Author: abura
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define ARRAY_SIZE 50000
#define LOOP_TIME 50

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

/*
 * main function
 */
int main(int argc, char** argv)
{
	struct timeval b0,b1,q0,q1,t0,t1;
	gettimeofday(&t0,NULL);
	double bubble_time;
	double quick_time;
	FILE * out = fopen(argv[1],"w");
	static int array[ARRAY_SIZE];
	srand(atoi(argv[2]));
	pid_t x,y;
	for(int k = 0; k < LOOP_TIME; k ++)
	{
		for(int i = 0; i < ARRAY_SIZE; i++)
		{
			array[i] = rand();

		}
		x= fork();
		if(x == 0)
		{
			gettimeofday(&b0,NULL);
			BubbleSort(array,ARRAY_SIZE);
			gettimeofday(&b1,NULL);
			bubble_time =  (double) (b1.tv_usec - b0.tv_usec)/1000000 + (double)(b1.tv_sec - b0.tv_sec);
			fprintf(out,"%c %f\n",'b',bubble_time);
			exit(0);
		}
		y= fork();
		if(y == 0)
		{
			gettimeofday(&q0,NULL);
			quickSort(array,0,ARRAY_SIZE - 1);
			gettimeofday(&q1,NULL);
			quick_time =  (double) (q1.tv_usec - q0.tv_usec)/1000000 + (double)(q1.tv_sec - q0.tv_sec);
			fprintf(out,"%c %f\n",'q',quick_time);
			exit(0);
		}
		waitpid(x,NULL,0);
		waitpid(y,NULL,0);
	}
	float arrB[LOOP_TIME];
	float arrQ[LOOP_TIME];
	char s[1];
	float time;
	int indexB = 0;
	int indexQ = 0;
	FILE * in = fopen(argv[1],"r");

	while (fscanf(in, "%s %f",s,&time) != EOF)
	{
		if(s[0] =='b')
		{

			arrB[indexB] = time;
			indexB++;
		}
		if(s[0] == 'q')
		{
			arrQ[indexQ] = time;
			indexQ++;
		}
	}

	float avgB = 0;
	float minB =0;
	float maxB = 0;
	float avgQ = 0;
	float minQ =0;
	float maxQ = 0;

	avgB = avgB / LOOP_TIME;
	printf("Bubble sort average running time: %f\n",avgB);
	quickSort(arrB,0,LOOP_TIME - 1);
	minB = arrB[0];
	maxB = arrB[LOOP_TIME - 1];
	printf("Bubble sort max running time: %f\n",maxB);
	printf("Bubble sort min running time: %f\n",minB);


	avgQ = avgQ / LOOP_TIME;
	printf("Quick sort average running time: %f\n",avgQ);
	quickSort(arrQ,0,LOOP_TIME - 1);
	minQ= arrQ[0];
	maxQ = arrQ[LOOP_TIME - 1];
	printf("Quick sort max running time: %f\n",maxQ);
	printf("Quick sort min running time: %f\n",minQ);

	fclose(in);
	unlink(argv[1]);

	gettimeofday(&t1,NULL);
	double father_time;
	father_time =  (double) (t1.tv_usec - t0.tv_usec)/1000000 + (double)(t1.tv_sec - t0.tv_sec);
	printf("Father runtime is: %f\n",father_time);
}
