#include "math.h"

// Function to return gcd of a and b 
u64 gcd(u64 a, u64 b) 
{ 
    if (a == 0) 
        return b; 
    return gcd(b % a, a); 
} 
  
// A simple method to evaluate Euler Totient Function 
u64 phi(u64 n) 
{ 
    u64 result = 1; 
    for (int i = 2; i < n; i++) 
        if (gcd(i, n) == 1) 
            result++; 
    return result; 
} 