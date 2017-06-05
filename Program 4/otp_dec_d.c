/*********************************************************************************
Name: Peter Moldenhauer
Date: 6/3/2017
Description: This program (otp_dec_d.c) performs just like otp_enc_d.c but in 
	this case, this program will decrypt ciphertext it is given, using the 
	passed-in ciphertext and key. As a result, it returns plaintext to otp_dec.c 
*********************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<netinet/in.h>

int main(int argc, char ** argv) {
   int i;
   int listeningPort;
   int socketfd;
   int client_socket;
   int status;

   // check usage - if user did not supply a port number, print error and exit 
   if (argc < 2) {
	fprintf(stderr, "ERROR, no port number supplied\n");
	exit(1);
   }
   else {
   	// get the port number that the user entered
   	listeningPort = atoi(argv[1]);
   }

   
   // set up the socket 
   if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // create socket with socket() 
	// if something went wrong with socket() 
	fprintf(stderr, "ERROR, socket creation error\n");
	exit(1);
   }

   // set up address struct for this process (the server) 
   struct sockaddr_in server;
   memset((char *)&server, 0, sizeof(server)); // clear out the address struct 
   server.sin_family = AF_INET; // create a network-capable socket 
   server.sin_port = htons(listeningPort); // store the port number 
   server.sin_addr.s_addr = INADDR_ANY; // any address is allowed for connection to this process

   // once socket is created, bind socket to port with bind() 
   if(bind(socketfd, (struct sockaddr *) &server, sizeof(server)) == -1) { // connect socket to port 
	// if something went wrong with bind() print error and exit 
	fprintf(stderr, "ERROR, bind call failed\n");
	exit(1);
   }

   // listen for connections and flip the socket on - allow for up to 5 connections 
   if(listen(socketfd, 5) == -1) { 
	// if something went wrong with listen() print error and exit 
	fprintf(stderr, "ERROR, listen call failed\n");
	exit(1);
   }

   // use infinte loop to continually accept connections 
   while(1) { 
	// accpet connections with accept() 
	client_socket = accept(socketfd, NULL, NULL); // client_socket is where data is read from and written to 
	if (client_socket == -1) {
	    // if something went wrong with accept() print error and exit 	
	    fprintf(stderr, "ERROR, accept call failed\n");
	    exit(1);
	}
	
	// use fork() for a separate process to handle the rest of the transaction
	int pid = fork();

	if (pid == -1) { //fork error
	   fprintf(stderr, "ERROR, fork error\n");
	}
	// child process 
	else if(pid == 0) {
	   // send connection confimation num(1) to confirm otp_dec is trying to connect 
	   int toSend = htonl(0);
	   // send() here...
	   if(send(client_socket, &toSend, sizeof(toSend),
		    0) == -1){
		// if something went wrong with send() 
		fprintf(stderr, "ERROR, client send failed\n");
	   }

	   // get size of cipher text
	   int cNum;
	   if(recv(client_socket, &cNum, sizeof(cNum), 0) == -1) { //receive from client 
		// if soemthing went wrong receiving
		fprintf(stderr, "ERROR, recv cipher text size end_d -1\n");
	   }
	   else if(cNum == 0) {
		// if plain text file size == 0
		fprintf(stderr, "ERROR, recv cipher text size of 0\n");
	   }
		
	   // cLen == length of cipher text file
	   int cLen = ntohl(cNum); //convert
	   
	   // get size of key text
	   int kNum;
	   if(recv(client_socket, &kNum, sizeof(kNum), 0) == -1) { // receive from client 
		// if something went wrong receiving size
		fprintf(stderr, "ERROR, recv key text size end_d -1\n");
	   }
	   else if(kNum == 0) {
		// if size of key file == 0
		fprintf(stderr, "ERROR, recv key text size of 0\n");
	   }

	   // kLen == length of key file
	   int kLen = ntohl(kNum); //convert

	   // allocate memory for cipher text
   	   char *cipherText = malloc(sizeof(char) * cLen); 
   	   char buffer[1024];

	   // clear out cipher text
   	   memset(cipherText, '\0', cLen);

	   // receive cipher text
	   int len = 0;
	   int r;
	   while(len <= cLen) {//while the whole file has not been received 
	      memset((char *)buffer, '\0', sizeof(buffer)); //clear buffer each use
	      r = recv(client_socket, &buffer, 1024, 0); // receive from client 

	      if(r == -1) {
		// if something went wrong receiving
		fprintf(stderr, "ERROR, recv cipher text file -1\n");
		break;
	      }
	      else if (r == 0) {
		// if end of data
		if (len < cLen) { // if not enough received
		   break;
		}
	      }
	      else {
		// concat string
		strncat(cipherText,buffer,(r-1));
	      }

	      len += (r-1); // add len received to total len 
	   }

	   cipherText[cLen - 1] = '\0'; // null terminate
	
	   // allocate memory for key text
   	   char *keyText = malloc(sizeof(char) * kLen); 
   	   // clear buffer and key
   	   memset((char *)&buffer, '\0', sizeof(buffer));
	   memset(keyText, '\0', kLen);

	   // receive key text
	   len = 0;

	   while(len <= kLen) { // while whole string not received
	      // clear buffer each use
	      memset((char *) buffer, '\0', sizeof(buffer));

	      r = recv(client_socket, &buffer, 1024, 0); // receive from client 

		   if(r == -1) {
		       // if something went wrong receiving
		       fprintf(stderr, "ERROR, recv key text file dec_d\n");
			break;
		   }
		   else if (r == 0) {
		       // if end of data
			break;
		   }
		   else {
	 		// concat string
			strncat(keyText,buffer,(r-1));
		   }

		len += (r-1); // add len received to total len 
	   }

	   keyText[kLen - 1] = '\0'; // null terminate

	   int cipherNum;
	   int keyNum;
	   int decNum;
	   // decrypt the cipher text file using key
	   for (i = 0; i < cLen - 1; i++) {
		// change cipher chars to ints 0-26
		if(cipherText[i] == ' ') { // space
		  cipherNum = 26;
		}
		else { // letter
		   cipherNum = cipherText[i] - 65;
		}

		// change key chars to ints 0-26
		if(keyText[i] == ' ') { // space
		   keyNum = 26;
		}
		else {
		   keyNum = keyText[i] - 65;
		}

		// determine decrypted char
		decNum = cipherNum - keyNum;
		if (decNum < 0) { // if negative then add 27
		   decNum += 27;
		}

		//replace cipher char with decrypted char
		if(decNum == 26) { // space
		   cipherText[i] = ' ';
		}
		else { // letter
		   cipherText[i] = 'A' + (char)decNum;
		}
	   }

	   // send back decrypted file
	   len  = 0;

	   while (len <= cLen) { // while whole file is not sent
		char plainSend[1024];

		// copy subsect to send
		strncpy(plainSend, &cipherText[len], 1023);

		plainSend[1024] = '\0'; // null terminate

		// send subset
   	   	if(send(client_socket, &plainSend, 1024, 0) == -1) {
		   fprintf(stderr, "decryption text send\n");
	   	}

		len += 1023; // add sent len to total leni
	   }

	   // free memory
	   free(cipherText);
	   free(keyText);
	}  
	// parent     
	else {
	   // close client connection
	   close(client_socket);

	   // children finished?
	   do {
		waitpid(pid, &status, 0);
	   } while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
   }
	
   // close socket
   close(socketfd);
	   
   return 0;
}
