/*
 *  ex8a2.c:
 *  Semaphores.
 *  ======================================================================
 *  Written by Rani Abu Raia, id: 316396787, login: abura
 *  
 *	Desciption:
 *  This program creates 3 different copies to be run.
 *  Each program picks random numbers and store it the shared memory array.
 *  The numbers store is managed in turns and is controled by the semaphore:
 *  When the mutex is opened one program can enter the critical code, and
 *  locks the key for itself, at the same time other programs is waiting,
 *  when the program that holds the key is finish it open the key again the
 *  next program enters, doing this tell the array is full. when the array
 *  is full all programs prints the output needed. and searches for it in
 *  the array and deletes it.
 *
 *  -Input: the program id.
 *  -Output: how many values was new in the shared memory,
 *       	 and how many of all the numbers send was appeared the most in 
 *       	 the shared memory array.
 *
 *  Compile: gcc -Wall ex8a2.c -o ex8a2 -lpthread
 *  Run: ./ex8a2 <PROGRAM_ID>
 */
//-------------Includes---------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
//-------------Consts-----------------------------------------------------
#define ARR_SIZE 1000
#define FIRST 4
const int TRUE = 1;
const int ZERO = 0;
const int RANGE = 999;
const int N = 1000;
const int FALSE = 0;
const int RUNNING = 1;
const int FIRST_PROG = 1;
const int SECOND_PROG = 2;
const int THIRD_PROG = 3;
const char* MUTEX_NAME = "/my_mutex20";
//-------------Global-----------------------------------------------------
key_t key;
int shm_id;
sem_t* mutex;
//-------------Prototypes-------------------------------------------------
void mainnFunc(int i1);
void initialize(key_t* key, int* shm_id, int** shm_ptr);
bool is_prime(int wanted);
int count_ocur(int prime_num, int *shm_ptr);
void add_num(int arr[],int num);
void create_keys();
void addNew(int arr[],int num);
int max_apperance(int shm_ptr[],int addedNumbers[]);
//-------------Main--------------
int main(int argc, char* argv[])
{
	create_keys();
	mutex = sem_open(MUTEX_NAME, 0);
	if (mutex == SEM_FAILED) 
	{
		perror("parent sem_open failed (my_mutex20)");
		exit(EXIT_FAILURE);
	}
	
	mainnFunc(atoi(argv[1]));
	exit(EXIT_SUCCESS);
}
//------------------------------------------------------------------------

void mainnFunc(int i1)
{
	
	int shm_id, * shm_ptr;
	key_t key;
	initialize(&key, &shm_id, &shm_ptr);

	shm_ptr[i1] = RUNNING;
	srand(i1);
	
	int addedNumbers[ARR_SIZE]={ZERO};
	int flag = TRUE;
	while(flag)
	{
		if(shm_ptr[FIRST_PROG] == RUNNING && shm_ptr[SECOND_PROG] == RUNNING && shm_ptr[THIRD_PROG] == RUNNING) 
			flag = FALSE;
	}
	int random, ocur,max_ocur=0,new_numbers=0;
	flag = TRUE;
	while (flag)
	{
		if(shm_ptr[ARR_SIZE -1] != 0)//array is full
		{
			flag = FALSE;
			break;
		}
		sem_wait(mutex); //statr of critical code
		random = rand() % RANGE + 2;
		while(!is_prime(random))//check if the random number is prime
			random = rand() % RANGE + 2;
		ocur = count_ocur(random, shm_ptr);
		if(ocur == 0) new_numbers++;
		add_num(shm_ptr,random);
		addNew(addedNumbers,random);
		sem_post(mutex); //end of critical code
	}
	max_ocur = max_apperance(shm_ptr,addedNumbers);
	
	kill(shm_ptr[0], SIGUSR1);
	
	printf("%d %d\n", new_numbers, max_ocur);
	sem_close(mutex);
	sem_unlink(MUTEX_NAME);
	kill(shm_ptr[0], SIGUSR1);
	shmdt(shm_ptr);
}
//------------------------------------------------------------------------
void initialize(key_t* key, int* shm_id, int** shm_ptr)
{
	if ((*key = ftok(".", '8')) == -1)
	{
		perror("ftok() failed\n");
		exit(EXIT_FAILURE);
	}

	if ((*shm_id = shmget(*key, 0, 0600)) == -1)
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
//------------------------------------------------------------------------
/*
 * this function receives a wanted number and returns if it is a prime number
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
//------------------------------------------------------------------------
/*
 * this function counts the reoccurences of a prime num in the shared memory 
 * array
 */
int count_ocur(int prime_num, int *shm_ptr)
{
	int i;
	int count = 0;
	
	for(i = FIRST; shm_ptr[i] != 0; i++)
	{
		if(shm_ptr[i] == prime_num)
			count++;
	}
	return count;
}
/*
 * add a number to array
 */
void add_num(int arr[],int num)
{
	for(int i = FIRST; i < N; i++)
	{
		if(arr[i] == 0)
		{
			arr[i] = num;
			return;
		}
	}
}
//------------------------------------------------------------------------
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
	shm_id = shmget(key ,0 , 0600) ;

	if (shm_id == -1)
	{
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}
}
//------------------------------------------------------------------------
/*
 * add the random number to the array if it's the first time created
 */
void addNew(int arr[],int num)
{
	int flag = 1;
	int i;
	for(i = 0; arr[i] != 0; i++)
	{
		if(arr[i] == num)
		{
			flag = 0;
			break;
		}
			
	}
	if(flag)
	{
		arr[i] = num;
	}
	
}
//------------------------------------------------------------------------
/*
 * count the max apperance in the array
 */
int max_apperance(int shm_ptr[],int addedNumbers[])
{
	int max_app = 0,count=0;
	for(int i = 0; addedNumbers[i] != 0; i++)
	{
		for(int index = FIRST; index < ARR_SIZE+FIRST; index++)
		{
			if(shm_ptr[index] == addedNumbers[i])
			{
				count++;
			}
		}
		if(max_app < count) max_app = count;
		count = 0;
	}
	return max_app;

}
