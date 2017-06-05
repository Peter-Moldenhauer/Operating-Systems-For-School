/*******************************************************************************
Name: Peter Moldenhauer
Date: 6/3/2017
Description: This program (otp_enc_d.c) is part of program 4 and runs in the 
	background as a daemon. This program listens on a particular port and when 
	a connection is made, it calls accept() to generate the socket for 
	communication. This program then peforms the actual encoding through a 
	separate child process.

	This is the "server" that pairs with otp_enc.c (the "client"). 
*******************************************************************************/

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

   // Check usage & args
   // Print error to stderr if no port was given and then exit
   if (argc < 2) {
	fprintf(stderr, "USAGE: %s port\n", argv[0]);
	exit(1);
   }
   // Get the port to listen on from args
   else {
   	listeningPort = atoi(argv[1]);
   }

   // Set up the socket 
   // create the socket by calling socket() - the return result will be set to socketfd
   if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	// If something went wrong, print error message to stderr
	fprintf(stderr, "ERROR opening socket\n");
	exit(1);
   }

   // Set up the address struct for this process (the server) 
   struct sockaddr_in server;
   memset((char *)&server, '\0', sizeof(server)); // clear out the address struct
   server.sin_family = AF_INET; // create a network-capable socket
   server.sin_port = htons(listeningPort); // store the port number
   server.sin_addr.s_addr = INADDR_ANY; // any address is allowed for connection to this process

   // once we have a socket and a server address variable created, then we can call bind()
   // bind the port to the socket to enable use later...
   if(bind(socketfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
	// if something went wrong with bind()
	fprintf(stderr, "ERROR, bind call failed\n");
	exit(1);
   }

   // listen for connections - turn socket on to allow connections to be made
   if(listen(socketfd, 5) == -1) { // allow up to 5 connections
	// if something went wrong with listen()
	fprintf(stderr, "ERROR, listen call failed\n");
	exit(1);
   }

   // infinte loop to continually accept connections
   while(1) { 
	// accpet connection
	// client_socket is where data is read from and written to
	client_socket = accept(socketfd, NULL, NULL);
	// if something went wrong with accept() 
	if (client_socket == -1) {
	    fprintf(stderr, "ERROR, accept call failed\n");
	    exit(1);
	}
	
	// once connection is made, use a separate process to 
	// handle the rest of the transaction - use fork() 
	int pid = fork();

	// if something went wrong with fork() 
	if (pid == -1) {
	   fprintf(stderr, "ERROR, fork error\n");
	}
	// child process 
	else if(pid == 0) { 
	   // child process must first check to make sure it is communicating with otp_enc,
	   // send connection confimation num(1) to confirm otp_enc is trying to connect
	   int toSend = htonl(1);

	   if(send(client_socket, &toSend, sizeof(toSend),
		    0) == -1){
		// if confirmation number failed to send
		fprintf(stderr, "ERROR, client send failed\n");
		exit(1);
	   }

	   // get size of plain text
	   int pNum;
	   // receive here
	   if(recv(client_socket, &pNum, sizeof(pNum), 0) == -1) {
		// if something went wrong receiving
		fprintf(stderr, "ERROR, recv plain text size end_d -1\n");
	   }
	   else if(pNum == 0) {
		// if plain text file size == 0
		fprintf(stderr, "ERROR, recv plain text size of 0\n");
	   }
		
	   // pLen == length of plain text file
	   int pLen = ntohl(pNum); // convert, network to host
	   
	   // get the size of key text
	   int kNum;
	   // receive here
	   if(recv(client_socket, &kNum, sizeof(kNum), 0) == -1) {
		// if something went wrong receiving size
		fprintf(stderr, "ERROR, recv key text size end_d -1\n");
	   }
	   else if(kNum == 0) {
		// if size of key file == 0
		fprintf(stderr, "ERROR, recv key text size of 0\n");
	   }

	   // kLen == length of key file
	   int kLen = ntohl(kNum); // convert, network to host

	   // Allocate memory for plain text
   	   char *plainText = malloc(sizeof(char) * pLen); 
   	   char buffer[1024];

	   // Clear plain text
   	   memset(plainText, '\0', pLen);

	   // Receive plain text
	   int len = 0;
	   int r;
	   while(len <= pLen) {//while the whole file has not been received
	      memset((char *)buffer, '\0', sizeof(buffer)); //clear buffer each use

	      r = recv(client_socket, &buffer, 1024, 0); // receive

 	      if(r == -1) {
		 // if something went wrong receiving data
		 fprintf(stderr, "ERROR, recv plain text file -1\n");
		 break;
	      }
	      else if (r == 0) {
		 // end of data
		 if (len < pLen) { // If not enough received
		   break;
		 }
	      }
	      else {
	 	// concat string
		strncat(plainText,buffer,(r - 1));
	      } 

	      len += (r-1); // add len received to total len received
	   }

	   plainText[pLen - 1] = '\0'; // null terminate

	   // Allocate memory for key text
   	   char *keyText = malloc(sizeof(char) * kLen); 
   	   // clear buffer and key
   	   memset((char *)buffer, '\0', sizeof(buffer));
	   memset(keyText, '\0', kLen);

	   // Receive key text
	   len = 0;

	   while(len <= kLen) {// while whole string not received
	      // clear buffer each use
   	      memset((char *)buffer, '\0', sizeof(buffer)); 

	      r = recv(client_socket, &buffer, 1024, 0);//receive

	   	if(r == -1) {
		   // if something went wrong receiving data
		   fprintf(stderr, "ERROR, recv key text file -1\n");
	  	   break;
	   	}
	   	else if (r == 0) {
		       //end of data
		       break;
		   }
		   else {
	 		// Concat string
			strncat(keyText,buffer,(r - 1));
		   }
	       
	      len += (r - 1); // add len recived to total len received
	   }

	   keyText[kLen - 1] = '\0';

	   int plainNum;
	   int keyNum;
	   int enNum;
	   // Encrypt the plain text file using key
	   for (i = 0; i < pLen - 1; i++) {
		// change plain chars to ints 0-26
		if(plainText[i] == ' ') { // space
		  plainNum = 26;
		}
		else { // uppercase letter
		   plainNum = plainText[i] - 65;
		}

		// change key chars to ints 0-26
		if(keyText[i] == ' ') { // space
		   keyNum = 26;
		}
		else {
		   keyNum = keyText[i] - 65;
		}

		// Determine encrypted char
		enNum = plainNum + keyNum;
		if (enNum >= 27) {// if >= 27 subtract 27
		   enNum -= 27;
		}

		// replace plain char with encrypted char
		if(enNum == 26) { // space
		   plainText[i] = ' ';
		}
		else { // uppercase letter
		   plainText[i] = 'A' + (char)enNum;
		}
	   }

	   // send back encrypted file
	   len = 0;
	   while (len <= pLen) { // while whole file is not sent
		char cipherSend[1024];
	
		// copy subsection of string to send	
		strncpy(cipherSend, &plainText[len], 1023);

		cipherSend[1024] = '\0'; // null terminate!

   		if(send(client_socket, &cipherSend, 1024, 0) == -1) {
		   // if something went wrong with send()  
		   fprintf(stderr, "ERROR, encryption text send\n");
		}

		len += 1023; // add sent  lenght to len
	   }

	   // free memory
	   free(plainText);
	   free(keyText);
	}
	// parent       
	else {
	   //close the existing socket which is connected to the client 
	   close(client_socket);

	   // children finished?
	   do {
		waitpid(pid, &status, 0);
	   } while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
   } // terminate while loop 
	
   // close the listening socket
   close(socketfd);

   return 0;
}
