/* CSCI 367 Lexiguess
 *
 * 4 OCT 2018, Zach Richardson and Mitch Kimball
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
  struct protoent *ptrp; /* pointer to a protocol table entry */
  struct sockaddr_in sad; /* structure to hold server's address */
  struct sockaddr_in cad; /* structure to hold client's address */

  int sd, sd2; /* socket descriptors */
  int port; /* protocol port number */
  int alen; /* length of address */
  int optval = 1; /* boolean value when we set socket option */
  char buf[1000]; /* buffer for string the server sends */

  if( argc != 2 ) {
    fprintf(stderr,"Error: Wrong number of arguments\n");
    fprintf(stderr,"usage:\n");
    fprintf(stderr,"./server server_port\n");
    exit(EXIT_FAILURE);
  }


}
