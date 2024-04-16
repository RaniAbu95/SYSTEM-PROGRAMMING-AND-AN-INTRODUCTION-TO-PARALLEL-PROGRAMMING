/* OS1, 2022
 * Ex6b3: shared memory
 * =================================================================================
 * Written by/ID: Rani Abu Raia-316396787
 * login: abura
 * .............
 *
 *  DESCRIPTION: this program running a client sockit which interact with two
 *   			 servers.the client recieves inputs from the user(p for int and q 
 *   			 for string).recieve output from this server based on the input - if
 *   			 p entered checks if the number is prime, if q checks if the string
 *   			 is palindrome and print the answer.
 *
 * inputs: a number or a series of characters
 *
 * outputs: 1 or 0 if the number is prime or not, or 1 or 0 if
	 	 	the series of characters is a palindrome or not
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

//-----------------------------const section-----------------------------------
const int TRUE = 1;
const int INT = sizeof(int);
const int FIVE = 5;
const int FIRST = 1;
const int SECOND = 2;
const int THIRD = 3;
const int FOURTH = 4;
const int MAX_STR_LEN  = 100;
const char* IP = "localhost";
const char P =  'p';
const char Q = 'q';

//-------------------------------prototypes------------------------------------
void print_arr(int *arr);
int create_socket(char* ip, char* port, struct addrinfo *addr_info_res);
void check_argc(int argc, char **argv);

//------------------------------main section-----------------------------------

int main(int argc, char **argv)
{
	int my_socket_prime, my_socket_palin;
	struct addrinfo *addr_info_res_prime = NULL, *addr_info_res_palin=NULL;
	int number;
	char s[MAX_STR_LEN];
	check_argc(argc, argv);
	//creating a socket for primacy
	my_socket_prime = create_socket(argv[FIRST],argv[SECOND],addr_info_res_prime);
	//creating a socket for plindrome
	my_socket_palin  = create_socket(argv[THIRD],argv[FOURTH],addr_info_res_palin);
	
	 while (TRUE)
	 {
		 char c;
		 scanf("%c", &c);
		 if(c == P)//if prime send to prime server
		 {
			 scanf("%d", &number);
			 int result;
			 write(my_socket_prime, &number, INT);
			 read(my_socket_prime, &result, INT);
			 printf("%d\n", result);

		 }
		 else if(c == Q)//if palindrome send to prime server
		 {
			 int ans;
			 char* str = s;
			 scanf("%s", str);
			 int length = strlen(str); 
			 if(length > MAX_STR_LEN) printf("Bad Input: length > %d\n",MAX_STR_LEN);
			 write(my_socket_palin, &s, length + 1);
			 read(my_socket_palin, &ans, INT);
			 printf("%d\n",ans);
		 }
	 }
	 close(my_socket_prime);
	 close(my_socket_palin);
	 freeaddrinfo(addr_info_res_palin);
	 freeaddrinfo(addr_info_res_prime);
	 exit(EXIT_SUCCESS);
}

//------------------------------func section-----------------------------------
/*
 * this function creates a socket by recieving the ip address and ports 
 * needed
 */

int create_socket(char* ip, char* port, struct addrinfo *addr_info_res)
{
	int my_socket;
	int rc; // system calls return value storage
	struct addrinfo  con_kind;

	memset(&con_kind, 0, sizeof(con_kind));
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	if ((rc = getaddrinfo(IP, port, &con_kind, &addr_info_res) != 0))
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

	return my_socket;
}
//--------------------------------------------------------------------------
/*
 * this func checks if argc ad argv works fine
 */
void check_argc(int argc, char **argv)
{
	if (argc < FIVE)
	{
		fprintf(stderr, "Usage: %s <ip1> <port1> <ip2> <port2> \n", argv[0]) ;
		exit(EXIT_FAILURE) ;
	}
}
