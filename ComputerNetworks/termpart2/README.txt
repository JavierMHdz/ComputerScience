a) Javier Jesus Macossay Hernandez
b) 9907093913
c) In this assignment, I have created a transport layer UDP communication system between three servers. The main server, database server, and calculation server.
d) Makefile: It compiles and runs the *.c files.
mainServer: It works as the user interface. It receives the input and it sends the ID to the database server to retrieve the information of that specific ID file. If the ID of the file is found, then it sends the information to the calcServer to get the delays of sending the file.
dbServer: It works as the server that retrieves the data from database.txt file to identify if there is an ID in the text file that matches the ID sent by the mainServer. If the ID is found, then the corresponding information is sent to the mainServer. If the ID is not found in the database.txt file, then it sends "no match found.".
calcServer: This file is meant for receiving information of the mainServer and performing the calculations for transmission delay, propagation delay, and total delay.
e) The format of all the messages exchanged is an array of chars (char* or char[]). Then, in their respective *.c file, I convert it to an integer or to a double.
g) The project might fail if the size of the file is way to big, then its value might not be able to be stored in the double variables. Also, I have seen from my own tests that whenever I use an extremely large number as the size of the file, then I see that the transmission delay and total delay turn out to be negative.
h) I used as a starting point the listener.c and talker.c from Beej's Guide to Network Programming. Of course, after using it as a reference, I created my own code.