
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* Big Endian */
#define NUM_PAGES   128
#define OFFSET_FLAG 0x01FF
#define VPN_FLAG    0xFE00

int getPID(); 
int getAddress();
char getOperation();
uint8_t getVPN(int virtualAddress);
uint16_t getOffset(int virtualAddress);

FILE* openFile(const char* restrict path, const char* restrict mode);


typedef struct  {
    bool dirty;
    bool hot;
    bool valid;
    uint8_t frameNumber; // Bit [0-4] frame number 
} pageTableEntry;

pageTableEntry pageTable1[NUM_PAGES] = {0};
pageTableEntry pageTable2[NUM_PAGES] = {0};
pageTableEntry pageTable3[NUM_PAGES] = {0};
pageTableEntry pageTable4[NUM_PAGES] = {0};

pageTableEntry* getPageTable(int pid);