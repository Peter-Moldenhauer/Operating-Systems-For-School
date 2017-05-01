/***********************************************************************
Structures basics
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

    startRoom.id = 100;
    startRoom.name = malloc(16 * sizeof(char));

    strcpy(startRoom.name, "Living Room");

    printf("Starting Room's name and id are: %s %d \n", startRoom.name, startRoom.id);

    return 0;
}

