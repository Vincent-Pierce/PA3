
#include "../include/main.h"

int main(int argc, char* argv[]) {
    uint32_t pageFaults = 0;
    uint32_t diskRefs = 0;
    uint32_t dirtyPageWrite = 0;

    char line[32] = {0};
    int max = 32;

    int pid = 0;
    int addr = 0;
    char op = 0;

    printf("Number of args: %d\n", argc);

    for(int i=0; i<argc;++i)
    {
        printf("argv[%d] = %s\n", i,argv[i]);
    }

    FILE* fd = openFile(argv[1], "r");
    
    while(fgets(line, max, fd) != NULL)
    {
        pid = getPID(line);      // Each pid needs to allocate a Page Table
        addr = getAddress(line); 
        op = getOperation(line);
        uint16_t vpn = getVPN(addr);
        uint16_t offset = getOffset(addr);
        pageTableEntry* currPageTable = getPageTable(pid);

        printf("pid: %d\t virtual address: %d\t operation: %c\n", pid, addr, op);
        if(!currPageTable->valid)
        {
            ++pageFaults;
        }
        // use the vpn to index the page table array of frame numbers: frame = pageTable[vpn]
        // combine the offset and frame number to determine physical address: uint16_t physicalAddress = (frame << 9) |= (offset);

        // printf("VPN: %u\t Offset: %u\n", vpn, offset);
        // [PID, 16-bit memory address, R/W]
    }
    fclose(fd);
    // printf("Total Page Faults: %lu\t Total Disk References: %lu\t Total Dirty Page Writes: %lu\n", pageFaults, diskRefs, dirtyPageWrite);
}


pageTableEntry* getPageTable(int pid)
{
    ProcessNode* curr = processListHead;
    while (curr != NULL) {
        if (curr->pid == pid) {
            return curr->pageTable;
        }
        curr = curr->next;
    }
    
    ProcessNode* newNode = (ProcessNode*)malloc(sizeof(ProcessNode));
    if (!newNode) {
        perror("Failed to allocate");
        exit(EXIT_FAILURE);
    }

    newNode->pid = pid;
    
    newNode->pageTable = (pageTableEntry*)calloc(NUM_PAGES, sizeof(pageTableEntry));
    if (!newNode->pageTable) {
        perror("Failed to allocate pageTable");
        exit(EXIT_FAILURE);
    }

    newNode->next = processListHead;
    processListHead = newNode;

    return newNode->pageTable;
}

uint16_t getVPN(int virtualAddress)
{
    uint16_t VPN = VPN_FLAG; // VPN = [0-7]
    VPN &= virtualAddress;
}

uint16_t getOffset(int virtualAddress)
{
    uint16_t offset = OFFSET_FLAG; // offset = [8-15]
    offset &= virtualAddress;
}


int getPID(char* line) 
{
    return line[0]  - '0';
}

int getAddress(char* line)
{
    char address[16] = {0};
    for(uint8_t ch = 2; ch < strlen(line); ++ch)
    {
        if(isdigit(line[ch]))
        {
           address[ch-2] = line[ch]; 
        }
        else 
        {
            break;
        }
    }
    return atoi(address);
}

char getOperation(char* line)
{
    if(line[strlen(line)-1] == '\n')
    {
        return line[strlen(line)-2];
    }
    else
    {
        return line[strlen(line)-1]; 
    }
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