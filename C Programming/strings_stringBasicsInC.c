/***********************************************************************
Strings in C

printf() - this is the standard way to send data to standard out (prints to standard out)
sprintf() - prints to a string (a character array)
fprintf() - prints to a file, send formatted string to a file

3 different methods to declare the same string:
    char* mystring = "my string";  --> this is a string literal
    char mystring[] = "my string";  --> creates space for 10 bytes (including the null terminator at end) on the stack as an automatic variable, puts "my string" into the variable mystring with a null terminator after it. The variable mystring is editable as it is an array.
    char mystring[20] = "my string";  --> explicitly say how many bytes to be asked for, same as above otherwise
************************************************************************/

#include <stdio.h>
#include <string.h>


int main()
{
    // example of a string literal - method 1:
    char* mystring = "my string";
    printf("Variable is: %s\n", mystring);  // prints: Variable is: my string
    // CANT DO THIS --> mystring[3] = 'M';
    // Cant modify mystring because it is a string literal!

    // example of editable array - method 2:
    char mystring2[] = "my string";
    printf("Variable is: %s\n", mystring2); // prints: Variable is: my string
    mystring2[3] = 'P';
    printf("Variable is: %s\n", mystring2);  // prints: Variable is: my Ptring



    // example of fully initializing C string array:
    char mystring3[20];
    strcpy(mystring3, "my string"); // visual --> my_string\0BOGUSBOGUS - After the null terminator after "my string" we don't know whats left in the array (the remaining chars in the array)
    printf("%s", mystring3); // prints: my string - the printf() function knows to only print up until the null terminator so the remaining "garbage" in the array is not displayed

    // PROPER WAY TO INITIALIZE STRING - fill with null terminators \0
    char mystring4[20];
    memset(mystring4, '\0', 20); // write 20 bytes of null terminators into the string
    strcpy(mystring4, "my string");
    printf("Variable is: %s\n", mystring4); // prints: Variable is: my string
    mystring4[3] = 'P';
    printf("Variable is: %s\n", mystring4); // prints: Variable is: my Ptring

    // MEMSET arrays before you use them to clear them out!


    // Example of an array of strings (pointers)...
    // Array of pointers, each of which points to a string, each of these pointers can be pointed at
    // either array names or string literals
    char* myStringArray[3]; // declare an array of pointers of size 3
    char myArray[10]; // declare an additional array of size 10
    strcpy(myArray, "1 Array"); // string copy a string into myArray

    myStringArray[0] = "string literal"; // set the first pointer in myStringArray to the address of a string literal
    printf("myStringArray[0]: %s\n", myStringArray[0]); // prints: myStringArray: string literal
    myStringArray[0] = myArray; // set the first pointer in myStringArray to point to the name of a C string array
    printf("myStringArray[0]: %s\n", myStringArray[0]); // prints: myStringArray: 1 Array


    return 0;
}

