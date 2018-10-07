/* CSCI 367 Lexiguess: prog1_server.c
 *
 * 4 OCT 2018, Zach Richardson and Mitch Kimball
 */

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <netdb.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <unistd.h>
#include <wait.h>

#define QLEN 6 /* size of request queue */
 int visits = 0; /* counts client connections */

 /*------------------------------------------------------------------------
 *  Program: demo_server
 *
 *  Purpose: allocate a socket and then repeatedly execute the following:
 *  (1) wait for the next connection from a client
 *  (2) send a short message to the client
 *  (3) close the connection
 *  (4) go back to step (1)
 *
 *  Syntax: ./demo_server port
 *
 *  port - protocol port number to use
 *
 *------------------------------------------------------------------------
 */

#define MAXWORDSIZE 254

int isWon(char* board) {
    for(int i = 0; i < strlen(board); i++) {
        if(board[i] == '_') {
            return 0;
        }
    }
    return 1;
}

int check_guess(char guess, char* board, char* word) {
    int guessed = 0;
    for(int i = 0; i < strlen(board);i++) {
        if (guess == (board[i])) {
            guessed = 0;
            break;
        } else if(guess == word[i]) {
            guessed = 1;
            board[i] = word[i];
        }
    }
    return guessed;
}

 /*main game loop for server*/
 void play_game(char* word,int c_sd){
     //max word size is 254 so indecies should be 0-253
     char boardbuffer[MAXWORDSIZE+1];
     int wordlength = (int)strlen(word);
     //the number of guesses based on the length of the word
     uint8_t numguesses = (uint8_t)wordlength;
     //used to "hide" the word -> using _ instead of the characters
     char* displayword = (char*) malloc((strlen(word)+1) * sizeof(char));
     char guess;
     int isCorrect = 0;
     //char* guessedletters = (char*) malloc(27);

     //memset(guessedletters,0,sizeof(guessedletters));
     memset(boardbuffer,0,sizeof(boardbuffer));

     //TODO: declare i at top of function
     for (int i = 0; i < wordlength; ++i) {
         displayword[i] = '_';
     }
     //null terminate the board
     displayword[wordlength] = 0;
     //TODO: add checking if the word is solved to while loop
     while(numguesses > 0) {
         //send N --> seems to work
         send(c_sd,&numguesses,sizeof(numguesses),0);

         //prepare to send the board
         sprintf(boardbuffer,"%s",displayword);


         send(c_sd,boardbuffer,(size_t)wordlength,0);//TODO: Send the board without the null terminator
         //recieve the guess
         recv(c_sd,&guess,1,0);

         isCorrect = check_guess(guess,displayword,word);
         if(!isCorrect) {
             numguesses--;
         } else if (isWon(displayword)) {
             numguesses = 255;
             send(c_sd,&numguesses,sizeof(numguesses),0);
             sprintf(boardbuffer,"%s",displayword);
             send(c_sd,boardbuffer,(size_t)wordlength,0);
         }
     }
     send(c_sd,&numguesses,sizeof(numguesses),0);
     sprintf(boardbuffer,"%s",displayword);
     send(c_sd,boardbuffer,(size_t)wordlength,0);
 }

int main(int argc, char **argv) {
  struct protoent *ptrp; /* pointer to a protocol table entry */
  struct sockaddr_in sad; /* structure to hold server's address */
  struct sockaddr_in cad; /* structure to hold client's address */

  int sd, sd2; /* socket descriptors */
  int port; /* protocol port number */
  int alen; /* length of address */
  int optval = 1; /* boolean value when we set socket option */
  char buf[1000]; /* buffer for string the server sends */
  port = atoi(argv[1]); /* convert argument to binary */
  char* word = argv[2];/*get the secret word*/


  if( argc != 3 ) {
    fprintf(stderr,"Error: Wrong number of arguments\n");
    fprintf(stderr,"usage:\n");
    fprintf(stderr,"./prog1_server server_port\n");
    exit(EXIT_FAILURE);
  }

  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */

  //TODO: Set socket family to AF_INET
  sad.sin_family = AF_INET;

  //TODO: Set local IP address to listen to all IP addresses this server can assume. You can do it by using INADDR_ANY
  sad.sin_addr.s_addr = INADDR_ANY;



  if (port > 0) { /* test for illegal value */
    //TODO: set port number. The data type is u_short
    sad.sin_port = htons(port);
  } else { /* print error message and exit */
    fprintf(stderr,"Error: Bad port number %s\n",argv[1]);
    exit(EXIT_FAILURE);
  }

  /* Map TCP transport protocol name to protocol number */
  if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
    fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
    exit(EXIT_FAILURE);
  }

  /* TODO: Create a socket with AF_INET as domain, protocol type as
  SOCK_STREAM, and protocol as ptrp->p_proto. This call returns a socket
  descriptor named sd. */
  sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0) {
    fprintf(stderr, "Error: Socket creation failed\n");
    exit(EXIT_FAILURE);
  }

  /* Allow reuse of port - avoid "Bind failed" issues */
  if( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ) {
    fprintf(stderr, "Error Setting socket option failed\n");
    exit(EXIT_FAILURE);
  }

  /* TODO: Bind a local address to the socket. For this, you need to
  pass correct parameters to the bind function. */
  if (bind(sd, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
    fprintf(stderr,"Error: Bind failed\n");
    exit(EXIT_FAILURE);
  }

  /* TODO: Specify size of request queue. Listen take 2 parameters --
  socket descriptor and QLEN, which has been set at the top of this code. */
  if (listen(sd, QLEN) < 0) {
    fprintf(stderr,"Error: Listen failed\n");
    exit(EXIT_FAILURE);
  }
     pid_t pid; //process id of the child processes.
  /* Main server loop - accept and handle requests */
  while (1) {
    alen = sizeof(cad);
    if ((sd2 = accept(sd, (struct sockaddr *)&cad, &alen)) < 0) {
      fprintf(stderr, "Error: Accept failed\n");
      exit(EXIT_FAILURE);
    }
    //TODO fork here and implement logic
    //int status;
    //visits++;
    //sprintf(buf,"This server has been contacted %d time%s\n",visits,visits==1?".":"s.");
    //send(sd2,buf,strlen(buf),0);
    //close(sd2);

    pid = fork();
    if (pid < 0) {
      perror("Error Fork() failure");
    }
      //we are in the child process.
    else if (pid == 0) {

      //play hangman
      play_game(word,sd2);

      close(sd2);

      exit(EXIT_SUCCESS);
    }
      //close(sd2);

    /*
    visits++;
    sprintf(buf,"This server has been contacted %d time%s\n",visits,visits==1?".":"s.");
    //sprintf(buf,"slam");
    send(sd2, buf, strlen(buf),0);
    close(sd2);*/
  }
}
