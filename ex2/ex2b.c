/*
 * ex2b.c
 *
 *  Created on: Nov 15, 2021
 *      Author: abura
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_SIGNALS_TO_SEND 7
#define MAX_SIGNALS_TO_RECEIVE 5
#define MAX_TIME_TO_SLEEP 3
#define SEED 17

pid_t c;
int user1_signals = 0;
int user2_signals = 0;

void sig_user_handler(int signo)
{
	signal(SIGUSR1, sig_user_handler);
	signal(SIGUSR2, sig_user_handler);
	if(signo == SIGUSR1)
	{
		user1_signals++;
		printf("process %d got signal SIGUSR1\n", (int)getpid());
	}
	else if(signo == SIGUSR2)
	{
		user2_signals++;
		printf("process %d got signal SIGUSR2\n", (int)getpid());
	}

	if(user1_signals == MAX_SIGNALS_TO_RECEIVE || user2_signals == MAX_SIGNALS_TO_RECEIVE)
	{
		printf("process %d surrender\n", ((int)getpid()));
		if(c > 0)
		{
			kill(c, SIGTERM);
		}
		else
		{
			kill(getppid(), SIGTERM);
		}
		exit(0);
	}
}

//-----------------------------------------------------------------------------

void term(int signo)
{
	signal(SIGTERM, term);
	printf("process %d win\n", getpid());
	exit(0);
}


void run_process(pid_t other)
{
	int n;
	int counter1 = 0, counter2 = 0;
	while(1)
	{
		sleep(rand() % MAX_TIME_TO_SLEEP);
		n = rand() % MAX_TIME_TO_SLEEP;
		if(n == 0)
		{		
			exit(0);
		}			
		if(n == 1) 
		{
			counter1++;
			kill(other, SIGUSR1);
			if(counter1 == MAX_SIGNALS_TO_SEND) 
			{
				printf("you probably ended\n"); // this print probably wont be executed
				exit(0);
			}
		}
			
		else if(n == 2) 
		{
			counter2++;
			kill(other, SIGUSR2);
			if(counter2 == MAX_SIGNALS_TO_SEND) 
			{
				printf("you probably ended\n"); // this print probably wont be executed
				exit(0);
			}
		}	
	}
}


int main(int argc, char* argv[])
{
    signal(SIGUSR1, sig_user_handler);
	signal(SIGUSR2, sig_user_handler);
	signal(SIGTERM, term);
	srand(SEED);
	pid_t other;
	c = fork();
	if(c < 0) {
		printf("error in fork") ;
		exit(EXIT_FAILURE) ;
	}
	if(c == 0)
	{
		other = getppid();
		run_process(other);
	}
	else
	{
		other = c;
		run_process(other);
	}
	return 0;
}

