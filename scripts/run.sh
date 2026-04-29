#!/bin/bash
ALGO=$1
# error enabled
set e
#  1 = RAND
#  2 = FIFO
#  3 = LRU
#  4 = PER


../build/main ../data/data1.txt "$ALGO"
echo "Run input data1.txt complete"


../build/main ../data/data2.txt "$ALGO"
echo "Run input data2.txt complete"