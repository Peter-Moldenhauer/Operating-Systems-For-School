/***********************************************************************
Complete read/write example
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>


int main()
{
    int file_descriptor;
    char* newFilePath = "./newFile.txt";
    char* giveEm = "Hello World\n";

    ssize_t nread, nwritten; // this is a special type in C in UNIX that tracks the number of bytes read and written

    char readBuffer[32];

    file_descriptor = open(newFilePath, O_RDWR | O_CREAT | O_TRUNC, 0600); // remember, if you use the
                                                                                        // O_CREAT (create) flag, we
                                                                                        // need to include the
                                                                                        // permission bits (read/write)
                                                                                        // to user or others etc.

    if(file_descriptor == -1)
    {
        printf("Hull breach - open() failed on \"%s\"\n", newFilePath);
        perror("In main()");
        exit(1);
    }

    nwritten = write(file_descriptor, giveEm, strlen(giveEm) * sizeof(char));

    memset(readBuffer, '\0', sizeof(readBuffer)); // clear out the array before using it - Set everything to null terminators
    lseek(file_descriptor, 0, SEEK_SET); // reset the file pointer to the beginning of the file
    nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

    printf("File contents: \n%s", readBuffer);

    return 0;
}

