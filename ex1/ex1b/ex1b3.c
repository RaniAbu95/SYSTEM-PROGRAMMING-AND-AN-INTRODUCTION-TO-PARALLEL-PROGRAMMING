/*
 * ex1b3.c
 *
 *  Created on: Nov 6, 2021
 *      Author: Rani Abu Raia
 *      user: abura
 *      id: 316396787
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * This program prints the pid of the parent process of the current process if executed without arguments
 * if "-a" was passed it prints the the current process pid and its parent pid
 */
int main(int argc, char* argv[])
{
	if(argv[1] == NULL)
	{
		printf("Parent process id is: %d\n",getppid());
	}
	else if(! strcmp(argv[1],"-a"))
	{
		printf("Parent process id is: %d\n",getppid());
		printf("Child process id is: %d\n",getpid());
	}
	else
	{
		printf("Error: Invalid Input\n");
	}
	exit(1);
}



