/***********************************************************************
Name: Peter Moldenhauer
Date: 5/7/2017
Description: Program 2 - Adventure Part

SORRY FOR THE MESSY CODE! If I have time I will clean it up, just trying
to get it to work for now!

Note to self: compile with -lpthread option in gcc
************************************************************************/

#include <stdio.h>  // for file input/output
#include <stdlib.h> // for gen. purpose standard library (macros, memory allocation, etc.)
#include <string.h> // for string functions
#include <assert.h> // for assert() function (instead of if/else statements)
#include <sys/stat.h> //for mkdir() function
#include <unistd.h> // for NULL pointer
#include <dirent.h> // for directory stream
#include <pthread.h> // for thread implementation



// Enums for struct
enum RoomType { START_ROOM, END_ROOM, MID_ROOM };

// This struct contains info for one given room such as a given room's name, connections and type
struct oneRoom {
    int roomID;
    char* name;
    int* roomConnections;
    int numOfConnections;
    enum RoomType roomType;
};

// This struct contains info for ALL of the rooms that contains an array of oneRoom structs,
// an array of room names and the total number of rooms
struct allRooms {
    struct oneRoom** roomsArray;
    char** roomNames;
    int numberOfRooms;
};


// Function Prototypes...
void getDirName();
struct allRooms *createTheRooms(int n);
void freeMemory(struct allRooms *roomContainer);
void loadData(struct allRooms *roomContainer, char *directoryName);
int readFileLines(FILE *fin, char *fileBuffer, int roomBuffer);
int getRoomIndex(struct allRooms *roomContainer, char *stringStart);
struct oneRoom *getStartingRoom(struct allRooms *roomContainer);
void printRoutes(struct allRooms *roomContainer, struct oneRoom *nodeForRoom);
struct oneRoom *getNextRoom(struct allRooms *roomContainer, struct oneRoom *currentRoom);
void* playTheGame(); // change to a void * when I get to the "threads" portion of the assignment
void endOfGame();
void PrintTime();
void* LogTime(); // void * 


// Other variables (make them not global if I have time?)
char newestDirName[256]; // Holds the name of the newest directory that contains the prefix
int maxGameMoves = 100;
struct oneRoom *currentLocation;
struct oneRoom *userChoice;
char *history[100];
int playerMoves = 0;
int validInput;
struct allRooms *roomArray;
char timeInput[100];
int time_entered = 0;
int game_over = 0;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER; // start up the mutex
pthread_t game_thread, time_thread; // thread ID's
int game_return, time_return;


int main()
{
    // Get the name of the directory that the (most recent) room files are in
    // Note: This block of code was taken from the course lecture material in TopHat section 2.4 in the article "Manipulating Directories"
    getDirName();

	// Create the struct that will hold all of the data from the rooms files
    struct allRooms *allTheRooms = createTheRooms(7);

    // Put the data from the 7 separate files into the struct we just created
    loadData(allTheRooms, newestDirName);

    // Set up the game
    currentLocation = getStartingRoom(allTheRooms); // Find the start node, and set the currentLocation to it.
    roomArray = allTheRooms;

    // create thread here...
    game_return = pthread_create(&game_thread, NULL, playTheGame, NULL);
    pthread_join(game_thread,NULL);

    // Play the game
    while(currentLocation->roomType != END_ROOM && playerMoves < maxGameMoves){ //repeat until the player reaches the end room or the player reaches the maximum moves allowed
        playTheGame();
	// block main thread until game_thread completes 
        pthread_join(game_thread,NULL);
    }

    // End of game is reached - print results
    endOfGame();

    // free up allocated memory
    freeMemory(allTheRooms);
    allTheRooms = NULL;

    return 0;
}




/************************************************
getDirName() Function
This function sets the name of the specific
directory that is needed.
Note: This function was mostly taken from the 
lecture article on managing directories 
************************************************/
void getDirName(){
    int newestDirTime = -1; // The modified timestamp of the newest subdir we've examined
	char targetDirPrefix[32] = "moldenhp.rooms."; // The prefix we're looking for for all subdirs
	//char newestDirName[256]; // Holds the name of the newest directory that contains the prefix
	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck; // Holds the directory we're starting in
	struct dirent *fileInDir; // Holds the current subdir of the starting dir we're looking at
	struct stat dirAttributes; // Holds information we've gained about subdir

	dirToCheck = opendir("."); // Open up the directory this program was run in

	if (dirToCheck > 0) // Make sure the current directory could be opened
	{
		while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in the directory
		{
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has the prefix
			{
				//printf("Found the prefex: %s\n", fileInDir->d_name);
		                stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

				if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
				{
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
					//printf("Newer subdir: %s, new time: %d\n", fileInDir->d_name, newestDirTime);
				}
			}
		}
	}
	closedir(dirToCheck); // Close the directory we opened
	//printf("Newest entry found is: %s\n", newestDirName);
}


/********************************************
createTheRooms() Function
This function allocates memory and returns a
pointer to an allRooms struct
*********************************************/
struct allRooms *createTheRooms(int n){
    // allocate space for an allRooms struct
    struct allRooms *roomContainer = malloc(sizeof(struct allRooms));

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
        roomContainer->roomsArray[i]->roomType = MID_ROOM;

        // set the starting values for the roomConnections array
        int j;
        for(j = 0; j < n; j++)
        {
            roomContainer->roomsArray[i]->roomConnections[j] = 0; // set all of the connections to 0 for now
        }
    }

    // set the START_ROOM and END_ROOM
    roomContainer->roomsArray[0]->roomType = START_ROOM;
    roomContainer->roomsArray[rand() % (roomContainer->numberOfRooms - 1) + 1]->roomType = END_ROOM;

    return roomContainer;
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
        free(roomContainer->roomNames[i]);
        roomContainer->roomNames[i] = NULL;
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

/********************************************************
loadData() Function
This function takes data from files in a given directory
and "loads" the data into a given struct.
********************************************************/
void loadData(struct allRooms *roomContainer, char *directoryName){
    int roomBuffer = 100;
    DIR *directory1;
    struct dirent *newDirectory;
    int fileCount = 0;
    char fileBuffer[roomBuffer];
    char filePathName[roomBuffer];
    char *roomName = 0;
    char *bufferCommand = 0;
    char *name = 0;

    // allocate space in the struct for the correct number of rooms
    roomContainer->roomNames = malloc(sizeof(char *) * roomContainer->numberOfRooms);

    // open the given directory
    directory1 = opendir(directoryName);
    assert(directory1 != NULL);

    // directory . and ..
    readdir(directory1);
    readdir(directory1);

    // load the name of the room in the file
    while((newDirectory = readdir(directory1)) != NULL){
        fileBuffer[0] = 0; // clear the buffer

        // get the file name
        filePathName[0] = 0;
        strcpy(filePathName, directoryName);
        strcat(filePathName, "/");
        strcat(filePathName, newDirectory->d_name);

        // open the file
        FILE *theFile = fopen(filePathName, "r");

        // read the name of the room and store in the buffer
        readFileLines(theFile, fileBuffer, roomBuffer);

        fclose(theFile);

        // find the location of the room name
        roomName = strstr(fileBuffer, ": ") + 2;

        // allocate space for room name to be stored with a max string length of 31
        roomContainer->roomNames[fileCount] = malloc(sizeof(char) * 31);

        //copy roomname to our array and to the node
        strcpy(roomContainer->roomNames[fileCount], roomName);
        roomContainer->roomsArray[fileCount]->name = roomContainer->roomNames[fileCount];

        fileCount++;
    }

    closedir(directory1);

    directory1 = opendir(directoryName);
    assert(directory1 != NULL);

    // directory . and ..
    readdir(directory1);
    readdir(directory1);

    // reset file count
    fileCount = 0;

    //for each entry in directory, load the name, connections, and room type.
    while((newDirectory = readdir(directory1)) != NULL){
        filePathName[0] = 0;
        strcpy(filePathName, directoryName);
        strcat(filePathName, "/");
        strcat(filePathName, newDirectory->d_name);

        FILE *theFile = fopen(filePathName, "r");
        rewind(theFile);

        while(readFileLines(theFile, fileBuffer, roomBuffer)){
            //split string based on command and name.
            name = strstr(fileBuffer, ": ") + 2;
            *strstr(fileBuffer, ": ") = 0;
            bufferCommand = fileBuffer;

            if(strcmp(bufferCommand, "ROOM NAME") == 0){
                assert(strcmp(name, roomContainer->roomsArray[fileCount]->name) == 0);
            }else if(strncmp(bufferCommand, "CONNECTION", 10) == 0){
                roomContainer->roomsArray[fileCount]->roomConnections[getRoomIndex(roomContainer, name)] = 1;
                roomContainer->roomsArray[fileCount]->numOfConnections++;
            }else if(strcmp(bufferCommand, "ROOM TYPE") == 0){
                if(strcmp(name, "START_ROOM") == 0){
                    roomContainer->roomsArray[fileCount]->roomType = START_ROOM;
                }else if(strcmp(name, "MID_ROOM") == 0){
                    roomContainer->roomsArray[fileCount]->roomType = MID_ROOM;
                }else if(strcmp(name, "END_ROOM") == 0){
                    roomContainer->roomsArray[fileCount]->roomType = END_ROOM;
                }else{
                    printf("Error, could not read room type (%s) in file (%s)\n", name, filePathName);
                    exit(1);
                }
            }else{
                printf("Error, could not parse command (%s) in file (%s)\n", bufferCommand, filePathName);
                exit(1);
            }
        }
        fclose(theFile);
        fileCount++;
    }
    closedir(directory1);
}


/*************************************************
readFileLines() Function
This function reads a line from a file and then
returns it when it reaches either a newline or the
end of the file
*************************************************/
int readFileLines(FILE *fin, char *fileBuffer, int roomBuffer){
    char fileChar;
     int i = 0;          //index
     int endFlag = 0;

     while (endFlag == 0) {
          //retrieve the next character
          fileChar = fgetc(fin);

          if (fileChar == '\n' || fileChar == '\r' || fileChar == 0 || fileChar == -1) {
               fileBuffer[i] = 0;
               endFlag = 1;
               return i;
          } else {
               fileBuffer[i] = fileChar;
          }

          //error check, prevent buffer overflows
          assert(i < roomBuffer);

          i++;
     }

     return i;
}


/*********************************************************
getRoomIndex() Function
This function searches through names in the given allRooms
struct and returns the index for the room name, or -1 if
not found
*********************************************************/
int getRoomIndex(struct allRooms *roomContainer, char *stringStart){
    int i;
    for(i = 0; i < roomContainer->numberOfRooms; i++){
        if(strcmp(roomContainer->roomNames[i], stringStart) == 0){
            return i;
        }
    }
    // if index is not found
    return -1;
}


/***********************************************************
getStartingRoom() Function
This function finds the start room in roomContainer and then
returns a pointer to the start room
***********************************************************/
struct oneRoom *getStartingRoom(struct allRooms *roomContainer){
    int i;
    struct oneRoom *roomPointer;

    for(i = 0; i < roomContainer->numberOfRooms; i++){
        roomPointer = roomContainer->roomsArray[i];
        if(roomPointer->roomType == START_ROOM){
            return roomPointer;
        }
    }
    printf("Error, could not find the starting room\n");
    exit(1);
}


/**********************************************************************
printRoutes() Function
This function prints the names of the connections for the given room
**********************************************************************/
void printRoutes(struct allRooms *roomContainer, struct oneRoom *nodeForRoom){
    int i;
    int count = 0;

    for(i = 0; i < roomContainer->numberOfRooms; i++){
        if(nodeForRoom->roomConnections[i] == 1){
            count++;

            if(count < nodeForRoom->numOfConnections){
                printf("%s, ", roomContainer->roomNames[i]);
            }else{
                printf("%s.", roomContainer->roomNames[i]);
            }
        }
    }
}


/************************************************************
getNextRoom() Function
This function gets user input for a specific room name, if it
is a valid room name, a pointer to that room is returned
************************************************************/
struct oneRoom *getNextRoom(struct allRooms *roomContainer, struct oneRoom *currentRoom){
    //char roomChar;
    int index;
    int bufferSize = 100;
    char roomBuffer[bufferSize];
    memset(timeInput, '\0',sizeof(timeInput));

    // get user input
    fgets(roomBuffer, bufferSize, stdin);
    *strchr(roomBuffer, '\n') = 0;

    // Testing...
    //printf("YOU ENTERED: %s\n", roomBuffer);
    sprintf(timeInput, roomBuffer);
    //printf("timeInput is: %s\n", timeInput);
    if(strcmp(timeInput, "time") == 0){
        return NULL;
    }else{
    index = getRoomIndex(roomContainer, roomBuffer);
    if(index >0){
        if(currentRoom->roomConnections[index] == 1)
            return roomContainer->roomsArray[index];
    }
    return NULL;
    }
}


/***************************************************************
playTheGame() Function
This function allows the user to actually play the game and
traverse through the rooms
***************************************************************/
void* playTheGame(){
    // mutex lock at the beginning and unlock at the end
    pthread_mutex_lock(&myMutex);
    // create a thread here...has 4 different arguments, see lecture video
    time_return = pthread_create(&time_thread, NULL, LogTime, NULL);

    do{
            printf("CURRENT LOCATION: %s\r\n", currentLocation->name);
            printf("POSSIBLE CONNECTIONS: ");
            printRoutes(roomArray, currentLocation);
            printf("\r\n");

            printf("WHERE TO? >");

            // get user input, store new location to currentLocation
            userChoice = getNextRoom(roomArray, currentLocation);

            if(userChoice == NULL && (strcmp(timeInput, "time") != 0)){
                validInput = 0; // false
                printf("\r\n");
                printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\r\n");
            }else if(strcmp(timeInput, "time") == 0){
                //printf("\nThe time is 2:33pm\n");
                PrintTime();
            }else{
                validInput = 1; //true
                currentLocation = userChoice;
            }
            printf("\n");
        }while(validInput == 0);

        if(strcmp(timeInput, "time") != 0){
            history[playerMoves] = currentLocation->name;
            playerMoves++;
        }
    // unlock mutex here
    pthread_mutex_unlock(&myMutex);
    // block the main thread until time_thread completes
    pthread_join(time_thread,NULL);

    return NULL;
}


/****************************************************
endOfGame() Function
This function prints text to the user once the end
of the game is reached
****************************************************/
void endOfGame(){
    int i;
    if(currentLocation->roomType == END_ROOM){
        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\r\n");
        printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\r\n", playerMoves);
        // print names of rooms visited in order
        for(i = 0; i < playerMoves; i++){
            printf("%s\r\n", history[i]);
        }
    }else{
        printf("GAME OVER.\r\n");
    }
}


/****************************************************
LogTime() Function
This function writes the current time to a file
****************************************************/
void* LogTime(){
    // mutex lock at begniing and unlock at the end	
    pthread_mutex_lock(&myMutex);

    if (game_over == 0)
    {
        time_t rawtime;
        struct tm *info;

	// reset/clear the arrays
        char complete_date_time[80];
        memset(complete_date_time, '\0',sizeof(complete_date_time));
        char hours[3];
        memset(hours, '\0',sizeof(hours));
        char minutes[3];
        memset(minutes, '\0',sizeof(minutes));
        char am_pm[3];
        memset(am_pm, '\0',sizeof(am_pm));
        char day_name[20];
        memset(day_name, '\0',sizeof(day_name));
        char month[20];
        memset(month, '\0',sizeof(month));
        char day_number[3];
        memset(day_number, '\0',sizeof(day_number));
        char year[5];
        memset(year, '\0',sizeof(year));

	// format the time per the assignment description
        time( &rawtime );
        info = localtime( &rawtime );
        strftime(hours,3,"%I", info);
        strftime(minutes,3,"%M", info);
        strftime(am_pm,3,"%p", info);
        strftime(day_name,20,"%A", info);
        strftime(month,20,"%B", info);
        strftime(day_number,3,"%d", info);
        strftime(year,5,"%Y", info);

        if(hours[0] == '0'){
            hours[0] = hours[1];
            hours[1] = hours[2];
        }

        if(am_pm[0] == 'A'){
            am_pm[0] = 'a';
            am_pm[1] = 'm';
        }

        if(am_pm[0] == 'P'){
            am_pm[0] = 'p';
            am_pm[1] = 'm';
        }

        if(day_number[0] == '0'){
            day_number[0] = day_number[1];
            day_number[1] = day_number[2];
        }
	
	// put it all together...
        strcat(complete_date_time,hours);
        strcat(complete_date_time,":");
        strcat(complete_date_time,minutes);
        strcat(complete_date_time,am_pm);
        strcat(complete_date_time,", ");
        strcat(complete_date_time,day_name);
        strcat(complete_date_time,", ");
        strcat(complete_date_time,month);
        strcat(complete_date_time," ");
        strcat(complete_date_time,day_number);
        strcat(complete_date_time,", ");
        strcat(complete_date_time,year);

	// print this formatted time to the currentTime.txt file
        FILE *f = fopen("currentTime.txt", "w");
        fprintf(f, "%s\n", complete_date_time);
        fclose(f);
    }
    // unlock
    pthread_mutex_unlock(&myMutex);
    return NULL;
}


/***********************************************
PrintTime() Function
This function reads data from a file to print
the current time during gameplay
***********************************************/
void PrintTime(){
    char* source = NULL;
	// open the currentTime.txt file for reading
	FILE * fp = fopen ("currentTime.txt", "r");
	if (fp)
	{
		// initialize source
		fseek(fp, 0L, SEEK_END);
		long bufsize = ftell(fp);
		source = malloc(sizeof(char) * (bufsize + 1));
		memset(source,'\0',sizeof(char) * (bufsize + 1));
		fseek(fp, 0L, SEEK_SET);
		fread(source, sizeof(char), bufsize, fp);
        printf("\n%s",source);
	// close the file
	fclose (fp);
	// free up the mem
        free(source);
	}
}


