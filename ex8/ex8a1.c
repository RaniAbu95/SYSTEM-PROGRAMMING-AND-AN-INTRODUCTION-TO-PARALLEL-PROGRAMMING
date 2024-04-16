/*
 *  ex8a1.c:
 *  Semaphores.
 *  ===========================================================================
 *  Written by Rani Abu Raia, id: 316396787, login: abura
 *  
 *  Desciption:
 *  This program defines an shared memory array of int numbers of a thousand
 *  cells. the program wait tell other programs fill all cells of the
 *  array. When the array is full the prog prints the needed output.
 *
 *  -Input: NONE.
 *  -Output: distinct , min and ma numbers in the array.
 *
 *  Compile: gcc -Wall ex8a1.c -o ex8a1 -lpthread
 *  Run: ./ex8a1
 */
//-------------Includes--------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
//-------------Consts----------------------------------------------------------
#define ARR_SIZE 1000
#define FIRST 4
const int ZERO = 0;
const char* MUTEX_NAME = "/my_mutex20";
//-------------Global----------------------------------------------------------
key_t key ;
int shm_id ;
sem_t * mutex;
//-------------Prototypes------------------------------------------------------
void create_keys();
void check_ptr(int *shm_ptr_pf);
void init_ptr(int* shm_ptr);
void sig_handler(int signo);
void initialize(key_t* key, int* shm_id, int** shm_ptr);
void delete(int shm_id, int* shm_ptr);
void delete_shm();
int count_distinct(int arr[]);
int min_val(int* shm_ptr);
int max_val(int* shm_ptr);
//-------------Main-------------------------------------------------------
int main()
{
	signal(SIGUSR1,sig_handler);//signal handler to continue 
	create_keys();
	int *shm_ptr ; // create shared memory
	
	shm_ptr = (int *) shmat (shm_id , NULL, 0);
	check_ptr(shm_ptr);
	
	mutex = sem_open(MUTEX_NAME, O_CREAT, 0644, 1);
	if (mutex == SEM_FAILED) {
		perror("parent sem_open failed (my_mutex)");
		return EXIT_FAILURE;
	}
	sem_post(mutex) ; // initialize the mutex to 1
	init_ptr(shm_ptr);
	pause();
		
	printf("%d %d %d\n", 
	count_distinct(shm_ptr), 
	max_val(shm_ptr), 
	min_val(shm_ptr));//print all the output needed
	
	// delete shared memory
	shmdt(shm_ptr);
	delete_shm(shm_ptr);
	sem_close(mutex);
	sem_unlink(MUTEX_NAME);
	exit (EXIT_SUCCESS);
}
//-----------------------------------------------------------------------------
void initialize(key_t* key, int* shm_id, int** shm_ptr)
{
	if ((*key = ftok(".", '8')) == -1)
	{
		perror("ftok() failed\n");
		exit(EXIT_FAILURE);
	}

	if ((*shm_id = shmget(*key, ARR_SIZE, IPC_CREAT | 
	IPC_EXCL | 0600)) == -1)
	{
		perror("shmget failed\n");
		exit(EXIT_FAILURE);
	}

	*shm_ptr = (int*)shmat(*shm_id, NULL, 0);
	if (*shm_ptr == (int*)-1)
	{
		perror("shmat failed()\n");
		exit(EXIT_FAILURE);
	}
}
//-----------------------------------------------------------------------------
void delete(int shm_id, int* shm_ptr)
{
	shmdt(shm_ptr);

	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		perror("shmctl failed\n");
		exit(EXIT_FAILURE);
	}
}
//-----------------------------------------------------------------------------
/*
 * create interal and external keys
 */
void create_keys()
{
	if ((key = ftok(".", '8')) == -1)
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
//-----------------------------------------------------------------------------
/*
 * check_ptr checks if allocated pointers are okay to use
 */
void check_ptr(int *shm_ptr_pf)
{
	if (shm_ptr_pf == (int *) -1)
	{
		perror("shmat failed");
		exit( EXIT_FAILURE );
	}
}

/*
 * this functions updates the giving array cells
 */
void init_ptr(int* shm_ptr)
{

	shm_ptr[0] = getpid();
	//sleep(5);
	for(int index = 1; index < ARR_SIZE; index++) shm_ptr[index] = ZERO;
	//sleep(1);
}
// this func counts how many unique and distinct numbers there are in the array
// it returns the result

int count_distinct(int arr[])
{
    int res = 0;
    int i,j;
    // Pick all elements one by one
    for (i = FIRST; i < ARR_SIZE; i++) 
    {
        for (j = FIRST + 1; j < i; j++)
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
    int max = *(shm_ptr + FIRST);
    
    for (i = FIRST+1; i < ARR_SIZE; i++)
        if (*(shm_ptr + i) > max)
            max = *(shm_ptr + i);
 
    return max;
}
//-----------------------------------------------------------------------------
/*
 * a func to finc the minimal value in the array
 */

int min_val(int* shm_ptr)
{
	int i;
    int min = shm_ptr[FIRST];
    
    for (i = FIRST+1; i < ARR_SIZE; i++)
        if (*(shm_ptr + i) < min)
            min = *(shm_ptr + i);
 
    return min;
}
//-----------------------------------------------------------------------------
/*
 * delete the shared memory.
 */
void delete_shm()
{
		if (shmctl(shm_id,IPC_RMID, NULL) == -1)
	{
		perror( "shmctl failed" );
		exit( EXIT_FAILURE );
	}
}
//-----------------------------------------------------------------------------
/*
 * signal handler that recieves a signal when the shared memory array is filled
 */
void sig_handler(int signo)
{	
	signal(SIGUSR1,sig_handler);
	if(signo==SIGUSR1);
}
