/*
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program creates 4 threads (MAIN , PRIME,
 * PALINDROME, CLIENTS ).
 * the main thread creates the other threads then saves their id's in
 * a global array and finishes his work, the clients thread gets from
 * user what operation he asks to do ( PRIME or PALINDROME ) then gets
 * the needed numbers from user and sends a signal to the thread who needs
 * them, the thread makes the demanded process and updates the solution to
 * the program, then sends back a signal to the client to inform him the
 * solution is ready and the program is ready for another client.
 *
*/
// --------------INCLUDE------------------------
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#define IS_PALINDROME "yes"
#define NOT_PALINDROME  "no"

// --------------CONST--------------------------
pthread_t id[] = {0,0,0};
const int MAX_STR_LEN  = 100;
int prime[] = {0,0};
char* palindrome[2];
const int TRUE = 1;
const int FIRST = 0;
const int SECOND = 1;
const int THIRD = 2;


// --------------prototype----------------------
void signal_handler_user1(int i);
void * do_prime(void * n);
void * do_palindrome(void * n);
void * do_clients(void * n);
void check_status(int status1, int status2, int status3);
void signal_handler_SIGINT(int i);
void update_id(pthread_t , pthread_t , pthread_t );
bool is_prime(int wanted);
int is_palindrome( char *str);

//--------------------------------------------------------------------------

int main()
{
	pthread_t thread_data1, thread_data2, thread_data3 ;
	signal(SIGUSR1,signal_handler_user1);
	signal(SIGINT,signal_handler_SIGINT);
	// creating threads
	//int status1 = pthread_create(&thread_data1, NULL, do_gcd, NULL);
	int status1 = pthread_create(&thread_data1, NULL, do_prime, NULL);
	int status2 = pthread_create(&thread_data2, NULL, do_palindrome, NULL);
	int status3 = pthread_create(&thread_data3, NULL, do_clients, NULL);
	check_status(status1, status2, status3);
	update_id(thread_data1, thread_data2, thread_data3);
	pthread_join(thread_data1, NULL);
	pthread_join(thread_data2, NULL);
	pthread_join(thread_data3, NULL);
	return EXIT_SUCCESS;
}
//--------------------------------------------------------------------------
/*
 * this function updates and saves the id's of all threads
 */
void update_id(pthread_t thread_data1, pthread_t thread_data2, pthread_t thread_data3)
{
	id[FIRST] = thread_data1;
	id[SECOND] = thread_data2;
	id[THIRD] = thread_data3;
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
 * do pthread function raffles off numbers and checks if found in bingo array,
 * it continuous to check numbers until all bingo array numbers are found.
 */
void * do_prime(void * n)
{
	while(TRUE)
	{
		pause();
		int check = prime[FIRST];
		int feedback = is_prime(check);
		prime[SECOND] = feedback;
		pthread_kill(id[THIRD], SIGUSR1);
	}
	return NULL;
}
//--------------------------------------------------------------------------

void signal_handler_user1(int i)
{
	signal(SIGUSR1,signal_handler_user1);
	// wakes up pause
}

//--------------------------------------------------------------------------
/*
 * do pthread function raffles off numbers and checks if found in bingo array,
 * it continuous to check numbers until all bingo array numbers are found.
 */
void * do_palindrome(void * n)
{

	while(TRUE)
	{
		pause();
		char* str = palindrome[FIRST];
		int res = is_palindrome(str);
		if(res) palindrome[SECOND] = IS_PALINDROME;
		else palindrome[SECOND] = NOT_PALINDROME;
		pthread_kill(id[THIRD], SIGUSR1);
	}
	return NULL;
}
//--------------------------------------------------------------------------
/*
 * do pthread function raffles off numbers and checks if found in bingo array,
 * it continuous to check numbers until all bingo array numbers are found.
 */
void * do_clients(void * n)
{
	char str[MAX_STR_LEN];
	while(TRUE)
	{
		char c;
		scanf("%c", &c);
		if(c == 'p')
		{
			int num;
			scanf("%d",&num);
			prime[FIRST] = num;
			pthread_kill(id[FIRST], SIGUSR1);
			pause();
			printf("%d\n", prime[SECOND]);
		}
		if(c == 'q')
		{
			scanf("%s", str);
			palindrome[FIRST] = str;	
			pthread_kill(id[SECOND], SIGUSR1);
			pause();
			int ans;
			char* yes  = IS_PALINDROME;
			if(palindrome[SECOND] == yes) ans = 1;
			else ans = 0;
			printf("%d\n",ans);
			
		}
	}
	return NULL;
}
//--------------------------------------------------------------------------

void signal_handler_SIGINT(int i)
{
	signal(SIGINT,signal_handler_SIGINT);
	exit(EXIT_SUCCESS);
}

/*
 *  this function receives a wanted number and returns if it is a prime number
 *  or not
 */
bool is_prime(int wanted)
{
	if(wanted == 1) return false;
		
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
//this function recieves a number and an array that has a series of integers
//checks the plindrome of this series, returns 1 if its indeed a palindrome 
//and 0 if not
int is_palindrome( char *str)
{
	int i, length;
	int flag = 1;
	
	length = strlen(str);
	
	for(i=0;i < length ;i++)
	{
		if(str[i] != str[length-i-1])
		{
			flag = 0;
			break;
		}
	}
	return flag;	
}
