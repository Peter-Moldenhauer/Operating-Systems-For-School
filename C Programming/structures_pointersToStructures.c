/********************************************************************************************************************
This program demonstrates the use of structs, pointers to structs and calloc()
*********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct room {
    int id;
    char* name;
    int numOutboundConnnections;
    struct room* outboundConnections[6];
};


void PrintRoomOutboundConnections(struct room* input){
    printf("The rooms connected to (%s/%d) are: \n", input->name, input->id);

    int i;
    for(i = 0; i < input->numOutboundConnnections; i++){
        printf(" (%s/%d)\n", input->outboundConnections[i]->name, input->outboundConnections[i]->id);
    }

    return;
}



int main(int argc, char* argv[])
{
    struct room startRoom;
    startRoom.id = 0;
    startRoom.name = calloc(16, sizeof(char));
    strcpy(startRoom.name, "Living Room");

    struct room midRoom1;
    midRoom1.id = 1;
    midRoom1.name = calloc(16, sizeof(char));
    strcpy(midRoom1.name, "Kitchen");

    struct room midRoom2;
    midRoom2.id = 2;
    midRoom2.name = calloc(16, sizeof(char));
    strcpy(midRoom2.name, "Dining Room");

    struct room endRoom;
    endRoom.id = 3;
    endRoom.name = calloc(16, sizeof(char));
    strcpy(endRoom.name, "Garage");

    startRoom.numOutboundConnnections = 2;
    startRoom.outboundConnections[0] = &midRoom1;
    startRoom.outboundConnections[1] = &midRoom2;

    midRoom1.numOutboundConnnections = 3;
    midRoom1.outboundConnections[0] = &startRoom;
    midRoom1.outboundConnections[1] = &midRoom2;
    midRoom1.outboundConnections[2] = &endRoom;

    midRoom2.numOutboundConnnections = 3;
    midRoom2.outboundConnections[0] = &startRoom;
    midRoom2.outboundConnections[1] = &midRoom1;
    midRoom2.outboundConnections[2] = &endRoom;

    endRoom.numOutboundConnnections = 2;
    endRoom.outboundConnections[0] = &midRoom1;
    endRoom.outboundConnections[1] = &midRoom2;

    PrintRoomOutboundConnections(&startRoom);
    PrintRoomOutboundConnections(&midRoom1);
    PrintRoomOutboundConnections(&midRoom2);
    PrintRoomOutboundConnections(&endRoom);

    return 0;
}

/*****************************************************
Output of program:

The rooms connected to (Living Room/0) are:
  (Kitchen/1)
  (Dining Room/2)
The rooms connected to (Kitchen/1) are:
  (Living Room/0)
  (Dining Room/2)
  (Garage/3)
The rooms connected to (Dining Room/2) are:
  (Living Room/0)
  (Kitchen/1)
  (Garage/3)
The rooms connected to (Garage/3) are:
  (Kitchen/1)
  (Dining Room/2)

*************************************************************/
