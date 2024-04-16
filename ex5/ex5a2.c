/*
 * Written by: Rashid abounis, id = 315982421, login = rashidab
 *			   Mohamad ektelat, id = 208223826, login = mohamadek
 *
 * DESCRIPTION: this program works with shared memory,
 * it works as clients which fill the shared array with numbers
 * when it's full it prints how many numbers was added as new 
 * and from all numbers sent what is the max apperance of a given number.
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
#include <sys/shm.h>
#include <stdbool.h>
// --------------CONST--------------------------
const int ARR_SIZE = 1000;
const int RANGE = 999;
const int ZERO = 0;
const int N = 1000;
const int TRUE = 1;
const int END = -1;
const int OPEN = 1;
const int CLOSE = 0;
const int LOCK = 4;
const int EMPTY = -1;
const int FALSE = 0;
key_t key;
int shm_id;
// --------------prototype----------------------
void create_keys();
void check_ptr(int *shm_ptr_pf);
int count_ocur(int prime_num, int *shm_ptr);
void add_num(int arr[],int num);
bool is_prime(int wanted);
//--------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	int process_id = atoi(argv[1]);
	srand(process_id);
	int ocur, max_ocur = 0, unique = 0,random;
	// get external key
	create_keys();

	int *shm_ptr = (int *) shmat(shm_id, NULL, 0);
	check_ptr(shm_ptr);

	shm_ptr[process_id] = 1;
	
	int flag = TRUE;
	while(flag)
	{
		if(shm_ptr[1] == 1 && shm_ptr[2] == 1 && shm_ptr[3] == 1)
			flag = FALSE;
	}
	int new_numbers = 0;
	flag = TRUE;
	while(flag)
	{
		int lock = shm_ptr[LOCK];
		if(lock == CLOSE) continue;
		if(shm_ptr[LOCK] == OPEN)
		{
			shm_ptr[LOCK] = CLOSE;//activate lock
			if(shm_ptr[999] != 0)//array is full
			{
				flag = FALSE;
				shm_ptr[LOCK] = OPEN;//deactivate the lock
				break;
			}
			else
			{
				random = rand() % RANGE + 2;
				while(!is_prime(random))//check if the random number is prime
					random = rand() % RANGE + 2;
				ocur = count_ocur(random, shm_ptr);
				if(max_ocur < ocur) max_ocur = ocur;//check for max 
				if(ocur == 0) unique++;
				add_num(shm_ptr,random);
				new_numbers++;
				shm_ptr[LOCK] = OPEN;//deactivate the lock
			}
			
		}
	}
	shm_ptr[4] = OPEN;
	
	printf("%d %d\n", unique, new_numbers);

	shm_ptr[process_id] = 0;
	kill(shm_ptr[0], SIGUSR1);
	return EXIT_SUCCESS;
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
	shm_id = shmget(key ,0 , 0600) ;

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

/*
 * this function counts the reoccurences of a prime num in the shared memory 
 * array
 */
int count_ocur(int prime_num, int *shm_ptr)
{
	int i;
	int count = 0;
	
	for(i = 6; shm_ptr[i] != 0; i++)
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
	for(int i = 5; i < N; i++)
	{
		if(arr[i] == 0)
		{
			arr[i] = num;
			return;
		}
	}
}
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
