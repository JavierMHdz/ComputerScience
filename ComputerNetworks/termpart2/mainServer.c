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

//These are the ports that will be used during the communication
#define MAINPORT "22913"
#define DBPORT "23913"
#define CALCPORT "24913"

#define MAXBUFLEN 100


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

    //The server is up and running, it created all the connections
    printf("The Main Server is up and running.\n");

    //While loop should be here
    while(1){
        char buf[MAXBUFLEN] = " ";
        //Request input from user
        char userInputID[MAXBUFLEN] = "";
        char fileSize[MAXBUFLEN] = "";
        //Printing out message to the user
        printf("Please input link ID and file size(<ID><size>):\n");
        //Saving input from the user
        scanf("%s %s",userInputID, fileSize);

        //Output to user after receiving input
        printf("Link %s, file size %sMB.\n", userInputID, fileSize);

        //Send ID to dbServer
        if ((numbytes = sendto(sockfd, userInputID, strlen(userInputID), 0,
                p->ai_addr, p->ai_addrlen)) == -1) {
            perror("mainServer: sendto");
		  exit(1);
	   }

        //Free memory after sending data
        freeaddrinfo(servinfo);

        //Free memory before receiving data
        freeaddrinfo(servinfoTwo);

        //Upon sending to Database Server.
        printf("Send Link %s to database server.\n",userInputID);

        //I added this part
        if ((numbytes = recvfrom(sockfdTwo, buf, MAXBUFLEN-1 , 0,
                (struct sockaddr *)&their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
        }

        //Parsing the received array
        char* token = strtok(buf, "-");
        int receivedVal = atoi(token);
        int intInputID =  atoi(userInputID);
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
            printf("Receive %s\n",token);
            //Skip iteration
            continue;
        }

        //Now, send information to calcServer
        //Create the string that will be sent to calcServer
        char preCalcBuf[MAXBUFLEN] = "";
        strcat(preCalcBuf, fileSize);
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
        
    }//end of while loop
    close(sockfd);
	return 0;
}