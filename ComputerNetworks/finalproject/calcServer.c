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

//Ports used in this file
#define MAINPORT "22913"
#define CALCPORT "24913"

#define MAXBUFLEN 100

//listener
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void){
	//listener part
	int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    //I wrote the next part to be able to talk to mainServer
    struct addrinfo *servinfoTwo, *pTwo;
    int sockfdTwo;

    //this is to talk (destionation port)
    if ((rv = getaddrinfo("127.0.0.1", MAINPORT, &hints, &servinfoTwo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(pTwo = servinfoTwo; pTwo != NULL; pTwo = pTwo->ai_next) {
        if ((sockfdTwo = socket(pTwo->ai_family, pTwo->ai_socktype,
                pTwo->ai_protocol)) == -1) {
            perror("calcServer: destination socket");
            continue;
        }
        break;
    }

    if (pTwo == NULL) {
        fprintf(stderr, "calcServer: failed to create destination socket\n");
        return 2;
    }

    //this is to listen (source port)
    if ((rv = getaddrinfo("127.0.0.1", CALCPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("calcServer: source socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("calcServer: source bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "calcServer: failed to bind source socket\n");
        return 2;
    }

    addr_len = sizeof their_addr;

    //Print out message to user that the Calculation Server is working
    printf("The Calculation Server is up and running.\n");
    

    //Use the while loop to always be up and running
    while(1){
    	//Free memory before receiving
    	freeaddrinfo(servinfo);

    	//Creating a buffer for input
    	char buf[MAXBUFLEN] = "";
    	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        //Let user know that the request from Main Server was received
        printf("Receive request from Main Server.\n");
        
        //Parsing the received array
        char sizeFileBuf[MAXBUFLEN] = "";
        char capacityBuf[MAXBUFLEN] = "";
        char lengthBuf[MAXBUFLEN] = "";
        char velocityBuf[MAXBUFLEN] = "";
        char* token = strtok(buf, "-");
        strcat(sizeFileBuf, token);
        token = strtok(NULL, "-");
        strcat(capacityBuf, token);
        token = strtok(NULL, "-");
        strcat(lengthBuf, token);
        token = strtok(NULL, "-");
        strcat(velocityBuf, token);

        //Now convert the strings to integers
        int sizeFile = atoi(sizeFileBuf);
        sizeFile *= 8;
        int capacity = atoi(capacityBuf);
        int length = atoi(lengthBuf);
        int velocity = atoi(velocityBuf);

        //Compute calculations
        double transmissionDelay = 1000.00 * (double)sizeFile/capacity;
        double propagationDelay = 1000.00 * (double)length/velocity;
        double totalDelay = transmissionDelay + propagationDelay;

        //Create strings to save the response
        char transDelay[MAXBUFLEN] = "";
        char propDelay[MAXBUFLEN] = "";
        char totDelay[MAXBUFLEN] = "";

        gcvt(transmissionDelay, MAXBUFLEN, transDelay);
        gcvt(propagationDelay, MAXBUFLEN, propDelay);
        gcvt(totalDelay, MAXBUFLEN, totDelay);

        //Concatenate the string to send it to the mainServer
        char sentArray[MAXBUFLEN] = "";
        strcat(sentArray,transDelay);
        strcat(sentArray, "-");
        strcat(sentArray,propDelay);
        strcat(sentArray, "-");
        strcat(sentArray,totDelay);
        
        //Send the array
        if ((numbytes = sendto(sockfdTwo, sentArray, strlen(sentArray), 0,
                    pTwo->ai_addr, pTwo->ai_addrlen)) == -1) {
                perror("calcServer: sendto");
                exit(1);
        }

        //Free memory after sending
        freeaddrinfo(servinfoTwo);

        //Displaying message upon finishing calculation and sending to main server
        printf("Send transmission delay %.2fms, propagation delay %.2fms, total delay %.2fms.\n",
        		transmissionDelay, propagationDelay, totalDelay);

    }

    close(sockfd);
    return 0;
}