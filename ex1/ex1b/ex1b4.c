/*
 * ex1b4.c
 *
 *  Created on: Nov 6, 2021
 *      Author: Rani Abu Raia
 *      user: abura
 *      id: 316396787
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * This is the main program. it gets as an input file name and print on it
 * all the users input. the user input is one of the following commands:
 * history,echp,ps,exit (with or without arguments), based pn the user input
 * it execute the relevant program, example if "echo aloha" is the input of the user
 * the execute the ex1b2.c wich was compiled to echo.o file. and prints "aloha" to stdout.
 */
int main(int argc, char* argv[])
{
	FILE* fp;
	fp = fopen(argv[1],"w");
	pid_t x;
	char* first;
	char* second;

	while(1)
	{
		char *buffer;
		int numOfargs;
		size_t bufSize = 100;
		buffer = (char*) malloc(bufSize * sizeof(char));
		getline(&buffer,&bufSize,stdin);

		first = strtok(buffer," ,.-\n");

		if (first != NULL)
		{
			if(!strcmp(first,"echo"))
			{
				numOfargs = 2;
				second = strtok (NULL, "\n");
			}
			else
			{
				second = strtok (NULL, " ,.\n");
				if(second != NULL)
				{
					numOfargs = 2;
				}
				else
				{
					numOfargs = 1;
				}
			}

		}
		fprintf(fp,buffer);
		fprintf(fp,"\n");

		if(!strcmp(buffer,"exit"))
		{
			unlink("history");
			exit(1);
		}

		if( ! (!strcmp(first,"history")  || !strcmp(first,"echo") || !strcmp(first,"ps")))
		{
			printf("bad command\n");
		}

		x=fork();
		if(x==0)
		{
			if(! strcmp(first,"history"))
			{
				if(numOfargs==1)
				{
					char * args[]  = {"./history",NULL};
					execvp(args[0],args);
				}
				else
				{
					char * args[] = {"./history",second,NULL};
					execvp(args[0],args);
				}

			}
			else if(!strcmp(first,"echo"))
			{
				if(numOfargs==1)
				{
					char * args [] = {"./echo",NULL};
					execvp(args[0],args);
				}
				else
				{

					char* args[] = {"./echo",second,NULL};
					execvp(args[0],args);
				}

			}
			else if(!strcmp(first,"ps"))
			{
				if(numOfargs==1)
				{
					char *  args[] = {"./ps",NULL};
					execvp(args[0],args);
				}
				else
				{
					char* args [] = {"./ps",second,NULL};
					execvp(args[0],args);
				}

			}
		}
		waitpid(x,NULL,0);


	}
	exit(1);
}
