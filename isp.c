#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#define CommandSize 1500

/*
* Author Emre Derman
* Operating Systems Project 1
* Date 28/02/2021
* Approximate Time: 50-60 hours
*/

int main(int argc, char* argv[]) {

        char commands[CommandSize];
    while(1){
        printf("\nisp$: ");
        fgets(commands, CommandSize, stdin);
        // Replace line endings with string terminator.
        commands[strcspn(commands, "\n\r")] = '\0';      
        processInput(commands);
    }
    return 0;
}

// If the parser parsed the commands to 2 command than function returns true.
// else if its single argument then returns false.
int IsMultipleCommandParser(char commands[],char * newCommands[]){
    int i = 0;

    while(i < 2){
        newCommands[i] = strsep(&commands,"|");
        if(newCommands[i] == NULL){
            break;
        }
        i += 1;
    }
    if (newCommands[1] != NULL){
        return 1;
    }
    return 0;
}



// Done
void processInput(char commands []){
    char* argv1[10];
    char* argv2[10];
    
    int mode;
    int N;
    
    if(commandDecider(commands,argv1,argv2) == 1){      

            getMode(&N,&mode);
            
            if (mode == 1){
             
                runMultipleCommandNormalMode(argv1,argv2);

            } 
            if (mode == 2){
                printf("N is :%d\n",N);
                runMultipleCommandTappedMode(argv1,argv2,N);

            }  

        }else{

            runSingleCommand(argv1);
    }
    
}

void getMode(int *N,int *mode){

        printf("\nRing the format is: <N> <mode> \n");
        int  input [2];
        scanf("%d %d",&input[0],&input[1]);
        printf(" N is %d",input[0]);
        printf(" mode is %d",input[1]);
        *N = input[0];
        *mode = input[1];

}


int  commandDecider(char commands[],char * argv1,char * argv2){
    char * newCommands [2];
    int tmp = IsMultipleCommandParser(commands,newCommands);
    if(tmp == 1){
        parseCommand(newCommands[0],argv1);
        parseCommand(newCommands[1],argv2);
        return 1;
        
    }else{
        parseCommand(newCommands[0],argv1);
        return 0;
    }

}

// Done
void runMultipleCommandNormalMode(char* argv1[], char* argv2[]){

    //Single pipe
    int pip[2];
    int status;
    if(pipe(pip) < 0 ){
    	fprintf(stderr, "%sFailed pipe process \n");
    	return 1;
    }
    pid_t pid = fork();
    // Failed fork process.
    if (pid < 0) {
        fprintf(stderr, "%s Fork failed\n");
        return 1;
    
    } else if (pid == 0) { 
    // child 1 created.
        
        dup2(pip[1], 1);
        close(pip[0]);
        close(pip[1]);
        
        // Exec system call for argv1 in  child 1.
        status = execvp(argv1[0], argv1);
        if ( status < 0) {     
            fprintf(stderr, "%s Execution of the first command failed.\n");
            return 1;
        }
    }
    // Parent process 
    else {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            fprintf(stderr, "%s Fork failed for child 2.\n");
            return 1;
        
        }
        // Child 2 Process
        else if (pid2 == 0) { 
 
            dup2(pip[0], 0); 
            close(pip[1]);
            close(pip[0]);
            // Exec system call for argv1 in child 2.
            status = execvp(argv2[0], argv2);
            if (status < 0) {
                fprintf(stderr, "%s Execution of the second command failed.\n");
                return 1;
            }  
        } else { 
        // Parent process
            close(pip[1]); 
            close(pip[0]); 
            // Waits for the forks.
            waitpid(pid,NULL,0);
            waitpid(pid2,NULL,0);
        }
    }
    close(pip[1]); 
    close(pip[0]); 
}


void runMultipleCommandTappedMode(char* argv1[], char* argv2[],int N){
    //First and second pipe
    int firstPipe[2], secondPipe[2];
    if(pipe(firstPipe) < 0 ){
    	fprintf(stderr, "%sFailed fork process \n");
    	return 1;
    }
    if(pipe(secondPipe) < 0)
    {
    	fprintf(stderr, "%sFailed second fork process \n");
    	return 1;
    }
    pid_t pid = fork();
    int status;
    if (pid < 0) {
        fprintf(stderr, "%s Fork failed\n");
        return 1;
    
    } else if (pid == 0) { 
        // child 1
        close(firstPipe[0]);
        dup2(firstPipe[1],1);
        // Exec system call for argv1 in  child 1.
        status = execvp(argv1[0], argv1);
        if ( status < 0) {
            fprintf(stderr, "%s Execution of the first command failed.\n");
            return 1;
        }
        if (status >= 0) {
                fprintf(stderr, "%s Second Execution has been done.\n");
            }      
    }
    // Parent process 
    else {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            fprintf(stderr, "%s second fork failed for child.\n");
            return 1;
        }
        // Child 2 Process
        else if (pid2 == 0) {         
            close(secondPipe[1]);
            close(firstPipe[0]);
            close(firstPipe[1]);
            // Reads from the second pipe.
            dup2(secondPipe[0], 0);
            // Exec system call for argv2 in child 2.
            status = execvp(argv2[0], argv2);
            if (status < 0) {
                fprintf(stderr, "%s Execution of the second command failed.\n");
                return 1;
            }
            if (status >= 0) {
                fprintf(stderr, "%s Second Execution has been done.\n");
            }      
        } else { 
        // Parent process
        close(firstPipe[1]);
        close(secondPipe[0]);
        char buff[256];
        int readed;
        int readCalls = 1;
        int writeCalls = 0;
        int writtenBytes = 0;
        int transferedData = 0;
        int readedBytes = 0;
        while( (readed = read(firstPipe[0],&buff,N)) > 0 ){
            writtenBytes = write(secondPipe[1],buff,readed);
           transferedData += writtenBytes +readedBytes;
            readCalls = 1;
            writeCalls += 1;
        }
            
            close(firstPipe[0]);
            close(secondPipe[1]);
            waitpid(pid,NULL,0);
            waitpid(pid2,NULL,0);
            printf(" \n \n character-count: %d\nread-call-count: %d\nwrite-call-count: %d \n",transferedData,readCalls,writeCalls);
        }
     
    }
    close(firstPipe[1]); 
    close(secondPipe[0]);
    close(firstPipe[0]); 
    close(secondPipe[1]);
    
}

// Done
void runSingleCommand(char* argv1[]){
	pid_t pid = fork(); 
	int status;
    if (pid < 0){
        fprintf(stderr, "\nFork failed.");
        return 1;
    }
	if (pid == 0) { 
        // child process
		status = execvp(argv1[0], argv1);
		if ( status < 0) {
		    fprintf(stderr, "%sCommand execution failed.\n");
		    return 1;
		}
		return 0;

	    } else { 
            // parent process
		    wait(NULL); // waits for the child
	    }

}
void parseCommand(char command[], char* argv1[]) {
    int i =0;
    while(i < 11){  
        argv1[i] = strsep(&command, " ");
        if (argv1[i] == NULL) {
            break;
        }
        if (strlen(argv1[i]) == 0){
            i -= 1;
            }
        i += 1;
    }
}