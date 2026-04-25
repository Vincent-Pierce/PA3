
#include "../include/main.h"

int main(int argc, char* argv[]) {
    uint32_t pageFaults = 0;
    uint32_t diskRefs = 0;
    uint32_t dirtyPageWrite = 0;

    char line[32] = {0};
    int max = 32;

    int pid = 0;
    uint8_t addr = 0;
    uint8_t op = 0;

    printf("Number of args: %d\n", argc);

    for(int i=0; i<argc;++i)
    {
        printf("argv[%d] = %s\n", i,argv[i]);
    }

    FILE* fd = openFile(argv[1], "r");
    
    while(fgets(line, max, fd) != NULL)
    {
        pid = getPID(line);
        addr = getAddress(line);
        op = getOperation(line);

        // [PID, 16-bit memory address, R/W]
    }
    fclose(fd);
}

int getPID(char* line) 
{
    return line[0];
}

uint16_t getAddress(char* line)
{
    char address[16] = {0};
    for(uint8_t ch = 3; ch < strlen(line); ++ch)
    {
        if(isdigit(line[ch]))
        {
           address[ch-3] = line[ch]; 
        }
        else 
        {
            break;
        }
    }
    return atoi(address);
}

uint8_t getOperation(char* line)
{
    return line[strlen(line)-1];
}

FILE* openFile(const char* restrict path, const char* restrict mode)
{
    FILE* fd = fopen(path, mode);
    if(fd == NULL)
    {
        printf("File not opened\n");
        perror("File not opened\n");
        
    }
    else
    {
        printf("File opened successfully\n");
        return fd;
    }
}