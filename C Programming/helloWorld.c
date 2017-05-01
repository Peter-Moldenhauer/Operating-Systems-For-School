/***********************************************************************
Hello World Program
************************************************************************/

#include <stdio.h>  // preprocessor directive - the preprocessor runs before the compiler, allows for include statements
                    // The standard io library is included for the printf() function

int main()
{

    char* oblig = "Hello World!";  // pointer to a character
    float itsOver = 9000.0f;  // floating point #'s have an f at the end after the decimal to tell that they are floating point numbers

    printf("%s\n", oblig);  // Note the %s is a field that uses a string to sub in - oblig is a string, a char* is a string!
    printf("IT\'S OVER %.2f!\n", itsOver);

    return 0;
}

/* Output:

Hello World!
IT'S OVER 9000.00!

*/
