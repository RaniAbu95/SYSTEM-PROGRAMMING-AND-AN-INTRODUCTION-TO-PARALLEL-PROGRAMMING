/* ex4b2.c : the client
 * ===========================================================================
 * Author: Rani Abu Raia
 * ID: 316396787
 * Login: abura
 * ===========================================================================
 * this program runs as a client which comunicate with the server through a
 * message queue. first it sends it's file number for the  server,
 * then itgenerates a random number  and check if it  is a prime number.
 * for each number the client send it gets a feedback from a server that indicates
 * how much this numbers appears in the storing array. when the storing array 
 * is ready the client gets ending feedback and prints how many numbers it 
 * send was new and prints the max times sent of a number.
 * ===========================================================================
 * input:  number (1,2 or 3) of the generator.
 * output: prints how many new numbers sent and what is the max apperance of a
 * 		   number.
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
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//------------------------------constants------------------------------
#define RANGE 1000
#define PATH "."
#define START 1
#define END -1
//-----------------------------prototypes------------------------------
int get_msg();
void send_msg(int data);
bool is_prime(int wanted);
int find_max_sent(int* arr);
//-----------------------------structs---------------------------------
struct Data{
	pid_t _id;
	int _num;
};
struct my_msgbuf{
	long mtype;
	struct Data _data;
};
//-------------------------------main----------------------------------
int main(int argc , char* argv[])
{
	struct my_msgbuf my_msg;
	int msqid;
	key_t key;
	int prime[RANGE] = {0};
	srand(atoi(argv[1]));
	if ((key = ftok(PATH, '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0)) == -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
	my_msg.mtype = START ;
	my_msg._data._id = getpid() ;
	my_msg._data._num = 1;
	if (msgsnd(msqid, &my_msg,sizeof(struct Data), 0) == -1) 
	{
		perror("msgsnd failed");
		exit(EXIT_FAILURE);
	}
	if (msgrcv(msqid, &my_msg,sizeof(struct Data), (int) getpid(), 0) == -1) 
	{
		perror("msgrcv failed");
		exit(EXIT_FAILURE);
	}
	int new_numbers = 0;
	while(1){
		my_msg.mtype = 1 ;
		my_msg._data._id = getpid() ;
		my_msg._data._num = rand() % (RANGE - 1) + 2;
		if(is_prime(my_msg._data._num))
		{
			if (msgsnd(msqid, &my_msg,sizeof(struct Data), 0) == -1) 
			{
				perror("msgsnd failed");
				exit(EXIT_FAILURE);
			}
			prime[my_msg._data._num]++;
			if (msgrcv(msqid, &my_msg,sizeof(struct Data), (int) getpid(), 0) == -1) {
				perror("msgrcv failed");
				exit(EXIT_FAILURE);
			}
			//printf("%d get %d from server\n", (int) getpid(),my_msg._data._num) ;
			int feedback = my_msg._data._num;
			if(feedback == 0) new_numbers++;
			if(feedback == END) break;
		}
	}
	printf("%d\n", new_numbers);
	//get how much times max numver appears
	if (msgrcv(msqid, &my_msg,sizeof(struct Data), (int) getpid(), 0) == -1) 
	{
		perror("msgrcv failed");
		exit(EXIT_FAILURE);
	}
	int max = my_msg._data._num;
	printf("%d\n",max);
	exit(EXIT_SUCCESS);
}
//.............................................................................
// this function receives a wanted number and returns if it is a prime number
// or not
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
//max numbers sent for this client
int find_max_sent(int* arr)
{
	int max = 0;
	for(int i =0;i < RANGE; i ++)
	{
		if(arr[i] > max) max = arr[i];
	}
	return max;
}