#include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {
   int M ;
   if (argc > 1) {
        M = atoi(argv[1]);  
    }
    int bytesRead = 0;
    char tmp[M];
    while (bytesRead < M) {
        scanf(" %c",&tmp[bytesRead]); 
        bytesRead += 1;
    }
}