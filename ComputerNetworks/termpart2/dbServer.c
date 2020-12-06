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

//Ports used in this file
#define MAINPORT "22913"
#define DBPORT "23913"

#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    //I added this part
    //Creating a struct of addrinfo type
    struct addrinfo *servinfoTwo, *pTwo;
    int sockfdTwo;

    if ((rv = getaddrinfo("127.0.0.1", MAINPORT, &hints, &servinfoTwo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(pTwo = servinfoTwo; pTwo != NULL; pTwo = pTwo->ai_next) {
        if ((sockfdTwo = socket(pTwo->ai_family, pTwo->ai_socktype,
                pTwo->ai_protocol)) == -1) {
            perror("dbServer: destination socket");
            continue;
        }
        break;
    }

    if (pTwo == NULL) {
        fprintf(stderr, "dbServer: failed to create destination socket\n");
        return 2;
    }

    //I added the previous part

    if ((rv = getaddrinfo("127.0.0.1", DBPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("dbServer: source socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("dbServer: source bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "dbServer: failed to bind source socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
    //Print a message to the user letting the user know that the dbServer is working
    printf("The Database Server is up and running.\n");
    addr_len = sizeof their_addr;

    //Try using the while loop here
    while(1){
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
            (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        //Print message to user that the request was received from Main Server
        printf("Receive request from Main Server.\n");
        buf[numbytes] = '\0';

        //create variables used in this file to later send information to mainServer
        char* idInput = buf;
        char id[MAXBUFLEN] = "";
        char capacity[MAXBUFLEN] = "";
        char length[MAXBUFLEN] = "";
        char velocity[MAXBUFLEN] = "";

        //To keep track if the ID has been found
        bool found = false;

        //read only
        FILE* in_file = fopen("database.txt","r");

        if(! in_file){  //equivalent of saying if (in_file == NULL)
            printf("Error! Could not open file\n");
            exit(1);
        }

        //attempt to read the next line and store
        //the value in the "number" variable
        while(fscanf(in_file,"%s",id) == 1){

            if(strcmp(idInput,id) == 0){
                //Saving the database.txt information to the char arrays
                fscanf(in_file,"%s",capacity);
                fscanf(in_file,"%s",length);
                fscanf(in_file,"%s",velocity);
                found = true;
                //Break out of the loop if the ID was found
                break;
            }
            else{
                //Skip the variables and look at the next ID because it doesn't match
                fscanf(in_file,"%s",capacity);
                fscanf(in_file,"%s",length);
                fscanf(in_file,"%s",velocity);
            }
        }

        //If the id was found, enter this bracket
        if(found){

            //Create char array that will be sent to the mainServer
            char sentArray[MAXBUFLEN] = "";
            strcat(sentArray, id);
            strcat(sentArray, "-");
            strcat(sentArray, capacity);
            strcat(sentArray, "-");
            strcat(sentArray, length);
            strcat(sentArray, "-");
            strcat(sentArray, velocity);

            //Sending id
            if ((numbytes = sendto(sockfdTwo, sentArray, strlen(sentArray), 0,
                    pTwo->ai_addr, pTwo->ai_addrlen)) == -1) {
                perror("dbServer: sendto");
                exit(1);
            }

            //Upon finishing searching and sending to main server.
            printf("Send link %s, capacity %sMbps, link length %skm, propagation velocity %skm/s.\n", id, capacity, length, velocity);

        }
        //Otherwise, the ID is not in the database.txt
        else{
           
            //send the "no match found" message
            char* message = "no match found.";

            //Send "no match found." if the ID is not in the database
            if ((numbytes = sendto(sockfdTwo, message, strlen(message), 0,
                    pTwo->ai_addr, pTwo->ai_addrlen)) == -1) {
                perror("dbServer: sendto");
                exit(1);
            }

            //Print out the result to the user
            printf("No match found.\n");
        }

    }

    close(sockfd);

    return 0;
}