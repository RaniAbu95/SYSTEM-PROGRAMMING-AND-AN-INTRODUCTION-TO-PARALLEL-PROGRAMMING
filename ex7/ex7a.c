/*
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program works as a array program, the program has
 * 				about 50000 random numbers, the program gets prime numbers
 * 				from clients "threads" to add. when array is full each thread
 * 				stop running and print what is the is the most common number 
 * 				found in the array from all numbers was sent and how many new
 * 				numbers where send. and after all threads finish the main prog
 * 				prints how many distinct numbers in the array what is the max
 * 				and the min and exit.
 *
*/
// --------------INCLUDE--------------------------------------------------------
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
// --------------CONST----------------------------------------------------------
#define ARR_SIZE 50000
const int RANGE = 999;
const int SEED = 17;
const int LOCK = 0;
const int EMPTY = 0;
const int FOUND = 1;
const int FULL = -1;
const int NOT_FULL = 1;
const int CLOSED = 1;
const int OPEN = 0;
const int NOT_EMPTY = 1;
const int NOT_FOUND = -1;
int arr[ARR_SIZE] = {0};
const int FIRST = 1;
const int SECOND = 2;

// --------------prototype----------------------

void check_status(int status1, int status2, int status3);
void * do_pthread(void * n);
int check_n_update(int random);
int check_arr();
bool is_prime(int wanted);
int count_distinct(int arr[]);
int min_val(int* arr);
int max_val(int* arr);
//--------------------------------------------------------------------------

int main()
{
	pthread_t thread_data1, thread_data2, thread_data3 ;
	srand(SEED);
	int status1 = pthread_create(&thread_data1, NULL, do_pthread, NULL);
	int status2 = pthread_create(&thread_data2, NULL, do_pthread, NULL);
	int status3 = pthread_create(&thread_data3, NULL, do_pthread, NULL);
	check_status(status1, status2, status3);


	pthread_join(thread_data1, NULL);
	pthread_join(thread_data2, NULL);
	pthread_join(thread_data3, NULL);
	

	printf("%d %d %d\n", count_distinct(arr),min_val(arr),max_val(arr));
	pthread_exit(EXIT_SUCCESS);
}
//--------------------------------------------------------------------------
/*
 * check status function checks if all threads are created
 */
void check_status(int status1, int status2, int status3)
{
	if(status1 !=0 || status2 != 0 || status3 != 0)
	{
		fputs("pthread create failed", stderr);
		exit(EXIT_FAILURE);
	}
	return;
}
//--------------------------------------------------------------------------
/*
 * do pthread function raffles off numbers and checks if found in arr array,
 * it continuous to check numbers until all arr array numbers are found.
 */
void * do_pthread(void * n)
{
	int max_feedback = 0,new_numbers = 0;
	while(NOT_EMPTY)
	{
		if(arr[LOCK] == OPEN)
		{

			arr[LOCK] = CLOSED;
			int random = rand() % RANGE +2;
			while(!is_prime(random)) random = rand() % RANGE +2;
			int feedback = check_n_update(random);
			
			if(feedback > 0)
			{
				if(feedback > max_feedback) max_feedback = feedback;

			}
			else if(feedback == 0) new_numbers++;

			if(feedback == FULL)
			{
				arr[LOCK] = OPEN;
				break;
			}
			arr[LOCK] = OPEN;
		}
	}
	printf("%d %d\n", new_numbers,max_feedback);
	pthread_exit(EXIT_SUCCESS);
	return NULL;
}
//--------------------------------------------------------------------------
/*
 * check_n_update function checks if "random" number is in the "arr" or not.
 * if "random" is found, the number is changed to 0.
 */
int check_n_update(int random)
{
	if(check_arr() == FULL) return FULL;
	int i;
	int found = 0;
	for(i = 1; i < ARR_SIZE && arr[i] != 0 ; i++)
	{
		if(random == arr[i]) found++;
	}
	
	arr[i] = random;
	if(found) return found+1;

	return found;
}
//--------------------------------------------------------------------------
/*
 * check_arr checks if the arr array or empty or not
 */
int check_arr()
{
	if(arr[ARR_SIZE - 1] == 0) return NOT_FULL;
	
	return FULL;
}
/*
 *  this function receives a wanted number and returns if it is a prime number
 *  or not
 */
bool is_prime(int wanted)
{
    bool prime = true ;
    int i;
    for (i = 2; i < wanted/2 + 1; i++)
        if (wanted % i == 0)
        {
            prime = false ;
            break;
        }
    return prime;
}
/*
 * this func counts how many unique and distinct numbers there are in the array
 *  it returns the result
 */ 
int count_distinct(int arr[])
{
    int res = 1;
    int i,j;
    // Pick all elements one by one
    for (i = 2; i < ARR_SIZE; i++) 
    {
        for (j = 1; j < i; j++)
            if(arr[i] == arr[j])
                break;
        if (i == j)
        	res++;
        
            
    }
    return res;
}

/*
 * a func to finc the maximal value in the array
 */
int max_val(int* arr)
{
	int i;
    int max = arr[FIRST];
    for (i = SECOND; i < ARR_SIZE; i++)
        if (*(arr + i) > max)
            max = *(arr + i);
 
    return max;
}
//-----------------------------------------------------------------------------
/*
 * a func to finc the minimal value in the array
 */
int min_val(int* arr)
{
	int i;
    int min = arr[FIRST];
    
    for (i = SECOND; i < ARR_SIZE; i++)
        if (*(arr + i) < min)
            min = *(arr + i);
 
    return min;
}



