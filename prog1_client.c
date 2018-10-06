/* CSCI 367 Lexiguess: prog1_client.c
 *
 * 4 OCT 2018, Zach Richardson and Mitch Kimball
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*------------------------------------------------------------------------
* Program: demo_client
*
* Purpose: allocate a socket, connect to a server, and print all output
*
* Syntax: ./demo_client server_address server_port
*
* server_address - name of a computer on which server is executing
* server_port    - protocol port number server is using
*
*------------------------------------------------------------------------
*/

#define MAXWORDSIZE 254

/*main game loop for clients*/
void play_game(int sd) {

    char guess;
    uint8_t numguesses=0;
    

    //read the number of guesses
    int n = read(sd,&numguesses,sizeof(numguesses));

    //create a word buffer with size numguesses+1 to store the word + a null terminator
    char wordbuf[MAXWORDSIZE+1];
    memset(wordbuf,0,sizeof(wordbuf)); //this essentially adds the null  terminator to the board for us.

    //read the board, wait for all 6 characters of the word binary
    n = recv(sd,wordbuf,MAXWORDSIZE,0);

    //add the null terminator
    //wordbuf[*numguesses] = 0;

    printf("%d\n",numguesses);
    //TODO: sometimes prints out random characters or no characters at all
    printf("%s\n",wordbuf);

}

int main( int argc, char **argv) {
	struct hostent *ptrh; /* pointer to a host table entry */
	struct protoent *ptrp; /* pointer to a protocol table entry */
	struct sockaddr_in sad; /* structure to hold an IP address */
	int sd; /* socket descriptor */
	int port; /* protocol port number */
	char *host; /* pointer to host name */
	int n; /* number of characters read */
	char buf[1000]; /* buffer for data from the server */

	memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
	sad.sin_family = AF_INET; /* set family to Internet */

	if( argc != 3 ) {
		fprintf(stderr,"Error: Wrong number of arguments\n");
		fprintf(stderr,"usage:\n");
		fprintf(stderr,"./prog1_client server_address server_port\n");
		exit(EXIT_FAILURE);
	}

	port = atoi(argv[2]); /* convert to binary */
	if (port > 0) /* test for legal value */
		sad.sin_port = htons((u_short)port);
	else {
		fprintf(stderr,"Error: bad port number %s\n",argv[2]);
		exit(EXIT_FAILURE);
	}

	host = argv[1]; /* if host argument specified */

	/* Convert host name to equivalent IP address and copy to sad. */
	ptrh = gethostbyname(host);
	if ( ptrh == NULL ) {
		fprintf(stderr,"Error: Invalid host: %s\n", host);
		exit(EXIT_FAILURE);
	}

	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

	/* Map TCP transport protocol name to protocol number. */
	if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
		fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
		exit(EXIT_FAILURE);
	}

	/* Create a socket. */
	sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sd < 0) {
		fprintf(stderr, "Error: Socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	/* TODO: Connect the socket to the specified server. You have to pass correct parameters to the connect function.*/
	if (connect(sd, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		fprintf(stderr,"connect failed\n");
		exit(EXIT_FAILURE);
	}


	/* Repeatedly read data from socket and write to user's screen. */
    play_game(sd);
    /*
	n = recv(sd, buf, sizeof(buf), 0);
	while (n > 0) {
		write(1,buf,n);
		n = recv(sd, buf, sizeof(buf), 0);
	}
    */

	close(sd);

	exit(EXIT_SUCCESS);
}
