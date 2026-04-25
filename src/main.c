#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    
    printf("Number of args: %d\n", argc);

    for(int i=0; i<argc;++i)
    {
        printf("argv[%d] = %s\n", i,argv[i]);
    }
    // get line
    // [PID, 16-bit memory address, R/W]
}