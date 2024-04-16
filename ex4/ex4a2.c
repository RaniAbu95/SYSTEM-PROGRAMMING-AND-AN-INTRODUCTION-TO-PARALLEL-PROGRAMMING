/* ex4a2.c : the client
 * ===========================================================================
 * Author: Rani Abu Raia
 * ID: 316396787
 * Login: abura
 * ===========================================================================
 * this program runs as a client. first it sends it's file number for the  server,
 * then itgenerates a random number  and check if it  is a prime number.
 * for each number the client send it gets a feedback from a server that indicates
 * how much this numbers appears in the storing array. when the storing array 
 * is ready the client gets ending feedback and prints how many numbers it 
 * send was new and prints the max times sent of a number.
 * ===========================================================================
 * input: a command and a string/number from the user
 * output: 0 or 1 or -1 according to the above 
 */
//--------------------include section--------------------------
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
//------------------------------constants------------------------------
#define RANGE 1000
#define F0 "fifo0"
#define F1 "fifo1"
#define F2 "fifo2"
#define F3 "fifo3"
//-----------------------------prototypes------------------------------
void open_files(FILE **fifo0,FILE **myfile,int file_num);
bool is_prime(int wanted);
int find_max_sent(int* arr);
//-------------------------------main----------------------------------
int main(int argc , char* argv[])
{
	//printf("opened F successfully");
    FILE * my_fifo, * fifo0; // streamers for the pipe
    int file_num = atoi(argv[2]);
    open_files(&fifo0,&my_fifo,file_num);
    srand(file_num) ;
    int feedback;
    fprintf(fifo0, "%d ", file_num);//send server the file number
    fflush(fifo0);
    fscanf(my_fifo, "%d", &feedback);//get feedbakc from server to start
    fflush(my_fifo);
    int prime[RANGE] = {0};
    int new_numbers = 0;
	while (1)
	{
		// send a random number to fifo0
		int random = rand() % (RANGE - 1) + 2;
		if(is_prime(random))
		{
			fprintf(fifo0, "%d %d ",random,file_num);
			fflush(fifo0);
			prime[random]++;// get feedback from running fifo
			fscanf(my_fifo, "%d", &feedback);
			
			if(!feedback) new_numbers++; // first appeared in array
			if(feedback < 0) break; //end msg
		}
	}
	int max = 0;
	max = find_max_sent(prime);
    printf("%d\n", new_numbers);
    printf("%d\n",max);
    
    return 0;
}
//.............................................................................
/*
 * open fifo files for read and write
 */
void open_files(FILE ** fifo0,FILE ** myfile,int file_num)
{
	//printf("opened  successfully");
    *fifo0 = fopen(F0,"w");
    *myfile = fopen(F1,"r");
    
    switch (file_num)
    {
		case 1:
			*myfile = fopen(F1,"r");
			break;
		case 2:
			*myfile = fopen(F2,"r");
			break;
		case 3:
			*myfile = fopen(F3,"r");
			break;
    }

    if (!myfile || !fifo0)
    {
        perror("cant open a file");
        exit(EXIT_FAILURE);
    }
    
}
// this function receives a wanted number and returns if it is a prime number
// or not
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
//max numbers sent for this client
int find_max_sent(int* arr)
{
	int max = 0;
	for(int i =0;i < RANGE; i ++)
	{
		if(arr[i] > max)
		{
			max = arr[i];
		}
	}
	return max;
}