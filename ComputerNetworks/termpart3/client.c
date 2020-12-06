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
#include <stdbool.h>

//I think I don't need this
#define PORT "21913" //the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
    	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//reading input from the user to know the name of the client
int main(int argc, char *argv[]){
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    //check if the user entered the right amount of inputs
    if (argc != 3) {
        fprintf(stderr,"usage: client <ID> <size>\n");
        exit(1);
	}

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    //localhost IP address is hardcoded in this program
    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
			continue;
		}

		break;
	}

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
	}
	printf("The Client is up and running.\n");

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
    		s, sizeof s);

    freeaddrinfo(servinfo); // all done with this structure

    //I added the following code.
    
    //Concatenate the string to send it to the mainServer
    char sentArray[MAXDATASIZE] = "";
    strcat(sentArray,argv[1]);
    strcat(sentArray,"-");
    strcat(sentArray,argv[2]);

    if(send(sockfd, sentArray, MAXDATASIZE-1, 0) == -1){ //created the size of message with MAXDATASIZE-1
    	perror("send"); //if the message was not send, create an error message
    }
    else{
    	//if the message was sent succesfully, then print this message
    	printf("Send Link %s and file size %s MB to main server.\n",argv[1],argv[2]);
    }

    //this is the receiver part of the client
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
		exit(1);
	}

	buf[numbytes] = '\0';

    if(buf[0] == 'n' && buf[1] == 'o'){
        //If received "No match found." continue to next iteration
        printf("Receive %s\n", buf);
	}
    else{
        //Parse input from calcServer
        char transDelay[MAXDATASIZE] = "";
        char propDelay[MAXDATASIZE] = "";
        char totDelay[MAXDATASIZE] = "";
        char* token = strtok(buf, "-");
        strcat(transDelay,token);
        token = strtok(NULL, "-");
        strcat(propDelay, token);
        token = strtok(NULL, "-");
        strcat(totDelay, token);

        //print message out to know who I got the reply from, in this case the name of the mainServer
        printf("Receive transmission delay %.2fms, propagation delay %.2fms, and total delay %.2fms.\n", atof(transDelay), atof(propDelay), atof(totDelay));
    }

	close(sockfd);

	return 0;

}