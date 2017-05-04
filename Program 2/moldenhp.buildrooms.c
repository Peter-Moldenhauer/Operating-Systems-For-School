/***********************************************************************
Name: Peter Moldenhauer
Date: 5/2/2017
Description: Program 2 - Rooms Program Part, this program generates 7
different room files, one room per file, in a directory called:
moldenhp.rooms.<PROCESS ID>
************************************************************************/

#include <stdio.h>  // for file input/output
#include <stdlib.h> // for gen. purpose standard library (macros, memory allocation, etc.)
#include <string.h> // for string functions
#include <assert.h> // for assert() function (instead of if/else statements)
#include <time.h>   // for random num. generator
#include <unistd.h> //for process id
#include <sys/stat.h> //for mkdir() function
#include <sys/types.h> // for clock ticks
#include <unistd.h> // for NULL pointer
#include <dirent.h> // for directory stream

#define TOTAL_NUM_OF_ROOM_NAMES 10

// This struct contains info for one given room such as a given room's name, connections and type
struct oneRoom {
    int roomID;
    char* name;
    int* roomConnections;
    int numOfConnections;
    char* roomType;
};

// This struct contains info for ALL of the rooms that contains an array of oneRoom structs,
// an array of room names and the total number of rooms
struct allRooms {
    struct oneRoom** roomsArray;
    char** roomNames;
    int numberOfRooms;
};


// Function Prototypes...
struct allRooms *createTheRooms(int n);
void initializeRooms(struct allRooms *roomContainer);
char* _randName(char *names[], int numNames);
void _swapStringElements(char *anArray[], int A, int B);
void printForTesting(struct allRooms *roomContainer);
void createConnections(struct allRooms *roomContainer);
void _linkRooms(struct allRooms *roomContainer, int firstRoom, int secondRoom);
void createFiles(struct allRooms *roomContainer, char *directoryName);
void _printFileContents(struct allRooms *roomContainer, int roomID, char *fileName);
void freeMemory(struct allRooms *roomContainer);


int main()
{
    // Seed random generator
    srand(time(0));

    // Get PID
    int pid = getpid();

    // Create directory to store the room files in
    char theDirectory[30];
    sprintf(theDirectory, "moldenhp.rooms.%d", pid);
    mkdir(theDirectory, 0777);  // --> On WINDOWS switch to --> mkdir(theDirectory);

    // Build up the 7 rooms
    // Note: even though there are a total of 10 possible room names, we only need 7 of the 10 created
    struct allRooms *allTheRooms = createTheRooms(7);

    // Initialize the room names
    initializeRooms(allTheRooms);

    // Build the connections
    createConnections(allTheRooms);

    // TEST PRINT
    //printForTesting(allTheRooms);

    // Create each file to go into the directory that was created above
    createFiles(allTheRooms, theDirectory);

    // free up the mem. that was allocated
    freeMemory(allTheRooms);

    return 0;
}


/********************************************
createTheRooms() Function
This function allocates memory and returns a
pointer to an allRooms struct
*********************************************/
struct allRooms *createTheRooms(int n){
    // allocate space for an allRooms struct
    struct allRooms *roomContainer = malloc(sizeof(struct allRooms));

    // An array of all the possible room types
    char *allOfTheRoomTypes[] = {"MID_ROOM", "START_ROOM", "END_ROOM"};

    // set the number of rooms
    roomContainer->numberOfRooms = n;
    // set the array of room names to null (for now)
    roomContainer->roomNames = NULL;

    // allocate memory for the array of oneRoom structs
    roomContainer->roomsArray = malloc(sizeof(struct oneRoom *) * n);

    // create each room in the array of oneRoom structs
    int i;
    for(i = 0; i < n; i++)
    {
        // allocate memory for each individual room
        roomContainer->roomsArray[i] = malloc(sizeof(struct oneRoom));

        // set the starting values for this given room
        roomContainer->roomsArray[i]->roomID = i;
        roomContainer->roomsArray[i]->name = NULL;
        roomContainer->roomsArray[i]->roomConnections = malloc(sizeof(int) * n);
        roomContainer->roomsArray[i]->numOfConnections = 0;
        roomContainer->roomsArray[i]->roomType = allOfTheRoomTypes[0];  // set every room to "MID_ROOM" for now

        // set the starting values for the roomConnections array
        int j;
        for(j = 0; j < n; j++)
        {
            roomContainer->roomsArray[i]->roomConnections[j] = 0; // set all of the connections to 0 for now
        }
    }

    // set the START_ROOM and END_ROOM
    roomContainer->roomsArray[0]->roomType = allOfTheRoomTypes[1]; // set the first room to be the start room
    roomContainer->roomsArray[rand() % (roomContainer->numberOfRooms - 1) + 1]->roomType = allOfTheRoomTypes[2]; // set a random room (other than the fist) to be the end room

    return roomContainer;
}


/**************************************************
initializeRooms() Function
This function generates a random room name for
each of the oneRoom structs in the allRooms struct
***************************************************/
void initializeRooms(struct allRooms *roomContainer){
    // allocate space for all of the room names
    roomContainer->roomNames = malloc(sizeof(char *) * roomContainer->numberOfRooms);

    // array of all possible room names
    char *allOfTheRoomNames[] = {"Basement", "Bathroom", "Living Room", "Family Room", "Dining Room", "Attic", "Small Bedroom", "Master Bedroom", "Garage", "Closet"};

    // randomly assign 7 of the 10 rooms
    int i;
    for(i = 0; i < 7; i++){
        roomContainer->roomNames[i] = _randName(allOfTheRoomNames, TOTAL_NUM_OF_ROOM_NAMES - i);
        roomContainer->roomsArray[i]->name = roomContainer->roomNames[i];
    }
}


/**************************************************
_randName() Function
This function returns a unique and random element
from the given array of names
**************************************************/
char* _randName(char *names[], int numNames){
    int randNum = rand() % numNames;

    // swap names[randNum] with names[numNames - 1]
    _swapStringElements(names, randNum, numNames - 1);

    return names[numNames - 1];
}


/******************************************************
_swapStringElements() Function
This function swaps the given array elements at index
A with element at index B
*******************************************************/
void _swapStringElements(char *anArray[], int A, int B){
    char *temp;
	temp = anArray[A];
	anArray[A] = anArray[B];
	anArray[B] = temp;
	return;
}


/*******************************************************
createConnections() Function
This function generates a random number of connections
(min 3, max 6) between all of the rooms
*******************************************************/
void createConnections(struct allRooms *roomContainer){
    //int maxConnections = 6;
    int minConnections = 3;

    int i;
    for(i = 0; i < minConnections + 1; i++){

        int j;
        for(j = 0; j < roomContainer->numberOfRooms; j++){
            // if maxConnections then skip, the last connection has a 1 in 3 chance of additional connection
            if((i < minConnections && roomContainer->roomsArray[j]->numOfConnections <= i) || (i >= minConnections && rand() % 3 == 0)){
                int randomRoom = rand() % roomContainer->numberOfRooms;
                while((roomContainer->roomsArray[j]->roomConnections[randomRoom] == 1) || randomRoom == j){
                    randomRoom = rand() % roomContainer->numberOfRooms;
                }
                // link the rooms together
                _linkRooms(roomContainer, j, randomRoom);
            }
        }
    }
}


/******************************************************
_linkRooms() Function
This function asserts that there are no more than the
max num. of connections and then links the to given
rooms together
******************************************************/
void _linkRooms(struct allRooms *roomContainer, int firstRoom, int secondRoom){
    // make sure that the two rooms have no more than the max num. of connections
    assert(firstRoom != secondRoom);
    assert((roomContainer->roomsArray[firstRoom]->numOfConnections) < (roomContainer->numberOfRooms - 1));
    assert((roomContainer->roomsArray[secondRoom]->numOfConnections) < (roomContainer->numberOfRooms - 1));

    //link first room to the second room
    roomContainer->roomsArray[firstRoom]->roomConnections[secondRoom] = 1;

    // link second room to the first room
    roomContainer->roomsArray[secondRoom]->roomConnections[firstRoom] = 1;

    // increase the number of connections in both rooms
    roomContainer->roomsArray[firstRoom]->numOfConnections++;
    roomContainer->roomsArray[secondRoom]->numOfConnections++;
}


/**********************************************************
createFiles() Function
This function creates each of the separate room files and
puts the correct data in each of the files.
**********************************************************/
void createFiles(struct allRooms *roomContainer, char *directoryName){
    char fileNameA[50];
    char fileNameB[10];
    int i;

    // generate a file for each room
    for(i = 0; i < roomContainer->numberOfRooms; i++){
        //build up the file name
        fileNameA[0] = '\0'; // reset the contents of the file for each iteration
        strcat(fileNameA, directoryName);
        strcat(fileNameA, "/");
        sprintf(fileNameB, "%d", roomContainer->roomsArray[i]->roomID);
        strcat(fileNameA, fileNameB);

        // print data to the file
        _printFileContents(roomContainer, i, fileNameA);
    }
}


/***************************************************************
_printFileContents() Function
This function creates a file and then prints data to it
***************************************************************/
void _printFileContents(struct allRooms *roomContainer, int roomID, char *fileName){
    // create a file with the given file name for writing and assert that it worked
    FILE *fileOut = fopen(fileName, "w");
    assert(fileOut != NULL);

    struct oneRoom *thisRoom = roomContainer->roomsArray[roomID];

    // print the room name to the file
    fprintf(fileOut, "ROOM NAME: %s\n", thisRoom->name);

    // print this rooms connections to the file
    int i;
    int connectionCount = 0;
    for(i = 0; i < roomContainer->numberOfRooms; i++){
        if(thisRoom->roomConnections[i] == 1){  // if there is a connection
            connectionCount++;
            fprintf(fileOut, "CONNECTION %d: %s\n", connectionCount, roomContainer->roomsArray[i]->name);
        }
    }

    // print the room type to the file
    fprintf(fileOut, "ROOM TYPE: %s\n", thisRoom->roomType);

    // close the file
    fclose(fileOut);
}


/**************************************************
freeMemory() Function
This function frees up the mem that was allocated
throughout this program.
***************************************************/
void freeMemory(struct allRooms *roomContainer){
    // free mem in each room in the allRooms struct
    int i;
    for(i = 0; i < roomContainer->numberOfRooms; i++){
        //free(roomContainer->roomsArray[i]->name); // free the name
        free(roomContainer->roomsArray[i]->roomConnections); // free the connections
        //free(roomContainer->roomNames[i]);
        //roomContainer->roomNames[i] = NULL;
        free(roomContainer->roomsArray[i]);
        roomContainer->roomsArray[i] = NULL;
    }

    // free the arrays themselves
    free(roomContainer->roomNames);
    free(roomContainer->roomsArray);

    // reset values
    roomContainer->numberOfRooms = 0;
    roomContainer->roomNames = NULL;
    roomContainer->roomsArray = NULL;

    free(roomContainer);
    roomContainer = NULL; 	
}


/*****************************************
Print function for testing purposes
******************************************/
void printForTesting(struct allRooms *roomContainer){
    int i;
    for(i = 0; i < roomContainer->numberOfRooms; i++){
        printf("Room name: %s\n", roomContainer->roomsArray[i]->name);
        printf("Room type: %s\n", roomContainer->roomsArray[i]->roomType);
        printf("Number of Connections: %d\n", roomContainer->roomsArray[i]->numOfConnections);

        // print out the connections to each room
        struct oneRoom *thisRoom = roomContainer->roomsArray[i];
        int connectionCount = 0;
        int k;
        for(k = 0; k < roomContainer->numberOfRooms; k++){
            if(thisRoom->roomConnections[k] == 1){ // if a connection exists
                connectionCount++;
                printf("CONNECTION %d: %s\n", connectionCount, roomContainer->roomsArray[k]->name);
            }
        }

        printf("\n");
    }
}


