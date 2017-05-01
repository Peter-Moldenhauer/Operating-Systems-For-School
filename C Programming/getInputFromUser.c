/***********************************************************************
Get input from the user
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main()
{
    int numCharsEntered = -5; // how many chars we entered
    int currChar = -5; // tracks where we are when we print out every char
    size_t bufferSize = 0; // holds how large the allocated buffer is
    char* lineEntered = NULL;  // points to a buffer allocated by getline() that holds our entered string + \n + \0

    while(1)
    {
        // get input from the user
        printf("Enter in a line of text (CTRL-C to exit): ");
        numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // get a line from the user
        printf("Allocated %zu bytes for the %d chars you entered.\n", bufferSize, numCharsEntered);
        printf("Here is the raw entered line: \"%s\"\n", lineEntered);

        // Print out the actual contents of the string that was entered
        printf("Here are the contents of the entire buffer:\n");
        printf("  # CHAR INT\n");
        for(currChar = 0; currChar < bufferSize; currChar++) // display every character in both dec and ASCII
        {
            printf("%3d `%c\' %3d\n", currChar, lineEntered[currChar], lineEntered[currChar]);
        }
        free(lineEntered); // free the memory allocated by getline() or else memory leak

        lineEntered = NULL; // make getline() automatically allocate space on the NEXT iteration through the loop
    }

    return 0;
}

