#include <string.h>
#include <stdio.h>
#include "sm3.h"
#include <iostream>
#include<time.h>
//δ�Ż�ǰ10000000��time=2953403ms
//�Ż���10000000��time=216334ms
//����̫���Ż�������
int main(int argc, char* argv[])
{
 /*
    //��֤sm3�Ƿ���ȷ
    unsigned char input[256] = "abc";
    int ilen = 3;
    unsigned char output[32];
    int i;
    printf("Message:\n");
    printf("%s\n", input);

    SM3(input, ilen, output);
    printf("Hash:\n   ");
    for (i = 0; i < 32; i++)
    {
        printf("%02x", output[i]);
        if (((i + 1) % 4) == 0) printf(" ");
    }

    printf("\n");
   */
    
    //����ʱ��
    unsigned char input2[256] = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
    int ilen2 = 64;
    unsigned char output2[32];
    int i2;
    // ctx;

    int start, end;
    start = clock();
    for (int i = 0; i < 10000000; i++) {
        SM3(input2, ilen2, output2);
    }
    end = clock();
    printf("time=%dms\n",end-start);
    
    system("pause");
}
