/***********************************************************************
Strings in C - Dynamically allocating a string
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int main()
{
    char* mystring;
    char* literal = "string literal"; // string literal - where this address points cannot be modified

    mystring = malloc(20 * sizeof(char));
    assert(mystring != 0);

    memset(mystring, '\0', 20);

    sprintf(mystring, "Yay! %s\n", literal);
    printf("%s", mystring); // prints: Yay! literal
    mystring[3] = 'P';
    printf("%s", mystring); // prints: YayP literal

    free(mystring);

    return 0;
}

