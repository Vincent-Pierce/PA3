
#include "../include/main.h"

int main(int argc, char* argv[]) {
    srand(1432); // Modify for the Random page replacement policy
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
        RAND(pid, currPageTable, vpn, op);
        // FIFO(pid, currPageTable, vpn, op);
        // LRU(pid, currPageTable, vpn, op);
        PER(pid, currPageTable, vpn, op);
    }
    fclose(fd);
    printf("Total Page Faults: %u\t Total Disk References: %u\t Total Dirty Page Writes: %u\n", pageFaults, diskRefs, dirtyPageWrite);
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
    // uint16_t VPN = VPN_FLAG; // VPN = [0-7]
    // VPN &= virtualAddress;
    uint16_t VPN = (virtualAddress & VPN_FLAG) >> 9; 
    return VPN;
}

uint16_t getOffset(int virtualAddress)
{
    // uint16_t offset = OFFSET_FLAG; // offset = [8-15]
    // offset &= virtualAddress;
    uint16_t offset = (virtualAddress & OFFSET_FLAG); 
    return offset;
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

void RAND(int pid, pageTableEntry* pageTable, uint16_t vpn, char op)
{
    if(pageTable[vpn].valid)
    {
        if(op =='W')
        {
            pageTable[vpn].dirty = true;
        }
        return;
    }
    ++pageFaults;
    ++diskRefs;
    if(allocatedFrames < NUM_FRAMES)
    {
        pageTable[vpn].frameNumber = allocatedFrames;
        pageTable[vpn].valid = true;
        pageTable[vpn].dirty = (op == 'W');
        physicalMemory[allocatedFrames].pid = pid;
        physicalMemory[allocatedFrames].vpn = vpn;
        ++allocatedFrames;
    }
    else 
    {
        // Implementing random victim page;
        uint8_t victimIndex = rand() % NUM_FRAMES;
        PhysicalFrame* victimFrame = &physicalMemory[victimIndex];
        
        pageTableEntry* current = getPageTable(victimFrame->pid); 

        uint16_t victimVPN = victimFrame->vpn;
        if(current[victimVPN].dirty)
        {
            ++dirtyPageWrite;
            ++diskRefs;
        }
        current[victimVPN].valid = false; // Remove from physical memory
        current[victimVPN].dirty = false; // Clear dirty bit
    

        // Allocate the new page to the freed frame
        pageTable[vpn].frameNumber = victimIndex;
        pageTable[vpn].valid = true;
        pageTable[vpn].dirty = (op == 'W');
        physicalMemory[victimIndex].pid = pid;
        physicalMemory[victimIndex].vpn = vpn;
    }
}

void FIFO(int pid, pageTableEntry* pageTable, uint16_t vpn, char op)
{
    if(pageTable[vpn].valid)
    {
        if(op == 'W')
        {
            pageTable[vpn].dirty = true;
        }
        
        return;
    }

    ++pageFaults;
    ++diskRefs;
    
    if(allocatedFrames < NUM_FRAMES)
    {
        pageTable[vpn].frameNumber = allocatedFrames;
        pageTable[vpn].valid = true;
        pageTable[vpn].dirty = (op == 'W');
        physicalMemory[allocatedFrames].pid = pid;
        physicalMemory[allocatedFrames].vpn = vpn;
        ++allocatedFrames;
    }
    else
    {
        PhysicalFrame* victimFrame = &physicalMemory[fifoPointer];

        pageTableEntry* current = getPageTable(victimFrame->pid);

        uint16_t victimVPN = victimFrame->vpn;
        if(current[victimVPN].dirty)
        {
            ++dirtyPageWrite;   //Write back to disk 
            ++diskRefs;
        }
        current[victimVPN].valid = false; // Remove from physical memory
        current[victimVPN].dirty = false; // Clear dirty bit
    

        // Allocate the new page to the freed frame
        pageTable[vpn].frameNumber = fifoPointer;
        pageTable[vpn].valid = true;
        pageTable[vpn].dirty = (op == 'W');
        physicalMemory[fifoPointer].pid = pid;
        physicalMemory[fifoPointer].vpn = vpn;

        fifoPointer = (fifoPointer + 1) % NUM_FRAMES; // Move FIFO pointer
    }
}

void LRU(int pid, pageTableEntry* pageTable, uint16_t vpn, char op)
{
    accessTime++;
    if(pageTable[vpn].valid)
    {
        if(op =='W')
        {
            pageTable[vpn].dirty = true;
        }
        pageTable[vpn].time = accessTime; // as soon as a page is brought into MM its time is recorded
        return;
    }
    ++pageFaults;
    ++diskRefs;
    if(allocatedFrames < NUM_FRAMES)
    {
        pageTable[vpn].frameNumber = allocatedFrames;
        pageTable[vpn].valid = true;
        pageTable[vpn].time = accessTime;
        pageTable[vpn].dirty = (op == 'W');
        physicalMemory[allocatedFrames].pid = pid;
        physicalMemory[allocatedFrames].vpn = vpn;
        ++allocatedFrames;
    }
    else 
    {
        // Implementing LRU policy;
        int victimIndex = findLRUVictim();
        PhysicalFrame* victimFrame = &physicalMemory[victimIndex]; 

        pageTableEntry* current = getPageTable(victimFrame->pid);

        uint16_t victimVPN = victimFrame->vpn;

        
            if(current[victimVPN].dirty)
            {
                ++dirtyPageWrite;
                ++diskRefs;
            }
            current[victimVPN].valid = false; // Remove from physical memory
            current[victimVPN].dirty = false; // Clear dirty bit
        

            // Allocate the new page to the freed frame
            pageTable[vpn].frameNumber = victimIndex;
            pageTable[vpn].valid = true;
            pageTable[vpn].time = accessTime;
            pageTable[vpn].dirty = (op == 'W');
            physicalMemory[victimIndex].pid = pid;
            physicalMemory[victimIndex].vpn = vpn;
    
    }
}
int findLRUVictim()
{
    int victimFrame = 0;
    for(int i = 1; i < NUM_FRAMES; i++)
    {
        PhysicalFrame* candidate = &physicalMemory[i];
        PhysicalFrame* victim = &physicalMemory[victimFrame];

        pageTableEntry* candidatePT = getPageTable(candidate->pid);
        pageTableEntry* victimPT = getPageTable(victim->pid);

        uint16_t candidateVPN = candidate->vpn;
        uint16_t victimVPN = victim->vpn;

        pageTableEntry candidateEntry = candidatePT[candidateVPN];
        pageTableEntry victimEntry = victimPT[victimVPN];

        if (candidateEntry.time < victimEntry.time)
        {
            victimFrame = i;
        }
        else if (candidateEntry.time == victimEntry.time)
        {
            if (victimEntry.dirty && !candidateEntry.dirty)
            {
                victimFrame = i;
            }
        }
    }

    return victimFrame;
}

void PER(int pid, pageTableEntry* pageTable, uint16_t vpn, char op)
{
    ++globalTime;
    // Reset hot bits every 200 references
    if (globalTime % 200 == 0) {
        for(int idx = 0; idx < allocatedFrames; ++idx)
        {
            PhysicalFrame* currentFrame = &physicalMemory[idx];
            uint16_t currentVPN = currentFrame->vpn;
            pageTableEntry * currentPT = getPageTable(currentFrame->pid);
            currentPT[currentVPN].hot = false;
        }
    }

    pageTable[vpn].hot = true; // Set to true every reference.

    if(pageTable[vpn].valid)
    {
        if(op == 'W')
        {
            pageTable[vpn].dirty = true;
        }
        return;
    }

    ++pageFaults;
    ++diskRefs;
    
    // First, look for an unused page
    if(allocatedFrames < NUM_FRAMES)
    {
        pageTable[vpn].frameNumber = allocatedFrames;
        pageTable[vpn].valid = true;
        physicalMemory[allocatedFrames].pid = pid;
        physicalMemory[allocatedFrames].vpn = vpn;
        ++allocatedFrames;
        return;
    }

    // First run refernce bit = 0 and dirty bit = 0
    // Second run reference bit = 0 and dirty bit = 1
    // Third run reference bit = 1 and dirty bit = 0
    // Fourth run reference bit = 1 and dirty bit = 1
    for(int reference = 0; reference <= 1; ++reference)
    {
        for(int dirty=0; dirty<=1; dirty++)
        {
            for(int i=0; i<NUM_FRAMES; ++i)
            {
                PhysicalFrame* currentFrame = &physicalMemory[i];
                uint16_t currentVPN = currentFrame->vpn;
                pageTableEntry * currentPT = getPageTable(currentFrame->pid);
                if(currentPT[currentVPN].hot == reference && currentPT[currentVPN].dirty == dirty)
                {
                    replacePhysicalFrame(pid, pageTable, vpn, i);
                    return;
                }
            }
        }
    }
}

void replacePhysicalFrame(int pid, pageTableEntry* pageTable, uint16_t vpn, int victimIndex)
{
    PhysicalFrame* victimFrame = &physicalMemory[victimIndex];

    pageTableEntry* current = getPageTable(victimFrame->pid);

    uint16_t victimVPN = victimFrame->vpn;
    if(current[victimVPN].dirty)
    {
        ++dirtyPageWrite;   //Write back to disk 
        ++diskRefs;
    }
    current[victimVPN].valid = false; // Remove from physical memory
    current[victimVPN].dirty = false; // Clear dirty bit

    // Allocate the new page to the freed frame
    pageTable[vpn].frameNumber = victimIndex;
    pageTable[vpn].valid = true;
    physicalMemory[victimIndex].pid = pid;
    physicalMemory[victimIndex].vpn = vpn;
}