
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int getPID(); 
uint16_t getAddress();
uint8_t getOperation();
FILE* openFile(const char* restrict path, const char* restrict mode);
