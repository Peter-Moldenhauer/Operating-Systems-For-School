/***********************************************************************
User input strings
************************************************************************/

#include <stdio.h>
#include <string.h>  // this library allows for strlen() and strcmp()

int main()
{

    char a[1000], b[1000];  // define 2 arrays

    printf("Enter the first string\n");
    gets(a);

    printf("Enter the second string\n");
    gets(b);

    strcat(a, b);  // Note: strcat() dumps the results into a, overwriting what was previously in there

    printf("String obtained on concatenation is %s\n", a);

    return 0;
}

