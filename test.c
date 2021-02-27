#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#define CommandSize 1500



int main(int argc, char* argv[]) {

        char commands[CommandSize];
    while(1){
        printf("\nisp$: ");
        fgets(commands, CommandSize, stdin);
        commands[strcspn(commands, "\n\r")] = '\0';
        // Replace line endings with string terminator.
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
    // Get the mode from the command center OR separate from the command array.  
    int mode;
    int N;
    
    // printf("\n Before command decider.");

    if(commandDecider(commands,argv1,argv2) == 1){      

            getMode(&N,&mode);
            if (mode == 1){
        
                runMultipleCommandNormalMode(argv1,argv2);
                printf("\n MULTIPLE COMMAND NORMAL MODE EXECUTED");
            }else if (mode == 2){
         
                runMultipleCommandTappedMode(argv1,argv2,N);
                printf("\nMULTIPLE COMMAND TAPPED MODE EXECUTED");
            }  
        }else{

            runSingleCommand(argv1);
            printf("\nSINGLE COMMAND EXECUTED");
    }
    
}

void getMode(int *N,int *mode){
    char commands[CommandSize]; 
    printf("\nRing the format is:<N> <mode> \n");
    fgets(commands, 5, stdin);
    commands[strcspn(commands, "\n\r")] = '\0'; // remove the newline at the end
    
    /*
    for(int i = 0;i < 3;i++){
        printf("new command values are,%s\n",commands);
    }
    */
    // parseCommand(commands,newCommands);
    *N = commands[0]- '0';
    *mode = commands[2]- '0';
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
    int fd[2];
    int status;
    if(pipe(fd) < 0 ){
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
        
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        
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
 
            dup2(fd[0], STDIN_FILENO); 
            close(fd[1]);
            close(fd[0]);
            // Exec system call for argv1 in child 2.
            status = execvp(argv2[0], argv2);
            if (status < 0) {
                fprintf(stderr, "%s Execution of the second command failed.\n");
                return 1;
            }  
        } else { 
        // Parent process
            close(fd[1]); 
            close(fd[0]); 
            waitpid(pid,NULL,0);
            waitpid(pid2,NULL,0);
        }
    }
    close(fd[1]); 
    close(fd[0]); 
}


void runMultipleCommandTappedMode(char* argv1[], char* argv2[],int N){
    //First and second pipe
    int fd[2], fd2[2];
    if(pipe(fd) < 0 ){
    	fprintf(stderr, "%sFailed fork process \n");
    	return 1;
    }
    if(pipe(fd2) < 0)
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
        
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        
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
            dup2(fd2[0], STDIN_FILENO);
            close(fd2[0]);
           
            // Exec system call for argv1 in child 2.
            status = execvp(argv2[0], argv2);
            if (status < 0) {
                fprintf(stderr, "%s Execution of the second command failed.\n");
                return 1;
            }       
        } else { 
        // Parent process
        // tmp buff for change
        char buff[N];
        int readed;
        int writtenBytes = 0;
        int counter =0;
        while( readed = read(fd[0],buff,N > 0 && counter < 4096)){
            writtenBytes = write(fd2[1],buff,readed);
            printf("Temporary buff is %d \n",writtenBytes);
            counter += 1;
        }
            close(fd[1]); 
            close(fd2[0]);
            close(fd[0]); 
            close(fd2[1]);
            waitpid(pid,NULL,0);
            waitpid(pid2,NULL,0);
        }
    }
    close(fd[1]); 
    close(fd2[0]);
    close(fd[0]); 
    close(fd2[1]);
}


// Done
void runSingleCommand(char* argv1[]){
	pid_t pid = fork(); // fork a child
	int status;
    if (pid < 0){
        fprintf(stderr, "\nFork failed.");
        return 1;
    }
	if (pid == 0) { // child process
		status = execvp(argv1[0], argv1);
		if ( status < 0) {
		    fprintf(stderr, "%sCommand execution failed.\n");
		    return 1;
		}
		return 0;
	    } else { 
            // parent process
		    wait(NULL); // wait for the child
	    }

}


void parseCommand(char command[], char* argv1[]) {
    int i =0;
    while(i < 11){  
        argv1[i] = strsep(&command, " ");
        if (argv1[i] == NULL) {
            break;
        }
        i -= (strlen(argv1[i]) == 0);
        i += 1;
    }
}