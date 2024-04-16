/*
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program works with shared data,
 * its a server which creae array of 995 number and 
 * the client should fill it with prime numbers.
 * after the array is full the prog prints how much
 * distinct numbers it have what is the min and max.
 *
*/
// --------------INCLUDE------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// --------------CONST--------------------------
const int ARR_SIZE = 1000;
const int RANGE = 199999;
const int ZERO = 0;
const int ONE = 1;
const int N = 1000;
const int TRUE = 1;
const int END = -1;
const int FALSE = 0;
const int FULL = 1;
const int EMPTY = 0;
key_t key ;
int shm_id ;
// --------------prototype----------------------
void create_keys();
void check_ptr(int *shm_ptr_pf);
void init_ptr(int* shm_ptr);
void delete_shm();
int count_distinct(int arr[]);
int min_val(int* shm_ptr);
int max_val(int* shm_ptr);
void sig_handler(int signo);
//--------------------------------------------------------------------------
int main()
{
	// wait for signal to continue process
	signal(SIGUSR1,sig_handler);//signal handler to continue 
	// make external key
	create_keys();
	// create shared memory
	int *shm_ptr ;
	shm_ptr = (int *) shmat (shm_id , NULL, 0);
	check_ptr(shm_ptr);
	
	init_ptr(shm_ptr);
	pause();
	
	printf("%d %d %d\n", 
	count_distinct(shm_ptr), 
	max_val(shm_ptr), 
	min_val(shm_ptr));//print all the output needed
	
	// delete shared memory
	shmdt(shm_ptr) ;
	delete_shm();
	return EXIT_SUCCESS;
	
}
//--------------------------------------------------------------------------

//signal handler that recieves a signal when the shared memory array is filled
void sig_handler(int signo)
{	
	signal(SIGUSR1,sig_handler);
	if(signo==SIGUSR1);
}
//--------------------------------------------------------------------------
/*
 * create interal and external keys
 */
void create_keys()
{
	if ((key = ftok(".", '5')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// make internal key
	shm_id = shmget(key, ARR_SIZE, IPC_CREAT | IPC_EXCL | 0600) ;

	if (shm_id == -1)
	{
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}
}
//--------------------------------------------------------------------------
/*
 * check_ptr checks if allocated pointers are okay to use
 */
void check_ptr(int *shm_ptr_pf)
{
	if (shm_ptr_pf == (int *) -1)
	{
		perror( "shmat failed" ) ;
		exit( EXIT_FAILURE ) ;
	}
}
//--------------------------------------------------------------------------
/*
 * this functions updates the giving array cells
 */
void init_ptr(int* shm_ptr)
{
	shm_ptr[0] = getpid();
	shm_ptr[1] = ZERO;
	shm_ptr[2] = ZERO;
	shm_ptr[3] = ZERO;
	shm_ptr[4] = ONE;
}
//--------------------------------------------------------------------------
void delete_shm()
{
		if (shmctl(shm_id,IPC_RMID, NULL) == -1)
	{
		perror( "shmctl failed" );
		exit( EXIT_FAILURE );
	}
}

// this func counts how many unique and distinct numbers there are in the array
// it returns the result

int count_distinct(int arr[])
{
    int res = 1;
    int i,j;
    // Pick all elements one by one
    for (i = 5; i < ARR_SIZE; i++) 
    {
        for (j = 5; j < i; j++)
            if(arr[i] == arr[j])
                break;
        if (i == j)
            res++;
    }
    return res;
}

//-----------------------------------------------------------------------------

// a func to finc the maximal value in the array
int max_val(int* shm_ptr)
{
	int i;
    int max = *(shm_ptr + 5);
    
    for (i = 6; i < ARR_SIZE; i++)
        if (*(shm_ptr + i) > max)
            max = *(shm_ptr + i);
 
    return max;
}

//-----------------------------------------------------------------------------

// a func to finc the minimal value in the array
int min_val(int* shm_ptr)
{
	int i;
    int min = shm_ptr[5];
    
    for (i = 6; i < ARR_SIZE; i++)
        if (*(shm_ptr + i) < min)
            min = *(shm_ptr + i);
 
    return min;
}

	
