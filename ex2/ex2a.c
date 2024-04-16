/*
 * ex2a.c
 *
 *  Created on: Nov 14, 2021
 *      Author: abura
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define SEED 17
#define T2 2
#define EPSILON 0.000001

double seconds = 0;

void catch_alarm(int sig_num)
{
	exit(EXIT_SUCCESS) ;
}

int main()
{
	struct timeval start,end;
	signal(SIGALRM,catch_alarm);
	int pipe_descs[2] ;
	if (pipe(pipe_descs) == -1) 
	{
		printf("cannot open pipe") ;
		exit(EXIT_FAILURE) ;
	}
	
	pid_t c_pid = fork() ;
	if (c_pid < 0) {
		printf("error in fork") ;
		exit(EXIT_FAILURE) ;
	}

	if (c_pid == 0) // son process
	{
		srand(SEED);
		int num, cnt =0, flag = 0;
		alarm(T2);
		gettimeofday(&start,NULL);
		
		while(!flag)
		{
			if(cnt == 10000000000)
			{
				flag = 1;
			}
			num = rand();
			cnt++;
			
		}
		alarm(0);
		gettimeofday(&end,NULL);
		seconds =  (double) (end.tv_usec - start.tv_usec)/1000000 + (double)(end.tv_sec - start.tv_sec);
				
		printf("seconds + micro: %f\n",seconds);
		close(pipe_descs[0]) ; // son writes to pipe
		write(pipe_descs[1], &seconds, sizeof(double)) ;
		close(pipe_descs[1]) ;
		exit(EXIT_SUCCESS) ;
	}
		
	
	else
	{		
		int nbytes ;
		double n;
		close(pipe_descs[1]) ; // father reads
		nbytes = read(pipe_descs[0], &n, sizeof(double)) ;
		if(nbytes != -1)
		{
			close(pipe_descs[0]) ;
		}
		printf("N: %lf\n",n);
		if((n - 0) < EPSILON)
		{
			printf("killing child process\n");
			int ret;
			ret = kill(c_pid,SIGKILL);
			if(ret == -1)
			{
				printf("Error in kill signal\n");
				exit(EXIT_FAILURE);
			}
			printf("0\n");
			exit(EXIT_SUCCESS) ;
		}
		if(n < 1)
		{
			printf("1\n");
			exit(EXIT_SUCCESS) ;
		}
		else if(n < 2)
		{
			printf("2\n");
			exit(EXIT_SUCCESS) ;
		}

		
	}
				

}
