#include <immintrin.h>
#include <stdio.h>
#include <memory.h>
#include "sm3.h"


unsigned int Leftshift(unsigned int a, int k)
{
    k = k % 32;
    return ((a << k) & 0xFFFFFFFF) | ((a & 0xFFFFFFFF) >> (32 - k));
}
unsigned int* Reverse(unsigned int* word)
{
    unsigned char* byte, temp;

    byte = (unsigned char*)word;
    temp = byte[0];
    byte[0] = byte[3];
    byte[3] = temp;

    temp = byte[1];
    byte[1] = byte[2];
    byte[2] = temp;
    return word;

}
unsigned int T(int i)
{
    if (i >= 0 && i <= 15)
        return 0x79CC4519;
    else if (i >= 16 && i <= 63)
        return 0x7A879D8A;
    else
        return 0;
}
unsigned int FF(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
    if (i >= 0 && i <= 15)
        return X ^ Y ^ Z;
    else if (i >= 16 && i <= 63)
        return (X & Y) | (X & Z) | (Y & Z);
    else
        return 0;
}
unsigned int GG(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
    if (i >= 0 && i <= 15)
        return X ^ Y ^ Z;
    else if (i >= 16 && i <= 63)
        return (X & Y) | (~X & Z);
    else
        return 0;
}
unsigned int P0(unsigned int X)
{
    return X ^ Leftshift(X, 9) ^ Leftshift(X, 17);
}
unsigned int P1(unsigned int X)
{
    return X ^ Leftshift(X, 15)^ Leftshift(X, 23);
}
void Init(SM3_ctx* context)
{
    context->Digest[0] = 0x7380166F;
    context->Digest[1] = 0x4914B2B9;
    context->Digest[2] = 0x172442D7;
    context->Digest[3] = 0xDA8A0600;
    context->Digest[4] = 0xA96F30BC;
    context->Digest[5] = 0x163138AA;
    context->Digest[6] = 0xE38DEE4D;
    context->Digest[7] = 0xB0FB0E4E;
}
void SM3_update(SM3_ctx* context)
{
    int i;
    unsigned int W[68];
    unsigned int W_[64];
    unsigned int A, B, C, D, E, F, G, H, SS1, SS2, TT1, TT2;
    __m128i a, b, c,d,re;
    a=b=c=d=re=_mm_set1_epi32(0);
    unsigned int temp[4];
    unsigned int temp1[4];
    unsigned int temp2[4];
    //unsigned int t1, t2, t3, t4, t5, t6, t7, t8;
    for (i = 0; i < 16; i++)
    {
        W[i] = *(unsigned int*)(context->Block + i * 4);
        Reverse(W + i);
        
    }
    for (i = 16; i < 67; i+=3)
    {
        //不优化即把下面一行解注释上面+=3改成++，再把下面的注释掉
       // W[i] = P1(W[i - 16] ^ W[i - 9] ^ LeftRotate(W[i - 3], 15))^ LeftRotate(W[i - 13], 7)^ W[i - 6];
       
        
        temp[0] = W[i - 16] ^ W[i - 9] ^ Leftshift(W[i - 3], 15);
        temp[1] = W[i - 15] ^ W[i - 8] ^ Leftshift(W[i - 2], 15);
        temp[2] = W[i - 14] ^ W[i - 7] ^ Leftshift(W[i - 1], 15);
        temp[3] = 0;
        //printf("%d\n", W[i - 3]);
        
        a = _mm_loadu_epi32(temp);
        for (int i = 0; i < 4; i++) {
            temp1[i]=Leftshift(temp[i], 15);
        }
        
        

        b = _mm_loadu_epi32(temp1);
        for (int i = 0; i < 4; i++) {
            temp2[i]=Leftshift(temp[i], 23);
        }
        c = _mm_loadu_epi32(temp2);
        d = _mm_xor_si128(a, b);
        re = _mm_xor_si128(c, d);
        _mm_storeu_epi32(temp, re);
        //printf("%d\n", temp[0]);
        //printf("%d\n", temp[1]);
        //printf("%d\n", temp[2]);
        //printf("%d\n", temp[3]);
        W[i] = temp[0]^ Leftshift(W[i - 13], 7)^ W[i - 6];
        W[i+1] = temp[1]^ Leftshift(W[i - 12], 7)^ W[i - 5];
        W[i+2] = temp[2]^ Leftshift(W[i - 11], 7)^ W[i - 4];
        //W[i+3] = temp[3]^ LeftRotate(W[i - 10], 7)^ W[i - 3];
        //printf("%d\n", W[i]);
        //printf("%d\n", W[i+1]);
       // printf("%d\n", W[i+2]);
        //printf("%d\n", W[i+3]);
       
    }
    W[i] = P1(W[i - 16] ^ W[i - 9] ^ Leftshift(W[i - 3], 15)) ^ Leftshift(W[i - 13], 7) ^ W[i - 6];
    
    __m128i p, q, res;
    p = q = res = _mm_set1_epi32(0);
    //unsigned int Temp[8];
    for (i = 0; i < 64; i+=4)
    {
        //W_[i] = W[i] ^ W[i + 4];

        p = _mm_loadu_epi32(&W[i]);
        q = _mm_loadu_epi32(&W[i+4]);
        res = _mm_xor_si128(p, q);
        _mm_storeu_epi32(&W_[i], res);
        
    }

    A = context->Digest[0];
    B = context->Digest[1];
    C = context->Digest[2];
    D = context->Digest[3];
    E = context->Digest[4];
    F = context->Digest[5];
    G = context->Digest[6];
    H = context->Digest[7];
    for (i = 0; i < 64; i++)
    {
        //        printf("%d: %x\n", i, A);
        //        printf("%d: %x\n", i, B);
        //        printf("%d: %x\n", i, C);
        //        printf("%d: %x\n", i, D);
        //        printf("%d: %x\n", i, E);
        //        printf("%d: %x\n", i, F);
        //        printf("%d: %x\n", i, G);
        //        printf("%d: %x\n", i, H);
        unsigned int SS3;
        //        SS3 = LeftRotate(A, 12);
        //        printf("SS3: %d: %x\n", i, SS1);

        SS1 = Leftshift((Leftshift(A, 12) + E + Leftshift(T(i), i)), 7);
        SS2 = SS1 ^ Leftshift(A, 12);
        TT1 = FF(A, B, C, i) + D + SS2 + W_[i];
        TT2 = GG(E, F, G, i) + H + SS1 + W[i];


        //        printf("SS1: %d: %x\n", i, SS1);
        //        printf("SS2: %d: %x\n", i, SS2);
        //        printf("TT1: %d: %x\n", i, TT1);
        //        printf("TT2: %d: %x\n", i, TT2);

        D = C;
        C = Leftshift(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = Leftshift(F, 19);
        F = E;
        E = P0(TT2);
    }
    context->Digest[0] ^= A;
    context->Digest[1] ^= B;
    context->Digest[2] ^= C;
    context->Digest[3] ^= D;
    context->Digest[4] ^= E;
    context->Digest[5] ^= F;
    context->Digest[6] ^= G;
    context->Digest[7] ^= H;
}
unsigned char* SM3(const unsigned char* message,
    unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
    SM3_ctx context;
    unsigned int i, remainder, bitLen;


    Init(&context);


    for (i = 0; i < messageLen / 64; i++)
    {
        memcpy(context.Block, message + i * 64, 64);
        SM3_update(&context);
    }


    bitLen = messageLen * 8;
        Reverse(&bitLen);
    remainder = messageLen % 64;
    memcpy(context.Block, message + i * 64, remainder);
    context.Block[remainder] = 0x80;
    if (remainder <= 55)
    {

        memset(context.Block + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
        memcpy(context.Block + 64 - 4, &bitLen, 4);
        SM3_update(&context);
    }
    else
    {
        memset(context.Block + remainder + 1, 0, 64 - remainder - 1);
        SM3_update(&context);
        
        memset(context.Block, 0, 64 - 4);
        memcpy(context.Block + 64 - 4, &bitLen, 4);
        SM3_update(&context);
    }
        for (i = 0; i < 8; i++)
            Reverse(context.Digest + i);
    memcpy(digest, context.Digest, SM3_HASH_SIZE);

    return digest;
}
