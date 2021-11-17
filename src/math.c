#include "math.h"

// Greatest common divisor of a and b 
u64 gcd(u64 a, u64 b) 
{ 
    if (a == 0) 
        return b; 
    return gcd(b % a, a); 
} 

// Least common multiple of a & b
u64 lcm(u64 a, u64 b)
{
    return (a * b)/gcd(a, b);
}

// Order of a mod m
u64 ord(u64 a, u64 m)
{
    if(a == 0) return 1; //ord(e) == 1 for any group
    return m / gcd(a, m);
}

// Euler Totient Function 
u64 phi(u64 n) 
{ 
    u64 result = 1; 
    for (int i = 2; i < n; i++) 
        if (gcd(i, n) == 1) 
            result++; 
    return result; 
}


u64 mod(u64 a, u64 b) {
    return a % b;
}

u64 floor_sqrt(u64 x) 
{ 
    // Base cases 
    if (x == 0 || x == 1) 
    return x; 
  
    // Staring from 1, try all numbers until 
    // i*i is greater than or equal to x. 
    int i = 1, result = 1; 
    while (result <= x) 
    { 
      i++; 
      result = i * i; 
    } 
    return i - 1; 
} 

void prime_factors(u64 n) 
{ 
    // Print the number of 2s that divide n 
    while (n%2 == 0) 
    { 
        fbprintf("%d ", 2); 
        n = n/2; 
    } 
  
    // n must be odd at this point.  So we can skip  
    // one element (Note i = i +2) 
    for (int i = 3; i <= floor_sqrt(n); i = i+2) 
    { 
        // While i divides n, print i and divide n 
        while (n%i == 0) 
        { 
            fbprintf("%d ", i); 
            n = n/i; 
        } 
    } 
  
    // This condition is to handle the case when n  
    // is a prime number greater than 2 
    if (n > 2) 
        fbprintf ("%d ", n); 
} 