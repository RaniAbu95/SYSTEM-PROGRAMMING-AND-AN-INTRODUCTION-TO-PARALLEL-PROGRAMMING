/*
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *			 
 * DESCRIPTION: this program works with shared memory, 
 * it works as a PRIME server.
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
const int ARR_SIZE = 5*sizeof(int);
const int RANGE = 199999;
const int ZERO = 0;
const int N = 1000;
const int TRUE = 1;
const int END = -1;
const int OPEN = 1;
const int CLOSE = 0;
const int LOCK = 4;
const int EMPTY = -1;
const int FALSE = 0;
const int FIRST_NUMBER = 1;
const int SECOND_NUMBER = 2;
const int GCD = 3;
const int PID = 0;
const int CLIENT_PID = 1;
const int PRIME_NUM = 2;
const int RESULT = 3;

int shm_id;
key_t key ;
// --------------prototype----------------------
void signal_hadler_SIGUSR1(int i);
void signal_hadler_SIGINT(int i);
void create_keys();
void check_ptr(int *shm_ptr_pf);
int is_prime(int wanted);
//--------------------------------------------------------------------------

int main()
{
	// make external key and internal key
	create_keys();
	
	// make shared memory
	int *shm_ptr ;
	shm_ptr = (int *) shmat (shm_id , NULL, 0);
	check_ptr(shm_ptr);
	// update pid
	shm_ptr[PID] = getpid();

	signal(SIGINT,signal_hadler_SIGINT);
	signal(SIGUSR1, signal_hadler_SIGUSR1);

	while(TRUE)
	{
		pause();
		int num = shm_ptr[PRIME_NUM];
		shm_ptr[RESULT] = is_prime(num);
		kill(shm_ptr[CLIENT_PID], SIGUSR1);
	}
	return EXIT_SUCCESS;
}
//--------------------------------------------------------------------------

void signal_hadler_SIGUSR1(int i)
{
	//does nothing just wakes up pause;
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
	if ((key = ftok(".", 'p')) == -1)
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
 * this function receives a wanted number and returns if it is a prime number
 *  or not
 */
int is_prime(int wanted)
{
	if(wanted == 1) return 0;
    int prime = 1 ;
    int i;
    for (i = 2; i < wanted/2 + 1; i++)
        if (wanted % i == 0)
        {
            prime = 0 ;
            break;
        }
    return prime;
}
