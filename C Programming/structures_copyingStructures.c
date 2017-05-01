/***********************************************************************
Structures - copying structures
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct room {
    int id;
    char* name;
};


int main()
{
    struct room startRoom;
    startRoom.id = 0;
    startRoom.name = malloc(16 * sizeof(char));
    strcpy(startRoom.name, "Living Room");

    struct room copiedRoom;
    copiedRoom = startRoom;

    printf("Starting Room's name and id are: %s %d \n", startRoom.name, startRoom.id);
    printf("Copied Room's name and id are: %s %d \n", copiedRoom.name, copiedRoom.id);

    printf("strcpy()ing to copiedRoom which is a shared pointer\n");
    strcpy(copiedRoom.name, "PARLOR");

    printf("Starting Room's name and id are: %s %d \n", startRoom.name, startRoom.id);
    printf("Copied Room's name and id are: %s %d \n", copiedRoom.name, copiedRoom.id);

    return 0;
}

/* Output:

Starting Room's name and id are: (Living Room/0)
Copied Room's name and id are: (Living Room/0)
strcpy()ing to copiedRoom which is a shared pointer
Starting Room's name and id are: (PARLOR/0)
Copied Room's name and id are: (PARLOR/0)

*/
