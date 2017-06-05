/*******************************************************************************
Name: Peter Moldenhauer
Date: 6/3/2017
Description: This program (keygen.c) creates a key file of specified length.
*******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[]) {
   
   int i;
   int randNum;
   int randChar;

   // seed rand number generator
   srand(time(NULL));

   // if too few arguments, print to stderr and exit
   if (argc < 2) {
	fprintf(stderr, "Error: no key length spefified!\n");
        exit(1);
   }

   // if too many arguments, print to stderr and exit
   if (argc > 2) {
   	fprintf(stderr, "Error: too many arguments provided!\n");
	exit(1);
   }

   // get key length from args
   int keyLen = atoi(argv[1]);

   // create the key
   for (i = 0; i < keyLen; i++) {
	randNum = rand() % 27;

	if(randNum < 26) {
	   // determine letter - capital letters only!
	   randChar = 65 + randNum;
	   printf("%c", randChar);
	}
	else {
	   // print space if randNum is 26
	   printf(" ");
	}
   }

   // The last character keygen outputs is a newline
   printf("\n");

   return 0;
}
