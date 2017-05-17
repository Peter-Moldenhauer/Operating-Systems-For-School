/**********************************************************
Name: Peter Moldenhauer
Date: 5/14/16
Description: Program 3 - smallsh.c
	This program simulates a shell. This shell supports
	the three built in commands: exit, cd and status. 
	It also supports comments which are lines beginning
	with the # character.  
***********************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
#include<sys/stat.h>

// Global variable to determine the state of program
// where new commands can no longer be run in background
int noBg = 0;

// This function catches the SIGTSTP signal
void catchSIGTSTP(int signo){
	if(noBg == 0){
		char* message = "\nEntering foreground-only mode (& is now ignored)\n: ";
		write(1, message, 52);
		noBg = 1;
	}else{
		char* message = "\nExiting foreground-only mode\n: ";
		write(STDOUT_FILENO, message, 32); 
		noBg = 0;
	}
}

// This function replaces a substring with another string - for $$ variable expansion
// Source: http://www.linuxquestions.org/questions/programming-9/replace-a-substring-with-another-string-in-c-170076/
char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}


int main() {
	char* input = malloc(sizeof(char*) * 2048); //command lines with max len. of 2048 chars
	char* args[513]; // max of 512 arguments (512 + 1) 
	int numArgs;
	char* token;
	char* inputFile = NULL;
	char* outputFile = NULL;
	int background = 0;
	int fd;
	int status = 0;
	int pid;
	int exited = 0;
	int i;

	//signal handler to ignore SIGINT
	struct sigaction act = {0}; // initialize to be empty at start
	act.sa_handler = SIG_IGN; //set to ignore
	sigfillset(&act.sa_mask);
	act.sa_flags = 0; 
	sigaction(SIGINT, &act, NULL);

	//signal handler to handle SIGTSTP
	struct sigaction act2 = {0}; // initialize to be empty at start
	act2.sa_handler = catchSIGTSTP;
	act2.sa_flags = SA_RESTART;  // <-- finally figured this out! need this here!
	sigfillset(&act2.sa_mask); 	
	sigaction(SIGTSTP, &act2, NULL);


	while (!exited) {

           background = 0; // variable to determine if command will be executed in background

	   // The : symbol is the prompt for each command line
           printf(": ");     
           fflush(stdout);     

           // Read input via getline 
           ssize_t size = 0;
	   if (!(getline(&input, &size, stdin))) {
		return 0; // end if this is the end of the file
  	   }
        
	   // Begin the process to split the input into "tokens"
           numArgs = 0;
           
	   token = strtok(input, "  \n"); // Get the initial token

	   //printf("initial token is: %s\n", token); // TESTING 

           while (token != NULL) {
              if (strcmp(token, "<") == 0) { // input file
                token = strtok(NULL, " \n"); // get file name
                inputFile = strdup(token);//copy token(file name) to inputFile 

		// Get next arg
                token = strtok(NULL, " \n");
              } 
  	      else if (strcmp(token, ">") == 0) { // output file
                token = strtok(NULL, " \n"); // get the file name
                outputFile =  strdup(token); //copy token(file name) to outputFile

		// Get next arg
                token = strtok(NULL, " \n");
              } 
	      else if (strcmp(token, "&") == 0){ // command in background
		if(noBg == 0){ // if not set in "no background mode"
			background = 1; //set to indicate that it is to be in the background
                	break;
		}else{  // if set in "no background mode"
			background = 0;
			break;
		}
              } 
	      else {
                // This is a command or arg - store in array
                args[numArgs] = strdup(token);

		// Get next token
                token = strtok(NULL, " \n");
                numArgs++; //increment # of args
              }
           }

	   // End array of args with NULL
           args[numArgs] = NULL;


		// Get PID of parent in a string
		char *pidString;
		pidString = malloc(10 * sizeof(char)); 
		memset(pidString, '\0', 10);
		snprintf(pidString, 10, "%d", (int)getpid());

		// Loop through args, switch any instance of $$ with pid
		int k;
		for(k = 0; k < numArgs; k++){
			// replace any instance of $$ as its own argument
			if(strcmp(args[k], "$$") == 0){
				args[k] = pidString;
			}
			// replace any instance of $$ that is part of a larger string
			if(strstr(args[k], "$$") != NULL){
				args[k] = replace_str(args[k], "$$", pidString);
			}
		}


           //Determine command...
           if (args[0] == NULL || !(strncmp(args[0], "#", 1))) { // if it is a comment or NULL
              // ...then don't do anything
           } 
	   else if (strcmp(args[0], "exit") == 0) { // if it is an exit command
            	exit(0); // ...then exit
		exited = 1;
           } 
	   else if (strcmp(args[0], "status") == 0) { // if it is a status command
		// ...then print status
		if (WIFEXITED(status)) { // print exit status
			printf("exit value %d\n", WEXITSTATUS(status));
			fflush(stdout); 
		}
		else { // otherwise print terminating signal
			printf("terminated by signal %d\n", status);
			fflush(stdout); 			
		}
	   }
	   else if (strcmp(args[0], "cd") == 0) { // if it is a cd command
		// if there is not arg for cd command
            	if (args[1] == NULL) {
			// ...then go to HOME directory
               		chdir(getenv("HOME"));
           	} 
		else { // ...otherwise change to specified directory
                	chdir(args[1]);
            	}
       	   }	 
	   else { // if its a command other than the supported commands...
               
		 //fork command
            	pid = fork();

            	if (pid == 0) {  //Child 
 	  	   if(!background) { //If process is not background, command can be inturrupted
			act.sa_handler = SIG_DFL; //set to default
			act.sa_flags = 0;
			sigaction(SIGINT, &act, NULL);
		   }

                   if (inputFile != NULL) { // If input file is given
                    	// ... then open specified file
                    	fd = open(inputFile, O_RDONLY);

                    	if (fd == -1) {
				//Invalid file, exit
                        	fprintf(stderr, "cannot open %s for input\n", inputFile);
				fflush(stdout);
                       		exit(1);
                   	}
                   	else if (dup2(fd, 0) == -1) { //redirect input 
				//Error redirecting input
                        	fprintf(stderr, "dup2 error");
				fflush(stdout); 
                       		exit(1);
                   	}
                   		
			//close the file stream
			close(fd);
                     } 
		     else if (background) {
			//Else the process is in the background
                    	//redirect input to /dev/null 
                    	//if input file not specified
                    	fd = open("/dev/null", O_RDONLY);

                    	if (fd == -1) {
				//error opening
                        	fprintf(stderr, "open error");
				fflush(stdout);
                       		exit(1);
                   	}
                    	else if (dup2(fd, 0) == -1) { //redirect input
				//Error redirecting
                        	fprintf(stderr, "dup2 error");
				fflush(stdout);
                       		exit(1);
                   	}
			
			//Close file stream		
			close(fd);
                     }
                     else  if (outputFile != NULL) { //If file to output to given
                    	//open specified file
                    	fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    
			if (fd == -1) {
				//Error opening file, exit
                        	fprintf(stderr, "Invalid file: %s\n", outputFile);
                        	fflush(stdout);
                       		exit(1);
                   	}

	                if (dup2(fd, 1) == -1) { //Redirect output
				//Error redirecting
        	                fprintf(stderr, "dup2 error");
                	       	fflush(stdout);
				exit(1);
                   	}

			//close file stream
                    	close(fd);
                     }
                
		     //execute command stored in arg[0]
                     if (execvp(args[0], args)) { //execute
			//Command not recognized error, exit
                    	fprintf(stderr, "%s: no such file or directory\n",args[0]);
                    	fflush(stdout);
                   	exit(1);
		      }
                 }
		 else if (pid < 0) { //fork() error if pid < 0
			//print error msg
        	        fprintf(stderr, "fork error");
			fflush(stdout);
                	status = 1;
			break;
		 }
		 else { //Parent
	                if (!background) { //if not in background
        	           //wait for the foreground process to complete
                	   do {
			        waitpid(pid, &status, 0);
			   } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                	   // print out number of signal that killed its foreground child 
                	   if(status == 2){
                		printf("terminated by signal %d\n", status);
				fflush(stdout);
			   }
			} 
			else {
                   	    //print pid if process is in background
                    	    printf("background pid is %d\n", pid);
			    fflush(stdout);
                	}
           	 }	
        }
		
        // empty arg array to reuse for next input
        for (i = 0; i < numArgs; i++) {
            args[i] = NULL;
        }

	// Set files to null to be reused for future commands
        inputFile = NULL;
        outputFile = NULL;

	// free mem
	free(input);
	free(pidString);
	input = NULL; 
	pidString = NULL;

        //background processes finished?
        //Check to see if anything has died
        pid = waitpid(-1, &status, WNOHANG);
        while (pid > 0) {
	    //Print that process is complete
            printf("background pid %d is done: ", pid);
	    fflush(stdout);
	   
 	    if (WIFEXITED(status)) { //If the process ended successfully
	    	printf("exit value %d\n", WEXITSTATUS(status));
		fflush(stdout);
	    }
	    else { //If the process was terminated by a signal
		printf("terminated by signal %d\n", status);
		fflush(stdout);
	    }

	    pid = waitpid(-1, &status, WNOHANG);
        }
   }    

    return 0;
}
