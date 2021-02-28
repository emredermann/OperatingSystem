#include <unistd.h>
#include <stdlib.h>
#include <time.h>


const char alphabet [26] = "abcdefghijklmnopqrstuvwxyz";
int main(int argc, char* argv[]) {
 int M;
 
 if (argc > 1) {
        M = atoi(argv[1]); // get the value of M (will be used in experiments)
    }
    for(int i = 0; i < M; i++){
        printf("%c \n",alphabet[rand() % 26]);
    }

return 0;
}