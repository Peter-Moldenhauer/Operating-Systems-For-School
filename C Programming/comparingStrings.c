/***********************************************************************
Comparing Strings
************************************************************************/

#include <stdio.h>
#include <string.h>  // this library allows for strlen() and strcmp()

int main()
{

    char* boring = "boring"; // This is a string literal - it cannot be changed
    char* weirdSaying = "Eat more beef, kick less cats\n";
    int length;

    length = strlen(weirdSaying);
    printf("Length of entered string is = %d\n", length); // Note the %d means decimal value (for the int)

    if(strcmp(boring, weirdSaying) == 0) // the strcmp() function returns 0 when the two strings are exactly equivalent (so 0 == 0 )
        printf("Entered strings are equal.\n");
    else
        printf("Entered strings are not equal.\n");

    return 0;
}

/* Output:

Length of entered string is = 30
Entered strings are not equal

*/
