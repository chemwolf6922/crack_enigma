#include "entropy.h"
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>

float calculate_entropy_direct(uint8_t* codes,int len)
{
    int bins[26];
    for(int i=0;i!=26;i++)
    {
        bins[i] = 0;
    }
    for(int i=0;i!=len;i++)
    {
        bins[codes[i]]++;
    }
    float probility = 1.0f;
    float entropy = 0.0f;
    for(int i=0;i!=26;i++)
    {
        if(bins[i]!=0)
        {
            probility = (float)bins[i] / (float)len;
            entropy += -probility*logf(probility);
        }
    }
    return entropy;
}

uint8_t* text_to_codes(char* text,size_t* len)
{
    size_t text_len = strlen(text);
    uint8_t* codes = (uint8_t*)malloc(text_len);
    if(codes == NULL)
    {
        *len = 0;
        return NULL;
    }
    for(int i=0;i!=text_len;i++)
    {
        codes[i] = text[i] - 'A';
        if(codes[i]<0 || codes[i]>=26)
        {
            free(codes);
            *len = 0;
            return NULL;
        }
    }
    *len = text_len;
    return codes;
}

float calculate_entropy(char* text)
{
    size_t len = 0;
    uint8_t* codes = text_to_codes(text,&len);
    if(codes == NULL)
    {
        return -1.0f;
    }
    float entropy = calculate_entropy_direct(codes,len);
    free(codes);
    return entropy;
}

double entropy_speed_test(char* src,int iteration)
{
    size_t len;
    uint8_t* codes = text_to_codes(src,&len);
    if(codes == NULL)
    {
        return 0.0;
    }
    struct timeval start,end;
    gettimeofday(&start,NULL);
    for(int i=0;i!=iteration;i++)
    {
        calculate_entropy_direct(codes,len);
    }
    gettimeofday(&end,NULL);
    free(codes);
    double speed = ((double)iteration*1e6)/(double)(end.tv_sec*1000000+end.tv_usec-(start.tv_sec*1000000+start.tv_usec));
    return speed;
}