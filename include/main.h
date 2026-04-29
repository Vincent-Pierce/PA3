
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
#define NUM_FRAMES  32
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

typedef struct {
    int pid;
    uint16_t vpn;
} PhysicalFrame;


int getPID(char* line); 
int getAddress(char* line);
char getOperation(char* line);
uint16_t getVPN(int virtualAddress);
uint16_t getOffset(int virtualAddress);
FILE* openFile(const char* restrict path, const char* restrict mode);
pageTableEntry* getPageTable(int pid);
void RAND(int pid, pageTableEntry* pageTable, uint16_t vpn, char op);
void FIFO(int pid, pageTableEntry* pageTable, uint16_t vpn, char op);
void LRU(int pid, pageTableEntry* pageTable, uint16_t vpn, char op);
ProcessNode* processListHead = NULL;
PhysicalFrame physicalMemory[NUM_FRAMES] = {0};
uint32_t pageFaults = 0;
uint32_t diskRefs = 0;
uint32_t dirtyPageWrite = 0;
int allocatedFrames = 0;
int fifoPointer = 0;
int lruPointer = 0;