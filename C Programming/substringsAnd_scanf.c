/***********************************************************************
Substrings
************************************************************************/

#include <stdio.h>

int main()
{

    char string[1000], sub[1000];
    int position, length, c = 0;

    printf("Input a string\n");
    gets(string);

    printf("Enter the position of first char, a space, and length of substring\n");
    scanf("%d%d", &position, &length);  // scanf() will take user input and separate the user input data and put into
                                        // both variables depending on where the space is that the user enters

                                        // Note: scanf() is another option to get user input besides gets()

    while(c < length) {
        sub[c] = string[position + c - 1];
        c++;
    }
    sub[c] = '\0'; // put a null terminator at the end of the "string" to actually make it a string

    printf("Required substring is \"%s\"\n", sub);

    return 0;
}

