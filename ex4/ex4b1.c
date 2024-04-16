/* ex4b1.c : an application server
 * ===========================================================================
 * Author: Rani Abu Raia
 * ID: 316396787
 * Login: abura
 * ===========================================================================
 * this program runs as an application server that comunicates this it's clients
 * through a message queue. before start the server wait tell 
 * he gets the numbers of all clients. after that he gets a random numbers from each
 * client and stores it in array. for each random number the 
 * server gets he return a feedback as integer which represents the apperance
 * times for this number in the array. after the array is full the server sends
 * end message to all clients.
 * when finished the server prints how many distincit numbers he has in the array
 * and what is the max and the min.
 * ===========================================================================
 * input:  no input.
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
#include <sys/ipc.h>
#include <sys/msg.h>
//------------------------------constants------------------------------------------
#define ARR_SIZE 10000// size of the array
#define NUM_OF_CHILDREN 3 // number of children the father creates
#define PRIME_SIZE 1000
#define START 1
#define END -1
#define PATH "."
//-----------------------------prototypes-------------------------------------------
int open_message_queu();
int find_min(int* arr);
int find_max(int* arr);
bool found(int* arr, int id);
//-----------------------------structs-------------------------------------------
struct Data{
	pid_t _id;
	int _num;
};
struct my_msgbuf{
	long mtype;
	struct Data _data;
};
//-------------------------------main-----------------------------------------------
int main(int argc , char* argv[])
{
	struct my_msgbuf my_msg;
	int msqid;
	msqid = open_message_queu();

	int arr[ARR_SIZE]={0}; // the i/o array
	int prime[PRIME_SIZE] = {0}; // the prime numbers array has been added
	
	int ditinct = 0;
	int start[NUM_OF_CHILDREN] = {0};
	int start_index = 0;
	int id[NUM_OF_CHILDREN]={0};
	int id_index = 0;
	int c1[PRIME_SIZE]={0};
	int c2[PRIME_SIZE]={0};
	int c3[PRIME_SIZE]={0};
	//wait tell all client sends 1
	int pid;
	while(start[0] != 1 || start[1] != 1 || start[2] != 1)
	{
		if(msgrcv(msqid, &my_msg,sizeof(struct Data), 1, 0) == -1) 
		{
				perror("msgsnd failed");
				exit(EXIT_FAILURE);
		}
		
		pid = my_msg._data._id;
		my_msg.mtype = pid;
		
		if(!found(id, pid))
		{
			id[id_index] = pid;
			id_index++;
			start[start_index] = my_msg._data._num;
			start_index++;	
		}
	}
	//send start to all clients
	my_msg._data._num = START;
	for(int i=0;i< NUM_OF_CHILDREN;i++)
	{
		my_msg.mtype = id[i] ;
		if (msgsnd(msqid, &my_msg,sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
	}
	int i = 0;
	int max1=0,max2=0,max3 = 0,p ;
	while (i < ARR_SIZE) 
	{
		if(msgrcv(msqid, &my_msg,sizeof(struct Data), 1, 0) == -1) 
		{
				perror("msgsnd failed");
				exit(EXIT_FAILURE);
		}
		my_msg.mtype = my_msg._data._id ;
		p = my_msg._data._num;
		my_msg._data._num = prime[p];
		arr[i] = p;
		if(!prime[p]) ditinct++;
		prime[p]++;
		
		if(my_msg._data._id == id[0]) c1[p]=1;
		
		if(my_msg._data._id == id[1]) c2[p]=1;

		if(my_msg._data._id == id[2]) c3[p]=1;

		if (msgsnd(msqid, &my_msg,sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
		i++;
	}
	// find max apperances for a prime number was sent by the clients
	for(int i = 0; i < PRIME_SIZE; i++)
	{
		if(c1[i] ==1)
		{
			if(max1 < prime[i]) max1 = prime[i];
		}
	}
	for(int j = 0; j < PRIME_SIZE; j++)
	{
		if(c2[j] ==1)
		{
			if(max2 < prime[j]) max2 = prime[j];
		}
	}
	for(int k = 0; k < PRIME_SIZE; k++)
	{
		if(c3[k] ==1)
		{
			if(max3 < prime[k]) max3 = prime[k];
		}
	}

	//send end to all clients
	my_msg._data._num = END;
	for(int i=0;i< NUM_OF_CHILDREN;i++)
	{
		my_msg.mtype = id[i] ;
		if (msgsnd(msqid, &my_msg,sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
	}
	
	//send max apperance for clients
	for(int i=0;i< NUM_OF_CHILDREN;i++)
	{
		my_msg.mtype = id[i] ;
		if(i == 0) my_msg._data._num = max1;

		if(i == 1) my_msg._data._num = max2;

		if(i == 2) my_msg._data._num = max3;

		if (msgsnd(msqid, &my_msg,sizeof(struct Data), 0) == -1)
		{
			perror("msgsnd failed");
			exit(EXIT_FAILURE);
		}
	}
	int max = find_max(arr);
	int min = find_min(arr);
	printf("%d\n",ditinct);
	printf("%d\n",max);
	printf("%d\n",min);
	exit(EXIT_SUCCESS);
	
}
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
int find_max(int* arr)
{
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
// open the queue message 
int open_message_queu()
{
	int msqid;
	key_t key;
	if ((key = ftok(PATH, '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0600 | IPC_CREAT| IPC_EXCL))== -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
	return msqid;
}
// check if a given id stored in the  array
bool found(int* arr, int id)
{
	for (int i = 0 ; i < NUM_OF_CHILDREN; i++)
	{
		if(arr[i] == id)
		{
			return true;
		}
	}
	return false;
}
