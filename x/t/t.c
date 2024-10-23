#include "t.h"

// Test support for up to 8 args
A1(Fadd1,Ki(iK(x)+1))
A2(Fadd2,Ki(iK(x)+iK(y)))
A3(Fadd3,Ki(iK(x)+iK(y)+iK(z)))
A4(Fadd4,Ki(iK(x)+iK(y)+iK(z)+iK(u)))
A5(Fadd5,Ki(iK(x1)+iK(x2)+iK(x3)+iK(x4)+iK(x5)))
A6(Fadd6,Ki(iK(x1)+iK(x2)+iK(x3)+iK(x4)+iK(x5)+iK(x6)))
A7(Fadd7,Ki(iK(x1)+iK(x2)+iK(x3)+iK(x4)+iK(x5)+iK(x6)+iK(x7)))
A8(Fadd8,Ki(iK(x1)+iK(x2)+iK(x3)+iK(x4)+iK(x5)+iK(x6)+iK(x7)+iK(x8)))

#define REG(a,n) KR(#a,F##a,n);

__attribute__((constructor)) void loadfns();
void loadfns(){REG(add1,1)REG(add2,2)REG(add3,3)REG(add4,4)REG(add5,5)REG(add6,6)REG(add7,7)REG(add8,8)}
