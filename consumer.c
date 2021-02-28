#include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
   int M ;
   struct timeval startTime;
    struct timeval endTime;
    gettimeofday(&startTime,NULL);
   if (argc > 1) {
        M = atoi(argv[1]);  
    }
    int bytesRead = 0;
    char tmp[M];
    while (bytesRead < M) {
        scanf(" %c",&tmp[bytesRead]); 
        bytesRead += 1;
    }
    gettimeofday(&endTime,NULL);
    printf("Run time as sc : %ld \n",(endTime.tv_sec-startTime.tv_sec));
    printf("Run time as micro sec : %ld \n",(endTime.tv_usec-startTime.tv_usec));
}