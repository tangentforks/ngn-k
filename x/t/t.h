#include "unistd.h"
#include "k.h"

#define    _(a...) {return({a;});}

#define A1(f,b...) K f(K x             )_(b)/*1*/
#define A2(f,b...) K f(K x,K y         )_(b)/*01*/
#define A3(f,b...) K f(K x,K y,K z     )_(b)
#define A4(f,b...) K f(K x,K y,K z,K u )_(b)
#define A5(f,b...) K f(K x1,K x2,K x3,K x4,K x5                )_(b)
#define A6(f,b...) K f(K x1,K x2,K x3,K x4,K x5,K x6           )_(b)
#define A7(f,b...) K f(K x1,K x2,K x3,K x4,K x5,K x6,K x7      )_(b)
#define A8(f,b...) K f(K x1,K x2,K x3,K x4,K x5,K x6,K x7,K x8 )_(b)

