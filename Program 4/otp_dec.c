/*******************************************************************************
Name: Peter Moldenhauer
Date: 6/3/2017
Description: This program (otp_dec.c) will connect to otp_dec_d.c and will ask 
	it to decrypt ciphertext using a passed-in ciphertext and key. 
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

   // check usage - make sure whe have all the args that we need 
   if (argc < 4) {
	fprintf(stderr, "ERROR, need to specifiy ciphertext, key, and port number\n");
	exit(1);
   }

   // get port num from args
   int portNum = atoi(argv[3]);

   // open cipher text file and key for reading
   int fdCipher = open(argv[1], O_RDONLY);
   int fdKey = open(argv[2], O_RDONLY);

   // check that there was not an error opening
   if (fdCipher == -1 || fdKey == -1) {
	// if something went wrong with open() print error and exit 
	fprintf(stderr, "ERROR, could not open files\n");
	exit(1);
   }

   // get size of cipher text
   int cLen = lseek(fdCipher, 0, SEEK_END);

   // get size of key text
   int kLen = lseek(fdKey, 0, SEEK_END);

   // make sure key file is larger than cipher text
   if (kLen < cLen) {  // compare key to plain
	fprintf(stderr, "ERROR, key too short\n");
	exit(1);
   }

   // create string to hold cipher text
   char *cipherText = malloc(sizeof(char) * cLen); 

   // set file point to begining of file
   lseek(fdCipher, 0, SEEK_SET);

   // read cipher text into string
   if (read(fdCipher, cipherText, cLen) == -1) {
	// if something went wrong with read() 
	fprintf(stderr, "ERROR, read cipher text dec\n");
	exit(1);
   }

   // null terminate the string
   cipherText[cLen - 1] = '\0';

   // check that chars in cipher text are valid
   for (i = 0; i < cLen - 1; i++) {
	if(isalpha(cipherText[i]) || isspace(cipherText[i])) {
	   // if letter or space do nothing, valid chars
	}
	else { 
	   // if not letter or space then print error and exit 
	   fprintf(stderr, "ERROR, cipher text invalid char\n");
	   exit(1);
	}
   }

   // create string to hold key text
   char *keyText = malloc(sizeof(char) * kLen); 

   // set file point to begining of file
   lseek(fdKey, 0, SEEK_SET);

   // read key text into string
   if (read(fdKey, keyText, kLen) == -1) {
	// if something went wrong reading
	fprintf(stderr, "ERROR, read key text enc\n");
	exit(1);
   }

   // null terminate the string
   keyText[kLen - 1] = '\0';
 
   // check that chars in plain text are valid
   for (i = 0; i < kLen - 1; i++) {
	if(isalpha(keyText[i]) || isspace(keyText[i])) {
	   // if letter or space do nothing, valid chars 
	}
	else { 
	   // if not lettor or space then print error and exit 
	   fprintf(stderr, "ERROR, key text invalid char\n");
	   exit(1);
	}
   }

   
   // set up the socket
   int socketfd;
   // create socket and do error checking 
   if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	// if something went wrong creating the socket 
	fprintf(stderr, "ERROR, socket error\n");
	exit(1);
   }

   // set up the server address struct 
   struct hostent * server_ip_address;
   struct sockaddr_in server;
   memset((char *)&server, 0, sizeof(server)); // clear out the address struct  
   server.sin_family = AF_INET; // creat a network-capable socket 
   server.sin_port = htons(portNum); // store the port number
   server_ip_address = gethostbyname("localhost");
   // error checking 
   if(server_ip_address == NULL) {
	fprintf(stderr, "ERROR, could not resolve host name\n");
	exit(1);
   }
   
   // copy in the address 
   memcpy(&server.sin_addr, server_ip_address->h_addr, server_ip_address->h_length);


   // connect to the server 
   if(connect(socketfd, (struct sockaddr*) &server, sizeof(server)) == -1) {
	// if something went wrong connecting
	fprintf(stderr, "ERROR connecting to server\n");
	exit(2);
   }

   // confirm connection
   int r;
   int conNum;
   // receive confirmation number
   if((r = recv(socketfd, &conNum, sizeof(conNum), 0)) == -1) {
	// if something went wrong receiving
	fprintf(stderr, "ERROR, recv enc -1\n");
	exit(1);
   } 
   else if(r == 0) {
	fprintf(stderr, "ERROR, recv enc 0\n");
	exit(1);
   }

   // check that confirmation number is correct
   int confirm = ntohl(conNum);

   // if number recieved is not correct
   if (confirm != 0) {
	fprintf(stderr, "ERROR, could not contact otp_dec_d on port %d\n", portNum);
	exit(2);
   }

   // successful connection to otp_enc_d
   // send cipher text file size
   int cLenSend = htonl(cLen); // convert
   // send here... 
   if(send(socketfd, &cLenSend, sizeof(cLenSend), 0) == -1) { 
	// if something went wrong with send() 
	fprintf(stderr, "ERROR, cipher text file send\n");
	exit(1);
   }

   //send key text file size
   int kLenSend = htonl(kLen); //convert
   // send here...
   if(send(socketfd, &kLenSend, sizeof(kLenSend), 0) == -1) {
	// if something went wrong with send() 
	fprintf(stderr, "ERROR, key text file send\n");
	exit(1);
   }

   // send cipher text
   int len = 0;
   while (len <= cLen) {
	char cipherSend[1024];

	// subset of cipher  to send
	strncpy(cipherSend, &cipherText[len], 1023);

	cipherSend[1024] = '\0'; // null terminate

	// send here 
   	if(send(socketfd, cipherSend, 1024, 0) == -1){
	   // if something went wrong with send() 
	   fprintf(stderr, "ERROR, cipher text send -1\n");
	   exit(1);
	}

	len += 1023; // add len sent to len
   }

   // send key text
   len = 0;
   while (len <= kLen) { // while whole key is not sent
	char keySend[1024]; // subset

	// subset of key to send
	strncpy(keySend, &keyText[len], 1023);

	// null terminate
	keySend[1024] = '\0';

	// send here
   	if(send(socketfd, &keySend, 1024, 0) == -1){
	   // if something went wrong with send() 
	   fprintf(stderr, "ERROR, key text send -1\n");
	   exit(1);
	}

	len += 1023; // add len sent to len
   }

   // receive back decrypted text
   // allocate memory for plain text
   char *plainText = malloc(sizeof(char) * cLen);
   char buffer[1024];

   // clear plaintext 
   memset(plainText, '\0', cLen);

   // receive plain text
   len = 0;
   r = 0;
   while(len < cLen) { //while the whole file has not been received
 	//clear buffer each use
 	memset((char *)buffer, '\0', sizeof(buffer));

 	r = recv(socketfd, &buffer, 1024, 0); // receive 

	   if(r == -1) {
		// if something went wrong receiving
		fprintf(stderr, "ERROR, recv plain text file dec\n");
		exit(1);
	   }	   
	   else if(r == 0) {
		// end of data
		if (len < cLen) { 
		   // if not enough received 
		   fprintf(stderr, "ERROR, recv plain text file <\n");
		   exit(1);
		}
	   }
	   else {
		// concat string
		strncat(plainText,buffer,(r-1));
	   }	   
	
	len += (r-1); // add len received to len
   }

   plainText[cLen - 1] = '\0'; // null terminate

   // print plain text
   printf("%s\n", plainText);

   // free memory
   free(plainText);
   free(keyText);
   free(cipherText);

   return 0;
}
