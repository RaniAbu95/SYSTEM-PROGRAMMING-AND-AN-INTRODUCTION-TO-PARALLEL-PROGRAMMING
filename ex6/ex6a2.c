/*
 * ex6a2.c
 * Written by:  Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program works as clients server, which allows client to
 * 				send numbers to server in order to put it in the array,
 * 				clients stop sending numbers when all cells of the array are filled.
 * 				when finished clients prints how many new numbers were sent and what 
 * 				is the max apperance of a given number sent.
 *
*/
// --------------INCLUDE---------------------------------------------------------------
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
#include <netinet/in.h> /* Internet address structures */
#include <arpa/inet.h>
#include <sys/socket.h> /* socket interface functions */
#include <netdb.h>
#include <stdbool.h>

// --------------CONST------------------------------------------------------------------
const int RANGE = 1000;
const int TRUE = 1;
const int END = -1;
const int START = 1;
const int FALSE = 0;
const int NEW_NUMBER = 0;
const int INT = sizeof(int);
const int FIRST_RND_NUM = 2;
const int NUM_OF_ARGS = 3;

// --------------DEFINE-----------------------------------------------------------------
#define NUM_OF_PROC 3

// --------------prototype--------------------------------------------------------------
bool is_prime(int wanted);

//---------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	int process_id = atoi(argv[1]);
	srand(process_id);
	
	int rc; /* return code of s.c. */
	int my_socket;
	int num;
	struct addrinfo con_kind, *addr_info_res ;
	
	if (argc < NUM_OF_ARGS)
	{
		fprintf(stderr, "Usage: './client' <id> <host name> <port> \n") ;
		exit(EXIT_FAILURE) ;
	}

	memset(&con_kind, 0, sizeof(con_kind));
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	
	
	if ((rc = getaddrinfo(argv[2], argv[3], &con_kind, &addr_info_res) != 0))
	{
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc)) ;
		exit(EXIT_FAILURE) ;
	}
	
	struct addrinfo *p;
	for(p = addr_info_res; p!=NULL;p=p->ai_next)
	{
		if((my_socket = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
		{
			 perror("socket: allocation failed");
			 continue;
		}
		if(connect(my_socket ,p->ai_addr,p->ai_addrlen) == -1)
		{
			close(my_socket);
			continue;
		}
		break;
	}
	if(p==NULL)
	{
		fprintf(stderr,"client: failed to connect\n");
		exit(EXIT_FAILURE) ;
	}

	write(my_socket,&START,INT);//client sends msg to server to start
	int s;
	rc=read(my_socket,&s,INT);//get start message to start send prime numbers
	
	int feedback;
	int flag = TRUE;
	int new_numbers = 0;
	int max_feedback = 0;
	if(s == START)
	{
		
		while (flag)
		{
			 num =  rand() % RANGE + FIRST_RND_NUM;
			 while(!is_prime(num)) num =  rand() % RANGE + FIRST_RND_NUM;
			 write(my_socket,&num,INT);
			 rc = read(my_socket,&feedback,INT);
			 if(rc == -1) break;
			 if(rc > 0)
			 {
				//printf("server send %d\n",feedback);
				if(feedback == END) flag = FALSE; // server send end msg
				if(feedback == NEW_NUMBER) new_numbers++; // new number has been sent
				if(max_feedback < feedback) max_feedback = feedback; // number of occurances of number 
			 }
		 }
	}
	
	printf("%d %d\n", new_numbers, max_feedback);//print feedback collected
	close(my_socket);
	freeaddrinfo(addr_info_res);
	
	exit(EXIT_SUCCESS);
}
/*
 *  this function receives a wanted number and returns if it is a prime number
 *  or not
 */
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


