
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* Big Endian */
#define NUM_PAGES 128
int getPID(); 
int getAddress();
char getOperation();
uint8_t getVPN(int virtualAddress);
uint16_t getOffset(int virtualAddress);

FILE* openFile(const char* restrict path, const char* restrict mode);


typedef struct  {
    bool dirty;
    bool ref;
    bool valid;
    uint16_t virtualAddresses[NUM_PAGES]; // Bits [0-7] = VPN. Bits [8-15] = Offset
} pageTable;