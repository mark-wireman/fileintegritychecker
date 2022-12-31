#ifndef HEADERS_SHA256CPP_H_
#define HEADERS_SHA256CPP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <array>

#define byteSwap32(x) (((x) >> 24) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) | ((x) << 24))
#define byteSwap64(x)                                                      \
	((((x) >> 56) & 0x00000000000000FF) | (((x) >> 40) & 0x000000000000FF00) | \
	 (((x) >> 24) & 0x0000000000FF0000) | (((x) >> 8) & 0x00000000FF000000) |  \
	 (((x) << 8) & 0x000000FF00000000) | (((x) << 24) & 0x0000FF0000000000) |  \
	 (((x) << 40) & 0x00FF000000000000) | (((x) << 56) & 0xFF00000000000000))
// Tell our preprocessor to create a variable MAXCHAR with value of 100000
#define MAXCHAR 100000

struct HashValues {
    uint32_t H[8];
};

// Define a union for easy reference
// Union represents a message block
union messageBlock
{
    uint8_t e[64];
    uint32_t t[16];
    uint64_t s[8];
};

// ENUM to control state of the program
enum status
{
    READ, 
    PAD0,
    PAD1,
    FINISH
};

class SHA256CPP 
{
public:
    SHA256CPP();
    virtual ~SHA256CPP();
    bool endianCheck();
    HashValues calculateHash(FILE *file);
    void printFileContents(FILE *fileForPrinting);
    void endianCheckPrint();

private:
    uint32_t sig0(uint32_t x);
    uint32_t sig1(uint32_t x);

    uint32_t rotr(uint32_t n, uint16_t x);
    uint32_t shr(uint32_t n, uint16_t x);

    uint32_t SIG0(uint32_t x);
    uint32_t SIG1(uint32_t x);

    uint32_t Ch(uint32_t x,uint32_t y,uint32_t z);
    uint32_t Maj(uint32_t x,uint32_t y,uint32_t z);
    
    int calcFileSize(FILE *file);
    int fillMessageBlock(FILE *file, union messageBlock *msgBlock, enum status *state, uint64_t *numBits);
};

#endif /* HEADERS_SHA256CPP_H_ */
