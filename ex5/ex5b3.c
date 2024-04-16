/*
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program works with shared memory,
 * this program works as clients, sending numbers to other
 * servers to work with. 
 * this program shares data with a PRIME and PALINDROME servers.
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
const int ARR_SIZE1 = 5*sizeof(int);
const int ARR_SIZE2 = 22*sizeof(int);
const int RANGE = 199999;
const int SERVER_PID = 0;
const int PID = 1;
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
const int RESULT_PRIME = 3;
const int RESULT_PALIN = 21;
const int MAX_LENGTH = 20;
int shm_id_prime, shm_id_palindrome;
key_t key_prime, key_palindrome;
// --------------prototype----------------------
void signal_handler_SIGUSR1(int i);
void signal_handler_SIGUSR2(int i);
void signal_hadler_SIGINT(int i);
void create_keys();
void check_ptr(int *shm_ptr_gcd, int *shm_ptr_palindrome);
//--------------------------------------------------------------------------
int main()
{
	// create shared memory
	int *shm_ptr_prime;
	int *shm_ptr_palindrome; 
	// create internal and external keys
	create_keys();

	shm_ptr_prime = (int *) shmat (shm_id_prime , NULL, 0);
	shm_ptr_palindrome = (int *) shmat (shm_id_palindrome , NULL, 0);
	// check shared memory 
	check_ptr(shm_ptr_prime, shm_ptr_palindrome);
	int pid = getpid();
	shm_ptr_prime[PID] = pid; //insert the pid of the process in prime array
	shm_ptr_palindrome[PID] = pid;//insert the pid of the process in palindrome array
	// apply signals 
	signal(SIGUSR1,signal_handler_SIGUSR1);	
	signal(SIGUSR2,signal_handler_SIGUSR2);
	signal(SIGINT,signal_hadler_SIGINT);
	
	while(TRUE)
	{
		char c;
		scanf("%c", &c);
		if(c == 'p')
		{
			int x;
			scanf("%d", &x);
			shm_ptr_prime[FIRST_NUMBER] = x;
			kill(shm_ptr_prime[SERVER_PID], SIGUSR1);
			pause();
			printf("%d\n", shm_ptr_prime[RESULT_PRIME]);
		}
		else if(c == 'q')//check for plindrome
		{
			int num;
			int i = FIRST_NUMBER;
			while(i < MAX_LENGTH)
			{
				scanf("%d", &num);
				if(num == 0) break;
				shm_ptr_palindrome[i] = num;
				i++;
			}
			kill(shm_ptr_palindrome[SERVER_PID], SIGUSR2);
			pause();
			printf("%d\n", shm_ptr_palindrome[RESULT_PALIN]);
		}
	}
}

//--------------------------------------------------------------------------

void signal_handler_SIGUSR1(int i)
{
	//wakes up pause;
}
//--------------------------------------------------------------------------

void signal_handler_SIGUSR2(int i)
{
	//wakes up pause;
}
//--------------------------------------------------------------------------
/*
 * create interal and external keys
 */
void create_keys()
{
	if ((key_prime = ftok(".", 'p')) == -1 ||
	   (key_palindrome = ftok(".", 'q')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	// make internal key
	shm_id_prime = shmget(key_prime ,0 , 0600);
	shm_id_palindrome = shmget(key_palindrome ,0 , 0600);

	if ((shm_id_prime == -1) || (shm_id_palindrome == -1))
	{
		perror("shmget failed");
		exit(EXIT_FAILURE);
	}
}
//--------------------------------------------------------------------------
/*
 * check_ptr checks if allocated pointers are okay to use
 */
void check_ptr(int *shm_ptr_prime, int *shm_ptr_palindrome)
{
	if (shm_ptr_prime == (int *) -1 || shm_ptr_palindrome == (int *) -1)
	{
		perror( "shmat failed" ) ;
		exit( EXIT_FAILURE ) ;
	}
}
/*
 * signal handler for SIGINT signal
 */
void signal_hadler_SIGINT(int i)
{
	exit(EXIT_SUCCESS);
}
