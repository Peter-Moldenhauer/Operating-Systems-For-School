// This program demonstrates using pointers to functions

#include <stdio.h>

int addOne(int inputArg);  // function prototype - returns an int

int main()
{
    int (*fpArg)(int) = addOne; // declare a function pointer variable named fpArg, and sets it equal to the same
                                // address as where addOne() is declared
                                // Note: the (int) means the function pointer will take an int as an argument

    printf("10 + 1 = %d\n", fpArg(10));

    return 0;
}

int addOne(int input){
    return input + 1;
}
