/*
 * ex6a1.c
 * Written by: Rani Abu Raia, id = 316396787, login = abura
 *
 * DESCRIPTION: this program works as a  server, the server has to fill araay of size
 * 				1000 with random prime numbers from the client, t
 * 				when all cells of array are filled  the server sends message to all cleints
 * 				to end and print distinct,max and min in the array.
 *
*/
// --------------INCLUDE-------------------------------------------------------------------
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

// --------------DEFINE-------------------------------------------------------------------

#define ARR_SIZE 1000
#define RANGE 1000
#define NUM_OF_PROC 3
#define PORT "17171"

// --------------CONST-------------------------------------------------------------------
const int FIRST_APPERANCE = 0;
const int START = 1;
const int END = -1;
const int INT = sizeof(int);

// --------------prototype---------------------------------------------------------------
int count_distinct(int arr[]);
int max_val(int* arr);
int min_val(int* arr);

//---------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
	int rc, main_socket, serving_socket, fd;
	fd_set rfd, c_rfd;
	int num;
	int arr[ARR_SIZE]={0};
	int prime[RANGE]={1};
	for(int i=0; i < RANGE; i++) prime[i] = 1;
	
	struct addrinfo con_kind, *addr_info_res ;

	memset(&con_kind, 0, sizeof con_kind) ;
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	con_kind.ai_flags = AI_PASSIVE ;
    
	if ((rc = getaddrinfo(NULL, PORT, &con_kind, &addr_info_res)) != 0)
	{
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc)) ;
		exit(EXIT_FAILURE) ;
	}

	main_socket = socket(addr_info_res -> ai_family, addr_info_res -> ai_socktype, addr_info_res -> ai_protocol);
	if (main_socket < 0)
	{
		perror("socket: allocation failed");
		exit(EXIT_FAILURE) ;
	}
	

	rc = bind(main_socket, addr_info_res-> ai_addr, addr_info_res-> ai_addrlen);
	if (rc)
	{
		perror("bind failed");
		exit(EXIT_FAILURE) ;
	}
	
	rc = listen(main_socket, 5);
	if (rc)
	{
		perror("listen failed");
		exit(EXIT_FAILURE) ;
	}
	
	FD_ZERO(&rfd);
	FD_SET(main_socket, &rfd);
	

	
	
	int i = 0;
	int feedback;
	int client_r = 0;
	int client_w = 0;
	while (i <= ARR_SIZE )
	{
		c_rfd =rfd;
		rc = select(getdtablesize(),&c_rfd,NULL,NULL,NULL);
		if(FD_ISSET(main_socket,&c_rfd))
		{
			serving_socket = accept(main_socket,NULL,NULL);
			if(serving_socket >= 0){
				FD_SET(serving_socket,&rfd);
			}
			
		}
		
		
		for (fd= main_socket +1; fd < getdtablesize(); fd++)
		{
			if (FD_ISSET(fd, &c_rfd))
			{

				if(client_r < 3)
				{
					client_r++;
					rc = read(fd, &num, INT);

				}
				if(client_r == 3)
				{
					if(client_w < 3)
					{
						fd= main_socket+1;
						
						write(fd,&START,INT);
						write(fd+1,&START,INT);
						write(fd+2,&START,INT);
						client_w++;
					}
					if(client_w == 3)
					{
						
						rc = read(fd, &num, INT);
						if(i == ARR_SIZE)
						{
							feedback = END;
							write(fd,&feedback,INT);
							i++;
						}
						if(rc == 0)
						{
							close(fd);
							FD_CLR(fd,&rfd);
						}
						else if(rc > 0 && i < ARR_SIZE)
						{
							arr[i] = num;
							i++;
							if(prime[num] == 1) feedback = FIRST_APPERANCE;
							else feedback = prime[num];	
							prime[num]++;
							write(fd,&feedback,INT);
						}
					}
					
				}
				
				
			}
		}
		
	}

	int distinct,mx,mn;
	distinct = count_distinct(arr);
	mx = max_val(arr);
	mn = min_val(arr);
	printf("%d %d %d\n",distinct,mx,mn);
	exit(EXIT_SUCCESS);
}
//--------------------------------------------------------------------------
/*
 * this func counts how many unique and distinct numbers there are in the array
 *  it returns the result
 */ 
int count_distinct(int arr[])
{
    int res = 1;
    int i,j;
    // Pick all elements one by one
    for (i = 2; i < ARR_SIZE; i++) 
    {
        for (j = 1; j < i; j++)
            if(arr[i] == arr[j])
                break;
        if (i == j)
        	res++;
        
            
    }
    return res;
}
//-----------------------------------------------------------------------------
/*
 * a func to finc the maximal value in the array
 */
int max_val(int* arr)
{
	int i;
    int max = arr[0];
    for (i = 1; i < ARR_SIZE; i++)
        if (*(arr + i) > max)
            max = *(arr + i);
 
    return max;
}
//-----------------------------------------------------------------------------
/*
 * a func to finc the minimal value in the array
 */
int min_val(int* arr)
{
	int i;
    int min = arr[0];
    
    for (i = 1; i < ARR_SIZE; i++)
        if (*(arr + i) < min)
            min = *(arr + i);
 
    return min;
}
