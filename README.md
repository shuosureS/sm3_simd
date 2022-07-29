# Project: do your best to optimize SM3 implementation (software)

### 刘硕-202000460043-shuosureS

## 简介

本项目对实现了sm3并对其进行了simd优化，在sm3运行次数足够多的情况下（这里测试了）对比优化前和优化后速度提高了十多倍。运行次数太少优化不明显甚至负优化，因为使用simd的代价比重太大。实现过程中遇到了很多问题，最大的问题是_mm_rol无法正常运行通过，可能是处理器是AMD的原因，如果_mm_rol可以正常运行优化的结果还能更好，在项目里同样附上了用_mm_rol实现的代码。

## 代码说明

对于sm3的优化主要是对P1函数进行了simd，其主要代码如下。在一开始编写时尝试了__m256i每组8个变量进行simd但是结果不正确，改成_m128i后也不对，最后发现只能最多三个一组，因为会有依赖关系，所以不能得出正确结果，并且_mm_rol也无法正常使用，可能是因为cpu是AMD的原因，这里也给出使用_mm_rol的代码

```c_cpp

    for (i = 16; i < 67; i++)
    {
        //不优化即把下面一行解注释上面+=3改成++，再把下面的注释掉
        //W[i] = P1(W[i - 16] ^ W[i - 9] ^ Leftshift(W[i - 3], 15)) ^ Leftshift(W[i - 13], 7) ^ W[i - 6];
       
      
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
```

使用_mm_rol的P1函数

```c_cpp
    for (i = 16; i < 67; i += 3)
    {
        //不优化即把下面一行解注释上面+=3改成++，再把下面的注释掉
       // W[i] = P1(W[i - 16] ^ W[i - 9] ^ Leftshift(W[i - 3], 15))^ Leftshift(W[i - 13], 7)^ W[i - 6];


        temp[0] = W[i - 16] ^ W[i - 9] ^ Leftshift(W[i - 3], 15);
        temp[1] = W[i - 15] ^ W[i - 8] ^ Leftshift(W[i - 2], 15);
        temp[2] = W[i - 14] ^ W[i - 7] ^ Leftshift(W[i - 1], 15);
        temp[3] = 0;
        //printf("%d\n", W[i - 3]);

        a = _mm_loadu_epi32(temp);
        b = _mm_rol_epi32(a, 15);
        c = _mm_rol_epi32(a, 23);
        d = _mm_xor_si128(a, b);
        re = _mm_xor_si128(c, d);
        _mm_storeu_epi32(temp, re);
        //printf("%d\n", temp[0]);
        //printf("%d\n", temp[1]);
        //printf("%d\n", temp[2]);
        //printf("%d\n", temp[3]);
        W[i] = temp[0] ^ Leftshift(W[i - 13], 7) ^ W[i - 6];
        W[i + 1] = temp[1] ^ Leftshift(W[i - 12], 7) ^ W[i - 5];
        W[i + 2] = temp[2] ^ Leftshift(W[i - 11], 7) ^ W[i - 4];
        //W[i+3] = temp[3]^ LeftRotate(W[i - 10], 7)^ W[i - 3];
        //printf("%d\n", W[i]);
        //printf("%d\n", W[i+1]);
       // printf("%d\n", W[i+2]);
        //printf("%d\n", W[i+3]);

    }
    W[i] = P1(W[i - 16] ^ W[i - 9] ^ Leftshift(W[i - 3], 15)) ^ Leftshift(W[i - 13], 7) ^ W[i - 6];
```

在主函数里主要有两个部分，一个是来验证sm3结果是否正确，另一部分就是测试优化时间

```c_cpp
int main(int argc, char* argv[])
{
 
    //验证sm3是否正确
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
   
    
    //测试时间
    unsigned char input2[256] = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
    int ilen2 = 64;
    unsigned char output2[32];
    int i2;
    // ctx;

    int start, end;
    start = clock();
    for (int i = 0; i < 100; i++) {
        SM3(input2, ilen2, output2);
    }
    end = clock();
    printf("time=%dms\n",end-start);
    
}
```

## 结果

首先是验证sm3是否正确

![image text](https://github.com/shuosureS/sm3_simd/blob/master/09f2f52cc514f540633c2cdd202b4a55.png)

然后是时间的对比，优化后时间快了十多倍，还是比较明显的。

|运行10000000次|时间|
|--|--|
|优化前|2953403ms|
|优化后|216334ms|

![image text](https://github.com/shuosureS/sm3_simd/blob/master/fa6ae8c9f20ad07cd848f2389a43f720.png)

![image text](https://github.com/shuosureS/sm3_simd/blob/master/3b5da8ab7a77dcebaf3ec7efd88fcdad.png)
