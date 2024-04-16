/*
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program works with shared memory, 
 * it works as a palindrome checker server.
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
// --------------CONST--------------------------
const int ARR_SIZE = 22*sizeof(int);
const int RANGE = 199999;
const int PID = 0;
const int CLIENT_PID = 1;
const int N = 1000;
const int TRUE = 1;
const int END = -1;
const int OPEN = 1;
const int CLOSE = 0;
const int LOCK = 4;
const int EMPTY = -1;
const int FALSE = 0;
const int FIRST_NUMBER = 2;
const int GCD = 3;
const int PID_PF = 21;
const int RESULT = 21;

int shm_id;
key_t key ;
// --------------prototype----------------------
void signal_hadler_SIGINT(int i);
void signal_hadler_SIGUSR2(int i);
void update_shm_ptr(int num, int *arr);
void create_keys();
void check_ptr(int *shm_ptr_pf);
int is_palindrome(int n, int *arr);
int size(int* arr);
//--------------------------------------------------------------------------

int main()
{
	// make external and internal key
	create_keys();

	int *shm_ptr ;
	shm_ptr = (int *) shmat (shm_id , NULL, 0);
	check_ptr(shm_ptr);

	shm_ptr[PID] = getpid();

	signal(SIGINT,signal_hadler_SIGINT);
	signal(SIGUSR2,signal_hadler_SIGUSR2);
	int n;
	while(TRUE)
	{
		pause();
		n = size(shm_ptr);
		shm_ptr[RESULT] = is_palindrome(n,&shm_ptr[FIRST_NUMBER]);
		kill(shm_ptr[CLIENT_PID], SIGUSR2);
	}
	return EXIT_SUCCESS;
}

//--------------------------------------------------------------------------

void signal_hadler_SIGUSR2(int i)
{
	//does nothing just wakes up pause;
}

//--------------------------------------------------------------------------
/*
 * update_shm_ptr finds the prime factors of num and stores them 
 * in the shared memory
 */
void update_shm_ptr(int num, int *arr)
{
	int i, j, isPrime;
	int start = 2;
	/* Find all Prime factors */
	for(i=2; i<=num; i++)
	{
		/* Check 'i' for factor of num */
		if(num%i==0)
		{
			/* Check 'i' for Prime */
			isPrime = 1;
			for(j=2; j<=i/2; j++)
			{
				if(i%j==0)
				{
					isPrime = 0;
					break;
				}
			}
			/* If 'i' is Prime number and factor of num */
			if(isPrime==1)
			{
				arr[start] = i;
				start++;
			}
		}
	}
	for(i=start; i < 21; i++)
		arr[i] = 0;
	return ;
}
//--------------------------------------------------------------------------

void signal_hadler_SIGINT(int i)
{
	if (shmctl(shm_id,IPC_RMID, NULL) == -1)
	{
		perror( "shmctl failed" ) ;
		exit( EXIT_FAILURE ) ;
	}
	exit(EXIT_SUCCESS);
}
//--------------------------------------------------------------------------
/*
 * create interal and external keys
 */
void create_keys()
{
	if ((key = ftok(".", 'q')) == -1)
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
/*
 * this function recieves a a number and an array, the numbers recieved are 
 * stored in this array in order to check for plindrome features
 */

int is_palindrome(int n, int *arr)
{
	{
	    // Initialise flag to zero.
	    int flag = 0;
	 
	    // Loop till array size n/2.
	    for (int i = 0; i <= n / 2 && n != 0; i++) {
	 
	        // Check if first and last element are different
	        // Then set flag to 1.
	        if (arr[i] != arr[n - i - 1]) {
	            flag = 1;
	            break;
	        }
	    }
	 
	    // If flag is set then print Not Palindrome
	    // else print Palindrome.
	    if (flag == 1)
	        return 0;
	    else
	        return 1;
	}
}
/*
 * return length of the series of numbers entered by the user
 */
int size(int* arr)
{
	int length = 0;
	int i = 2;
	while(arr[i] != 0) 
	{
		length++;
		i++;
	}
	return length;
}

