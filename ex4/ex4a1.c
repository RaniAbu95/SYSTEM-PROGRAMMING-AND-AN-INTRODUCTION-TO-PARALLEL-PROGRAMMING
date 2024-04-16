/* ex4a1.c : an application server
 * ===========================================================================
 * Author: Rani Abu Raia
 * ID: 316396787
 * Login: abura
 * ===========================================================================
 * this program runs as an application server , before start the server wait tell 
 * he gets the numbers of all clients. after that he gets a random numbers from each
 * client and stores it in array. for each random number the 
 * server gets he return a feedback as integer which represents the apperance
 * times for this number in the array. after the array is full the server sends
 * end message to all clients.
 * when finished the server prints how many distincit numbers he has in the array
 * and what is the max and the min numbers.
 * ===========================================================================
 * input:  fifo0 fifo1 fifo2 fifo3
 * output: prints how many distincit numbers he has in the array
 * 		   and what is the max and the min numbers.
 */
//--------------------include section----------------------------------------------
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
//------------------------------constants------------------------------------------
#define ARR_SIZE 10000// size of the array
#define NUM_OF_CHILDREN 3 // number of children the father creates
#define PRIME_SIZE 1000
#define START 1
#define END -1
#define F0 "fifo0"
#define F1 "fifo1"
#define F2 "fifo2"
#define F3 "fifo3"
FILE * fifo0, * fifo1 ,* fifo2,* fifo3;
//-----------------------------prototypes-------------------------------------------
void create_fifoes() ;
void open_fifoes();
int find_min(int* arr);
int find_max(int* arr);
void send_feedback(int file_num, FILE ** f1,FILE ** f2,FILE ** f3,int feedback);
void send_all(FILE** f1,FILE** f2,FILE** f3,int msg);
//-------------------------------main-----------------------------------------------
int main(int argc , char* argv[])
{
	create_fifoes();// create fifo files
	open_fifoes(); // open fifo files

    int arr[ARR_SIZE]; // the i/o array
    int prime[PRIME_SIZE] = {0}; // the prime numbers array has been added
    int file_num, k=0;
    while(k != 3)
    {
		fscanf(fifo0,"%d",&file_num) ;
		fflush(fifo0);
		
		if(file_num == 1) k++;
		
		if(file_num == 2) k++;

		if(file_num == 3) k++;
    }

    send_all(&fifo1,&fifo2,&fifo3,START);
    
    int i = 0;
    int ditinct = 0;
    while (i < ARR_SIZE)
    {
    	int number,feedback,file_num;
        
    	fscanf(fifo0,"%d %d",&number,&file_num) ; // read a command from the client
    	fflush(fifo0);
    	
        arr[i] = number;
        feedback =  prime[number];
        send_feedback(file_num,&fifo1,&fifo2,&fifo3,feedback);// sends apropriate feedback to client
        if(feedback == 0) ditinct++;        
		prime[number]++;
		i++ ;
		
    }
    send_all(&fifo1,&fifo2,&fifo3,END);
    int min = find_min(arr);
    int max = find_max(arr);
    printf("%d\n",ditinct);
    printf("%d\n",max);
    printf("%d\n",min);
   
	return 0;
}
//..................................................................................
// this function creates the named pipes
void create_fifoes()
{
    if((mkfifo("fifo0",S_IFIFO|0644)) == -1
        && errno != EEXIST)
    {
        perror("cannot create fifo1 file") ;
        exit(EXIT_FAILURE) ;
    }
    if((mkfifo("fifo1",S_IFIFO|0644)) == -1
        && errno != EEXIST)
    {
        perror("cannot create fifo1 file") ;
        exit(EXIT_FAILURE) ;
    }
    
    if ((mkfifo("fifo2",S_IFIFO|0644)) == -1
        && errno != EEXIST)
    {
        perror("cannot create fifo1 file") ;
        exit(EXIT_FAILURE) ;
    }
    if ((mkfifo("fifo3",S_IFIFO|0644)) == -1
        && errno != EEXIST)
    {
        perror("cannot create fifo1 file") ;
        exit(EXIT_FAILURE) ;
    }
    
}
// this function opens the fifo pipes
void open_fifoes()
{
    if (!(fifo0 = fopen(F0,"r")))
    {
        perror("cannot open file for r") ;
        exit(EXIT_FAILURE) ;
    }
    if (!(fifo1 = fopen(F1,"w")))
	{
		perror("cannot open file for w") ;
		exit(EXIT_FAILURE) ;
	}
    
    if (!(fifo2 = fopen(F2,"w")))
	{
		perror("cannot open file for w") ;
		exit(EXIT_FAILURE) ;
	}
    if (!(fifo3 = fopen(F3,"w")))
	{
		perror("cannot open file for w") ;
		exit(EXIT_FAILURE) ;
	}
	
    
}
//.............................................................................................
//find max in arr
int find_min(int* arr){
	int min = PRIME_SIZE;
	for(int i =0; i < ARR_SIZE; i++)
	{
		  if(arr[i] < min)
		  {
			  min = arr[i];
		  }
	}
	return min;
}
//find min in arr
int find_max(int* arr){
	int max = 0;
	for(int i =0; i < ARR_SIZE; i++)
	{
		  if(arr[i] > max)
		  {
			  max = arr[i];
		  }
	}
	return max;
}
//send feedback to client
void send_feedback(int file_num, FILE ** f1,FILE ** f2,FILE ** f3,int feedback)
{
    switch (file_num)
    {
		case 1:
			fprintf(*f1, "%d ", feedback);
			fflush(*f1);
			break;
		case 2:
			fprintf(*f2, "%d ", feedback);
			fflush(*f2);
			break;
		case 3:
			fprintf(*f3, "%d ", feedback);
			fflush(*f3);
			break;
    }
}
//send message to all client message can be start or end.
void send_all(FILE** f1,FILE** f2,FILE** f3,int msg)
{
    fprintf(*f1,"%d ",msg); // sending mesage to fifo1 to start
    fflush(*f1);
    fprintf(*f2,"%d ",msg); // sending mesage to fifo2 to start
    fflush(*f2);
    fprintf(*f3,"%d ",msg); // sending mesage to fifo3 to start
    fflush(*f3);
}