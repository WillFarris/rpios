#include "types.h"

u64 strlen(const char* str)
{
    int len = 0;
    char *cur = str;
    while(*cur++ != 0) ++len;
    return len;
}