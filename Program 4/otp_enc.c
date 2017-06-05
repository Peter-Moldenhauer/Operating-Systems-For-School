/*******************************************************************************
Name: Peter Moldenhauer
Date: 6/3/2017
Description: This program (otp_enc.c) connects to otp_enc_d and asks it to 
	perform a one-time "pad style" encrytion.

	otp_enc.c is the "client" and otp_enc_d.c is the "server" 
*******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<netdb.h>

int main(int argc, char **argv) {
   int i;

   // make sure we have all of the arguments that we need
   if (argc < 4) {
	fprintf(stderr, "Error, must specifiy plaintext, key, and port number\n");
	exit(1);
   }

   // get the port number from args,
   // this is the port that otp_enc should attempt to connect to otp_enc_d on
   int portNum = atoi(argv[3]); // use atoi to convert from string given by user

   // open plain text file and key for reading
   int fdPlain = open(argv[1], O_RDONLY); // open plaintext file that conatins the plaintext we want to encrypt
   int fdKey = open(argv[2], O_RDONLY); // open key that contains the encryption key we want to use to encrypt the text

   // check that there was not an error opening
   if (fdPlain == -1 || fdKey == -1) {
	fprintf(stderr, "ERROR, could not open files\n");
	exit(1);
   }

   // get size of plain text
   int pLen = lseek(fdPlain, 0, SEEK_END);

   // get size of key text
   int kLen = lseek(fdKey, 0, SEEK_END);

   // verify that the key file is larger than plain text
   // need to compare kLen to pLen
   if (kLen < pLen) { 
	fprintf(stderr, "ERROR, key too short\n");
	exit(1);
   }

   // create string to hold plain text
   char *plainText = malloc(sizeof(char) * pLen); 

   // set file point to begining of file
   lseek(fdPlain, 0, SEEK_SET);

   // read plain text into string
   if (read(fdPlain, plainText, pLen) == -1) { //read
	// if something went wrong reading
	fprintf(stderr, "ERROR, read plain text enc\n");
	exit(1);
   }

   // null terminate the string
   plainText[pLen - 1] = '\0';

   // check that chars in plain text are valid
   for (i = 0; i < pLen - 1; i++) {
	if(isalpha(plainText[i]) || isspace(plainText[i])) {
	   // if letter or space do nothing - valid chars
	}
	// otherwise, if not a letter or space (not valid chars) then print error and exit
	else { 
	   fprintf(stderr, "ERROR, plain text invalid char\n");
	   exit(1);
	}
   }

   // create string to hold key text
   char *keyText = malloc(sizeof(char) * kLen); 

   // set file point to begining of file
   lseek(fdKey, 0, SEEK_SET);

   // read key text into string
   if (read(fdKey, keyText, kLen) == -1) { //read
	// if something went wrong reading
	fprintf(stderr, "ERROR, read key text enc\n");
	exit(1);
   }

   // null terminate the string
   keyText[kLen - 1] = '\0';
 
   // check that chars in plain text are valid
   for (i = 0; i < kLen - 1; i++) {
	if(isalpha(keyText[i]) || isspace(keyText[i])) {
	   // if letter or space do nothing - valid chars
	}
	// otherwise if not letter or space (not valid chars) then print error and exit
	else { 
	   fprintf(stderr, "ERROR, key text invalid char\n");
	   exit(1);
	}
   }



   // create and set up the socket 
   int socketfd;
   if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // create the socket
	// if something went wrong creating
	fprintf(stderr, "ERROR, socket error\n");
	exit(1);
   }

   // set up the server address struct 
   struct hostent * server_ip_address;
   struct sockaddr_in server;
   memset((char *)&server, 0, sizeof(server)); // clear out the address struct
   server.sin_family = AF_INET; // create a network-capable socket 
   server.sin_port = htons(portNum); // store the port number
   server_ip_address = gethostbyname("localhost");  
   if(server_ip_address == NULL) { // error checking
	fprintf(stderr, "ERROR, could not resolve host name\n");
	exit(1);
   }
   // load up address into server address struct 
   memcpy(&server.sin_addr, server_ip_address->h_addr, server_ip_address->h_length);


   // connect to the server with connect() 
   if(connect(socketfd, (struct sockaddr*) &server, sizeof(server)) == -1) {
	// if something went wrong with connect() print error and exit 
	fprintf(stderr, "ERROR connecting\n");
	exit(2);
   }

   // confirm connection
   int r;
   int conNum;
   // receive confirmation number
   if((r = recv(socketfd, &conNum, sizeof(conNum), 0)) == -1) {
	// if something went wrong receiving, print error and exit 
	fprintf(stderr, "ERROR, recv enc\n");
	exit(1);
   } 
   else if(r == 0) {
	fprintf(stderr, "ERROR, recv enc 0\n");
	exit(1);
   }

   //Check that confirmation number is correct
   int confirm = ntohl(conNum);

   //If number recieved is not correct
   if (confirm != 1) {
	fprintf(stderr, "ERROR, could not contact otp_enc_d on port %d\n", portNum);
	exit(2);
   }

   // connection to otp_enc_d successful 
   // send plain text file size
   int pLenSend = htonl(pLen); // convert

   if(send(socketfd, &pLenSend, sizeof(pLenSend), 0) == -1) {
	// if something went wrong with send()
	fprintf(stderr, "ERROR, plain text file send\n");
	exit(1);
   }

   // send key text file size
   int kLenSend = htonl(kLen); // convert

   if(send(socketfd, &kLenSend, sizeof(kLenSend), 0) == -1) {
	// if something went wrong with send() 
	fprintf(stderr, "ERROR, key text file send\n");
	exit(1);
   }

   // send plain text
   int len = 0;
   while (len <= pLen) { // while whole file not sent
	char plainSend[1024]; // hold text to send

	// subset of plain to send
	strncpy(plainSend, &plainText[len], 1023);

	plainSend[1024] = '\0'; //null terminate
	
	// send 
	if(send(socketfd, &plainSend, 1024, 0) == -1){
	   // if something went wrong with send() 
	   fprintf(stderr, "ERROR, plain text send\n");
	   exit(1);
	}

	len += 1023; //Add length sent to len
   }

   // send key text
   len = 0;
   while (len <= kLen) { // while whole key is not sent
	char keySend[1024]; // subset

	// subset of key to send
	strncpy(keySend, &keyText[len], 1023);

	// null terminate
	keySend[1024] = '\0';

	// send
   	if(send(socketfd, &keySend, 1024, 0) == -1){
		// if something went wrong with send() 
		fprintf(stderr, "ERROR, key text send\n");
		exit(1);
   	}

	len += 1023; //add len sent to len
   }

   // receive back encrypted text
   // allocate memory for cipher text
   char *cipherText = malloc(sizeof(char) * pLen);
   char buffer[1042];

   //Clear ciphertext 
   memset(cipherText, '\0', pLen);

   // receive cipher
   len = 0;
   r = 0;
   while(len < pLen) { //while the whole file has not been received 

	memset((char *)buffer, '\0', sizeof(buffer)); // clear buffer each use 

	// receive back from the server 
 	r = recv(socketfd, buffer, 1024, 0); // read data from the socket

        if(r == -1) {
	   // if something went wrong receiving data
	   fprintf(stderr, "ERROR, recv cipher text file -1\n");
	   exit(1);
	}	   
	else if(r == 0) {
	   // if end of data
	   if(len < pLen) {
		fprintf(stderr, "ERROR, recv cipher text file 0\n");
		exit(1);
	   }
	}
	else {
	   // concat string
	   strncat(cipherText,buffer,(r-1));
	}	   

	len += (r-1); // add len recieved to len
   }

   cipherText[pLen - 1] = '\0';

   // print cipher text
   printf("%s\n", cipherText);

   // free up the mem
   free(plainText);
   free(keyText);
   free(cipherText);

   return 0;
}
