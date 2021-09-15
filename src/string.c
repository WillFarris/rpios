#include "types.h"
#include "string.h"
#include "printf.h"

u64 strlen(char* s)
{
    int len = 0;
    char *cur = s;
    while(*cur++ != 0) ++len;
    return len;
}

u64 strcmp(char *strg1, char *strg2)
{
    while( ( *strg1 != '\0' && *strg2 != '\0' ) && *strg1 == *strg2 )
    {
        strg1++;
        strg2++;
    }
    if(*strg1 == *strg2)
        return 0;
    else
        return *strg1 - *strg2;
}

u64 pow(int i, int pow)
{
    if(pow==0) return 1;
    
    int val = i;
    for(int n=0;n<pow-1;++n)
        i *= val;
    return i;
}

u64 strtol(char* str)
{
    if(!str) return -1;
    int ret = 0;

    int len = strlen(str);
    
    for(int i=0;i<len;++i)
    {
        int digit = str[len-i-1] - '0';
        int power = pow(10, i);
        ret += digit * power;
    }
    return ret;
}