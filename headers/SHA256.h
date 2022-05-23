#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

union messageBlock;
enum status;

typedef struct {
    __uint32_t HASHVALUES[8];
} HashedValues;

typedef HashedValues * HashedValuesPtr;
// To instantiate use: HashedValuesPtr hashedvalue = (HashedValuesPtr) malloc (sizeof(HashedValues));

// Function decleration
// See Section 4.1.2
__uint32_t sig0(__uint32_t x);
__uint32_t sig1(__uint32_t x);

__uint32_t rotr(__uint32_t n, __uint16_t x);
__uint32_t shr(__uint32_t n, __uint16_t x);

__uint32_t SIG0(__uint32_t x);
__uint32_t SIG1(__uint32_t x);

__uint32_t Ch(__uint32_t x,__uint32_t y,__uint32_t z);
__uint32_t Maj(__uint32_t x,__uint32_t y,__uint32_t z);

void printFileContents();
int calcFileSize();
void endianCheckPrint();
_Bool endianCheck();
int fillMessageBlock();
//void calculateHash(FILE *file);
HashedValuesPtr* calculateHash(FILE *file);
int nextMessageBlock(FILE *file, union messageBlock *msgBlock, enum status *state, __uint64_t *numBits);
