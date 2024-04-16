/* OS1, 2022
 * Ex6b1: socket
 * ============================================================================
 * Written by/ID: Rani Abu Raia-316396787
 * login: abura
 * .............
 *
 *  DESCRIPTION: this is a primacy server which creates a stream socket 
 *				 and recieves a number from the client through this socket.
 *				 the server checks whether the number is prime or not and send
 *				 apropriate message to the client (either 1 if it is a prime
 *				 num or 0 if not).
 *
 * input: <PORT1>
 *
 * output: none
 *
 *
 */

//-----------------------------include section----------------------------------
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

//-----------------------------DEFINE------------------------------------------
#define PORT "18181"

//-----------------------------const section-----------------------------------
const int TRUE = 1;
const int INT = sizeof(int);
const int TWO = 2;

//-----------------------------global------------------------------------------
int fd;
int main_socket;
struct addrinfo con_kind, *addr_info_res ;

//-------------------------------prototypes------------------------------------
int is_prime(int p_num);
void SIGINT_handler(int signo);

//------------------------------main section-----------------------------------
int main(int argc, char **argv)
{
	if (argc != TWO)
	{
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	signal(SIGINT,SIGINT_handler);
	
	int rc, serving_socket;
	fd_set rfd, c_rfd;
	int number;

	
	memset(&con_kind, 0, sizeof con_kind) ;
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	con_kind.ai_flags = AI_PASSIVE ;
    
	if ((rc = getaddrinfo(NULL, argv[1], &con_kind, &addr_info_res)) != 0)
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


	while (TRUE)
	{
		c_rfd =rfd;
		rc = select(getdtablesize(),&c_rfd,NULL,NULL,NULL);
		if(FD_ISSET(main_socket,&c_rfd))
		{
			serving_socket = accept(main_socket,NULL,NULL);
			if(serving_socket >= 0)
			{
				FD_SET(serving_socket,&rfd);
			}
		}
		for (fd = main_socket + 1; fd < getdtablesize(); fd++)
			if (FD_ISSET(fd, &c_rfd))
			{
				rc = read(fd, &number, sizeof(int));//read input from socket

				if (rc == 0)
				{
					close(fd);
					FD_CLR(fd, &rfd);
				}
				else if (rc > 0)
				{
					int status = is_prime(number);//check its primacy 
					write(fd,&status,INT);
				}
				else
				{
					perror("read() failed") ;
					exit(EXIT_FAILURE) ;
				}
			}
	}
	exit (EXIT_SUCCESS);
}

//------------------------------func section-----------------------------------
/*
 * this function recieves a number checks its primacy if its prime it returns 1
 * if not reutnrs 0
 */
int is_prime(int n)
{
    int i;
    if (n <= 1)
        return 0;
  
    for (i = 2; i < n; i++)
        if (n % i == 0)
            return 0;
  
    return 1;
}

//-----------------------------------------------------------------------------
/*
 * this is a signal handler 
 */
void SIGINT_handler(int signo)
{
	freeaddrinfo(addr_info_res);
	close(main_socket);
	close(fd);
	exit(EXIT_SUCCESS);
}
