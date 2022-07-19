#pragma once


#ifndef C_SM3_H
#define C_SM3_H



#define SM3_HASH_SIZE 32


typedef struct SM3_ctx {
    unsigned int Digest[SM3_HASH_SIZE / 4];
    unsigned char Block[64];
} SM3Context;

/*
* SM3¼ÆËãº¯Êý
*/
unsigned char* SM3(const unsigned char* data,
    unsigned int datalen, unsigned char digest[SM3_HASH_SIZE]);

#endif //C_SM3_H

