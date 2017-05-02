/***********************************************************************
Create a Directory
************************************************************************/

#include <stdio.h>
#include <sys/stat.h>


int main()
{
    // use the mkdir() system call and include the set of permissions you want the new directory to have
    // If mkdir() returns 0 then the directory creation was successful

    int result = mkdir("newdir", 0755);
    printf("Result of mkdir(): %d\n", result);

    return 0;
}

