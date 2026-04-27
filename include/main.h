
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

int getPID(char* line); 
int getAddress(char* line);
char getOperation(char* line);
uint16_t getVPN(int virtualAddress);
uint16_t getOffset(int virtualAddress);

FILE* openFile(const char* restrict path, const char* restrict mode);


typedef struct  {
    bool dirty;
    bool hot;
    bool valid;
    uint8_t frameNumber; // Bit [0-4] frame number 
} pageTableEntry;

typedef struct ProcessNode {
    int pid;
    pageTableEntry* pageTable;
    struct ProcessNode* next;
} ProcessNode;

ProcessNode* processListHead = NULL;

pageTableEntry* getPageTable(int pid);
void cleanupMemory();