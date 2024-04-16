/* OS1, 2022
 * Ex6b2: socket
 * ============================================================================
 * Written by/ID: Rani Abu Raia-316396787
 * login: abura
 * .............
 *  DESCRIPTION: this is a palindrome server which creates a stream socket 
 *				 and recieves a string from the client through this socket. 
 *				 the server checks whether the string is palindrome or not and
 *				 send apropriate message to the client (either 1 if it is
 *				 palindrome string or 0 if not).
 *
 * inputs: <PORT2>
 *
 * outputs:	none
 *
 *
 */
//-----------------------------include section---------------------------------
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
#include <string.h>

//-----------------------------DEFINE------------------------------------------
#define PORT "18182"

//-----------------------------const section-----------------------------------
const int TRUE = 1;
const int MAX_STR_LEN = 100;
const int INT = sizeof(int);
const int TWO = 2;

//-----------------------------global------------------------------------------
int fd;
int main_socket;
struct addrinfo con_kind, *addr_info_res ;

//-------------------------------prototypes------------------------------------
int is_palindrome( char *str);
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
	char str[MAX_STR_LEN];
	int rc, serving_socket;
	fd_set rfd, c_rfd;

	memset(&con_kind, 0, sizeof con_kind) ;
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	con_kind.ai_flags = AI_PASSIVE ;

	if ((rc = getaddrinfo(NULL, argv[1], &con_kind, &addr_info_res)) != 0)
	{
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc)) ;
		exit(EXIT_FAILURE) ;
	}

	main_socket = socket(addr_info_res -> ai_family,
				  addr_info_res -> ai_socktype,	
				  addr_info_res -> ai_protocol);
	if (main_socket < 0)
	{
		perror("socket: allocation failed");
		exit(EXIT_FAILURE) ;
	}

	rc = bind(main_socket,
			  addr_info_res-> ai_addr,
			  addr_info_res-> ai_addrlen);
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
	FD_SET(main_socket, &rfd);//the procces of creating a socket
				//and checking if everything succeeded

	while (TRUE)
	{
		c_rfd = rfd;
		rc = select(getdtablesize(), &c_rfd, NULL, NULL,
				   (struct timeval *) NULL);
		if (FD_ISSET(main_socket, &c_rfd))
		{
			serving_socket = accept(main_socket, NULL, NULL);
			if (serving_socket >= 0)
				FD_SET(serving_socket, &rfd);
		}
		for (fd= main_socket +1; fd < getdtablesize(); fd++)
			if (FD_ISSET(fd, &c_rfd))
			{
				rc = read(fd, &str, MAX_STR_LEN);//read from socket

				if (rc == 0)
				{
					close(fd);
					FD_CLR(fd, &rfd);
				}
				else if (rc > 0)
				{
					int result = is_palindrome(str);//check for plindrome
					write(fd, &result, INT);
				}
				// check that write went okay...
				else
				{
					perror("read() failed") ;
					exit(EXIT_FAILURE) ;
				}
			}
	}
	exit(EXIT_SUCCESS);
}

//------------------------------func section-----------------------------------
//this function recieves a number and an array that has a series of integers
//checks the plindrome of this series, returns 1 if its indeed a palindrome 
//and 0 if not
int is_palindrome( char *str)
{
	int i, length;
	int flag = 1;
	
	length = strlen(str);
	
	for(i=0;i < length ;i++)
	{
		if(str[i] != str[length-i-1])
		{
			flag = 0;
			break;
		}
	}
	return flag;	
}

//-----------------------------------------------------------------------------
//this is a signal handler
void SIGINT_handler(int signo)
{
	freeaddrinfo(addr_info_res);
	close(main_socket);
	close(fd);
	exit(EXIT_SUCCESS);
}
