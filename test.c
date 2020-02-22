#ifndef __PROGTEST__
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#endif /* __PROGTEST__ */

typedef unsigned long long int num_t;

num_t gcd(num_t a, num_t b, long long int *x, long long int *y) 
{ 
    if (!a) { 
        *x = 0; 
        *y = 1; 
        return b; 
    } 
    long long int x1, y1;
    num_t g = gcd(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1; 
    *y = x1;
    return g; 
} 

num_t count_for_one(num_t length, unsigned s, unsigned bulkhead, unsigned *c1, unsigned *c2) {
  num_t one = s + bulkhead, all = length - bulkhead;
  if(all % one == 0) {
    *c1 = all / one;
    *c2 = 0;
    return 1;
  }
  return 0;
}

unsigned long long int count_for_two                       ( unsigned long long int length,
                                                             unsigned int        s1,
                                                             unsigned int        s2,
                                                             unsigned int        bulkhead,
                                                             unsigned int      * c1,
                                                             unsigned int      * c2 )
{
  if(bulkhead > length) return 0;
  if(bulkhead == length) {
    *c1 = *c2 = 0;
    return 1;
  }
  if(s1 > length) 
    s1 = 0;
  if(s2 > length) 
    s2 = 0;
  if(s1 == s2) 
    s2 = 0;
  switch((s1 != 0) * 2 + (s2 != 0)) {
    case 0: // s1 == s2 == 0
      return 0;
    case 1: // s1 == 0
      return count_for_one(length, s2, bulkhead, c2, c1);
    case 2: // s2 == 0
      return count_for_one(length, s1, bulkhead, c1, c2);
    default: {  // s1 != 0 && s2 != 0
      num_t A = s1 + bulkhead,
            B = s2 + bulkhead,
            C = length - bulkhead;
      /* A * c1 + B * c2 == C */
      if(A > C && B > C) 
        return 0;
      long long int x, y;
      num_t factor = gcd(A, B, &x, &y);
      if(C % factor != 0 ) 
        return 0;
      num_t factorC = gcd(factor, C, &x, &y);
      if(factorC != 1) {
        A /= factorC;
        B /= factorC;
        C /= factorC;
      }
      num_t lim = C / A;
      factor = gcd(A, B, &x, &y);
      long long X = (x * C / factor);
      if(X < 0) 
        X = (B / factor) - (-X % (B / factor));
      X %= B / factor;
      if((C - A * X) % B != 0 || A * X > C) 
        return 0;
      *c1 = X;
      *c2 = (C - A * X) / B;
      return (lim - (*c1)) / B + 1;
    }
  }
}

unsigned long long int hyperloop                           ( unsigned long long int length,
                                                             unsigned int        s1,
                                                             unsigned int        s2,
                                                             unsigned int        s3,
                                                             unsigned int        bulkhead,
                                                             unsigned int      * c1,
                                                             unsigned int      * c2,
                                                             unsigned int      * c3 )
{
  if(bulkhead > length) return 0;
  if(bulkhead == length) {
    *c1 = *c2 = 0;
    return 1;
  }
  if(s1 > length || s1 == s2) 
    s1 = 0;
  if(s2 > length || s2 == s3) 
    s2 = 0;
  if(s3 > length || s1 == s3) 
    s3 = 0;
  if(!s1) {
    *c1 = 0;
    return count_for_two(length, s2, s3, bulkhead, c2, c3);
  }
  if(!s2) {
    *c2 = 0;
    return count_for_two(length, s1, s3, bulkhead, c1, c3);
  }
  if(!s3) {
    *c3 = 0;
    return count_for_two(length, s1, s2, bulkhead, c1, c2);
  }
  num_t res = 0;
  for(num_t i = 0;; i++) {
    if(i * s1 > length - (bulkhead * i)) 
      break;
    unsigned x, y;
    if(i == 3373)
      printf("It is %llu %u %u %u\n", length - ((s1 + bulkhead) * i), s2, s3, bulkhead);
    num_t ret = count_for_two(length - ((s1 + bulkhead) * i), s2, s3, bulkhead, &x, &y);
    if(ret) {
      res += ret;
      *c1 = i;
      *c2 = x;
      *c3 = y;
    } 
  }
  return res;
}

#ifndef __PROGTEST__
//#define __DEBUG__
#ifdef __DEBUG__
int main() {
  unsigned c1, c2, c3;
  num_t ret =  hyperloop ( 6750, 1, 2, 3, 1, &c1, &c2, &c3 );
  printf("%llu: %u %u %u\n", ret, c1, c2, c3);
  return 0;
}
#else
int main ( int argc, char * argv [] )
{
  unsigned int c1, c2, c3;
  assert ( hyperloop ( 100, 4, 7, 5, 0, &c1, &c2, &c3 ) == 42
           && 4 * c1 + 7 * c2 + 5 * c3 + 0 * ( c1 + c2 + c3 + 1 ) == 100 );
  assert ( hyperloop ( 12345, 8, 3, 11, 3, &c1, &c2, &c3 ) == 82708
           && 8 * c1 + 3 * c2 + 11 * c3 + 3 * ( c1 + c2 + c3 + 1 ) == 12345 );
  c1 = 8;
  c2 = 9;
  c3 = 10;
  assert ( hyperloop ( 127, 12, 8, 10, 0, &c1, &c2, &c3 ) == 0
           && c1 == 8
           && c2 == 9
           && c3 == 10 );
  assert ( hyperloop ( 127, 12, 8, 10, 3, &c1, &c2, &c3 ) == 4
           && 12 * c1 + 8 * c2 + 10 * c3 + 3 * ( c1 + c2 + c3 + 1 ) == 127 );
  assert ( hyperloop ( 100, 35, 0, 0, 10, &c1, &c2, &c3 ) == 1
           && c2 == 0
           && c3 == 0
           && 35 * c1 + 10 * ( c1 + 1 ) == 100 );
  assert ( hyperloop ( 100, 35, 0, 35, 10, &c1, &c2, &c3 ) == 1
           && c2 == 0
           && 35 * c1 + 35 * c3 + 10 * ( c1 + c3 + 1 ) == 100 );
  assert ( hyperloop ( 100, 35, 35, 35, 10, &c1, &c2, &c3 ) == 1
           && 35 * c1 + 35 * c2 + 35 * c3 + 10 * ( c1 + c2 + c3 + 1 ) == 100 );
  c1 = 42;
  c2 = 43;
  c3 = 44;
  assert ( hyperloop ( 9, 1, 2, 3, 10, &c1, &c2, &c3 ) == 0
           && c1 == 42
           && c2 == 43
           && c3 == 44 );
  return 0;
}
#endif
#endif /* __PROGTEST__ */
