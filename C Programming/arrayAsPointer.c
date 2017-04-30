/***********************************************************************
Arrays vs. Pointers in C:
Looking at one-dimensional vs two-dimensional arrays...
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char* argv[]){

    // *** One Dimensional Arrays ***

    //An array of chars, of size 10
    char myArray[10];

    myArray[2] = 'A';
    char* myPointer = myArray;
    char A = myPointer[2];
    printf("char A: %c\n", A);  // prints: char A: A


    // *** Two Dimensional Arrays ***
    char my2DArray[10][20];
    my2DArray[2][4] = 'B';
    char specificChar = my2DArray[2][4];
    char* specificCharP = &my2DArray[2][4];
    printf("char specificChar: %c\n", specificChar);  // prints: B
    printf("char* specificCharP: %c\n", *specificCharP);  // prints: B

    char (*my2DPointer)[20] = my2DArray;
    printf("my2DPointer: %c\n", my2DPointer[2][4]);  // prints: B

    char (*noTrouble)[20] = malloc(10 * 20 * sizeof(char));
    printf("About to assign noTrouble[0][0]\n");
    noTrouble[0][0] = 'T';
    printf("About to print out noTrouble[0][0]\n");
    printf("noTrouble[0][0]: %c\n", noTrouble[0][0]);  // prints: T

    char one = 'C';
    char* two = &one;
    char** three = &two;
    printf("one deferenced twice: %c\n", **three); // prints: C


    //Note: The exec() family of functions change your program into another program
    // as specified by the path you pass in to the function.
    char* arr[10];
    int i;
    for (i = 0; i < 10; i++)
        arr[i] = malloc(20 * sizeof(char));
    strcpy(arr[0], "uptime");
    arr[1] = NULL;
    printf("About to convert to command \"uptime\"...\n");
    fflush(stdout);
    execvp(arr[0], arr);

    return 0;
}

/**************************************
Output:

$ gcc -o arrayAsPointer arrayAsPointer.c
$ arrayAsPointer

char A: A
char specificChar: B
char* specificCharP: B
my2DPointer: B
About to assign noTrouble[0][0]
About to print out noTrouble[0][0]
noTrouble[0][0]: T
one deferenced twice: C
About to convert to command "uptime"...

******************************************/
