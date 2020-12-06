 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

//this is added for the TCP connection
#include <signal.h>

//These are the ports that will be used during the communication
#define MAINPORT "22913"
#define DBPORT "23913"
#define CALCPORT "24913"

#define MAXBUFLEN 100

//This is to receive the information is for the TCP connection
#define MAXDATASIZE 100

#define BACKLOG 10 //how many pending connections queue will hold

//TCP PORT
#define TCPPORT "21913" //the port users will be connecting to

//The following function is for the TCP connection
void sigchld_handler(int s){
   //waitpid() might overwrite errno, so we save and restore it:
   int saved_errno = errno;
   
   while(waitpid(-1, NULL, WNOHANG) > 0);
   
   errno = saved_errno;
}

//I added this part
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//I added the previous part

// the port users will be connecting to
int main(int argc, char *argv[]){

	/****This part is added for CLIENT TCP connection****/

	int tcp_sockfd, tcp_new_fd; //listen on tcp_sockfd, new connection on tcp_new_fd
	struct addrinfo tcp_hints, *tcp_servinfo, *tcp_p;
	struct sockaddr_storage tcp_their_addr; //connector's address information
	socklen_t tcp_sin_size;
	struct sigaction tcp_sa;
  	int tcp_yes=1;
  	char tcp_s[INET6_ADDRSTRLEN];
  	int tcp_rv;

  	//This is to receive information, I created this variables
  	char tcp_buf[MAXDATASIZE];
  	int tcp_numbytes;

  	memset(&tcp_hints, 0, sizeof tcp_hints);
  	tcp_hints.ai_family = AF_UNSPEC;
  	tcp_hints.ai_socktype = SOCK_STREAM;
  	tcp_hints.ai_flags = AI_PASSIVE; //use my IP

  	if((tcp_rv = getaddrinfo(NULL, TCPPORT, &tcp_hints, &tcp_servinfo)) != 0){
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(tcp_rv));
    	return 1;
  	}

  	//loop through all the results and bind to the first we can
  	for(tcp_p = tcp_servinfo; tcp_p != NULL; tcp_p = tcp_p->ai_next){
    	if ((tcp_sockfd = socket(tcp_p->ai_family, tcp_p->ai_socktype,
        	tcp_p->ai_protocol)) == -1) {
        	perror("server: socket");
        	continue;
    	}

    	if (setsockopt(tcp_sockfd, SOL_SOCKET, SO_REUSEADDR, &tcp_yes,
        	sizeof(int)) == -1) {
        	perror("setsockopt");
        	exit(1);
    	}

    	if (bind(tcp_sockfd, tcp_p->ai_addr, tcp_p->ai_addrlen) == -1) {
        	close(tcp_sockfd);
        	perror("server: bind");
        	continue;
      	}
      	break;
   	}

   	freeaddrinfo(tcp_servinfo); // all done with this structure

   	if (tcp_p == NULL)  {
    	fprintf(stderr, "server: failed to bind\n");
    	exit(1);
   	}

   	if (listen(tcp_sockfd, BACKLOG) == -1) {
    	perror("listen");
    	exit(1);
   	}

   	tcp_sa.sa_handler = sigchld_handler; // reap all dead processes
   	sigemptyset(&tcp_sa.sa_mask);
   	tcp_sa.sa_flags = SA_RESTART;
   	if (sigaction(SIGCHLD, &tcp_sa, NULL) == -1) {
    	perror("sigaction");
    	exit(1);
   	}
   
   	/*****************************************************/

   	/****This part is added for UDP connection****/

   	int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    //I added this part to listen from dbServer and calcServer
    struct addrinfo *servinfoTwo, *pTwo;
    int sockfdTwo;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;


    if ((rv = getaddrinfo("127.0.0.1", MAINPORT, &hints, &servinfoTwo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(pTwo = servinfoTwo; pTwo != NULL; pTwo = pTwo->ai_next) {
        if ((sockfdTwo = socket(pTwo->ai_family, pTwo->ai_socktype,
                pTwo->ai_protocol)) == -1) {
            perror("mainServer: source socket");
            continue;
        }
        if (bind(sockfdTwo, pTwo->ai_addr, pTwo->ai_addrlen) == -1) {
            close(sockfdTwo);
            perror("mainServer: source bind");
            continue;
        }
        break;
    }

    if (pTwo == NULL) {
        fprintf(stderr, "mainServer: failed to create source socket\n");
        return 2;
    }

    /***dbServer***/
    //I added the previous part to talk to dbServer

    if ((rv = getaddrinfo("127.0.0.1", DBPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("mainServer: dbServer destination socket");
			continue;
		}
		break;
	}

    if (p == NULL) {
        fprintf(stderr, "mainServer: failed to create destination socket for dbServer\n");
        return 2;
	}

    /***calcServer***/

    //Now, I have to add the following part to talk to calcServer
    struct addrinfo *servinfoThree, *pThree;
    int sockfdThree;

    if ((rv = getaddrinfo("127.0.0.1", CALCPORT, &hints, &servinfoThree)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and make a socket
    for(pThree = servinfoThree; pThree != NULL; pThree = pThree->ai_next) {
        if ((sockfdThree = socket(pThree->ai_family, pThree->ai_socktype,
                pThree->ai_protocol)) == -1) {
            perror("mainServer: calcServer destination socket");
            continue;
        }
        break;
    }

    if (pThree == NULL) {
        fprintf(stderr, "mainServer: failed to create destination socket for calcServer\n");
        return 2;
    }

   	/*****************************************************/

    //The server is up and running, it created all the connections
    printf("The Main Server is up and running.\n");

    //While loop should be here
    while(1){
    	/****This part is added for CLIENT TCP connection****/
		tcp_sin_size = sizeof tcp_their_addr;
      	tcp_new_fd = accept(tcp_sockfd, (struct sockaddr *)&tcp_their_addr, &tcp_sin_size);
      	if (tcp_new_fd == -1) {
        	perror("accept");
        	continue;
      	}

      	inet_ntop(tcp_their_addr.ss_family,
        	get_in_addr((struct sockaddr *)&tcp_their_addr),
        	tcp_s, sizeof tcp_s);

     	if(!fork()){ //this is the child process

      		//I added the following code.
      		if((tcp_numbytes = recv(tcp_new_fd, tcp_buf, MAXDATASIZE-1, 0)) == -1){
            	perror("recv");
            	exit(1);
        	}

        	//This is to close the string of chars
        	tcp_buf[tcp_numbytes] = '\0';

        	//Parse message from client.c
        	char clientID[MAXDATASIZE] = "";
        	char clientSize[MAXDATASIZE] = "";
        	char* tcp_token = strtok(tcp_buf, "-");
        	strcat(clientID,tcp_token);
        	tcp_token = strtok(NULL,"-");
        	strcat(clientSize,tcp_token);

        	printf("Receive Link %s, file size %s MB.\n", clientID, clientSize);

        	/****This part is added for UDP connection****/

			//Send ID to dbServer
        	if ((numbytes = sendto(sockfd, clientID, strlen(clientID), 0,
            	p->ai_addr, p->ai_addrlen)) == -1) {
          		perror("mainServer: sendto");
		  		exit(1);
	   		}

        	//Free memory after sending data
        	freeaddrinfo(servinfo);

        	//Free memory before receiving data
        	freeaddrinfo(servinfoTwo);

        	//Upon sending to Database Server.
        	printf("Send Link %s to database server.\n",clientID);

			//I am receiving information from database server
        	char buf[MAXBUFLEN] = " ";
        	if ((numbytes = recvfrom(sockfdTwo, buf, MAXBUFLEN-1 , 0,
                	(struct sockaddr *)&their_addr, &addr_len)) == -1) {
          		perror("recvfrom");
          		exit(1);
        	}

			//Parsing the received array
        	char* token = strtok(buf, "-");
        	int receivedVal = atoi(token);
        	int intInputID =  atoi(clientID);
        	char capacity[MAXBUFLEN] = "";
        	char length[MAXBUFLEN] = "";
        	char velocity[MAXBUFLEN] = "";

        	//Compare user input to ID received from dbServer
       		if(receivedVal == intInputID){

          	//Saving string from dbServer to char[] variables
          	token = strtok(NULL, "-");
          	strcat(capacity, token);
          	token = strtok(NULL, "-");
          	strcat(length, token);
          	token = strtok(NULL, "-");
          	strcat(velocity, token);

          	//Print out information received from dbServer
          	printf("Receive link capacity %sMbps, link length %skm, and propagation velocity %skm/s.\n",capacity, length, velocity);

        }
        else{
        	//If received "No match found." continue to next iteration
          	printf("Receive %s\n",buf);

        	//sending information to the client
          	if (send(tcp_new_fd, buf, MAXDATASIZE-1, 0) == -1){
            	//if the message was not send, create an error message
            	perror("send");
          	}

			close(tcp_new_fd); //closing the child process
				
          //Skip iteration
          continue;
        }

        //Now, send information to calcServer
        //Create the string that will be sent to calcServer
        char preCalcBuf[MAXBUFLEN] = "";
        strcat(preCalcBuf, clientSize);
        strcat(preCalcBuf, "-");
        strcat(preCalcBuf,capacity);
        strcat(preCalcBuf, "-");
        strcat(preCalcBuf,length);
        strcat(preCalcBuf, "-");
        strcat(preCalcBuf,velocity);

        if ((numbytes = sendto(sockfdThree, preCalcBuf, strlen(preCalcBuf), 0,
                pThree->ai_addr, pThree->ai_addrlen)) == -1) {
          perror("calcServer: sendto");
          exit(1);
        }
        //Free memory after sending string
        freeaddrinfo(servinfoThree);

        //Upon sending to Calculation Server
        printf("Send information to calculation server.\n");

        //Receive information from calcServer
        char calcBuf[MAXBUFLEN] = "";
        if ((numbytes = recvfrom(sockfdTwo, calcBuf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
          perror("recvfrom");
          exit(1);
        }

        //Make a deep copy to sent it to client.c
        int sizeArray = sizeof(calcBuf)/sizeof(calcBuf[0]);
        char resultClient[sizeArray];
        for(int i = 0; i < sizeArray; i++){
          resultClient[i] = calcBuf[i];
        }

        //Parse input from calcServer
        char transDelay[MAXBUFLEN] = "";
        char propDelay[MAXBUFLEN] = "";
      	char totDelay[MAXBUFLEN] = "";
        char* calcToken = strtok(calcBuf, "-");
      	strcat(transDelay,calcToken);
      	calcToken = strtok(NULL, "-");
      	strcat(propDelay, calcToken);
      	calcToken = strtok(NULL, "-");
       	strcat(totDelay, calcToken);

        //Print out final message to user
        printf("Receive transmission delay %.2fms, propagation delay %.2fms, and total delay %.2fms.\n", atof(transDelay), atof(propDelay), atof(totDelay));

        /*****************************************************/

        //Sending information to the client 
        if (send(tcp_new_fd, resultClient, MAXDATASIZE-1, 0) == -1){
          //if the message was not send, create an error message
          perror("send");
        }

        close(tcp_new_fd); //closing the child process
        exit(0); //I might have to delete this

    	}
    	/*********************************************/

    }//end of while loop

    close(tcp_sockfd);

	return 0;
}
