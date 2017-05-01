/***********************************************************************
Open a file for read
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


int main()
{
    char file[] = "sampleFile.txt";
    int file_descriptor;  // file_descriptor is a handle or "reference" for the given file we are working with

    // use the open function and pass in the file path along with the permissions (or mode) that we want to open
    // the particular file in
    file_descriptor = open(file, O_WRONLY);  // IMPORTANT: Open file for WRITE ONLY! (for this example)

    if(file_descriptor < 0)
    {
        fprintf(stderr, "Could not open %s\n", file);
        perror("Error in main()");
        exit(1);
    }

    close(file_descriptor);

    return 0;
}

