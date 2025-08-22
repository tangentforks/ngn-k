#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE

Z A2 f;
Z C VER=0x00;
A1(ser,x(f(x,apc(aC(0),VER))))
Z A ff(L v,A y)_(cts(y,(V*)&v,8))
Z A fi(I v,A y)_(cts(y,(V*)&v,4))
Z A2(fA,/*01*/N n=xn;y=fi(n,y);F(n|!n,y=f(xa,y))y)
Z A2(f,/*01*/y=apc(y,xt);
 X(
   RmMA(fA(x,y))
   RE(y=ff(xL[0],y);ff(xL[1],y))
   RS(y=fi(xn,y);F(xn,y=f(as(xi),y))y)
   Rs(L v=xv;S s=su(v);I n=SL(s)+1;y=apc(y,n);cts(y,s,n))
   R6(ti,tc,tu,tv,tw,tx,ff(x,y))
   R4(to,tp,tq,tr,Q(xw==4);fA(x,apc(y,(xtr?xE:0)<<3|xk)))
   R_(cts(fi(xn,y),xV,xtB?(xn+7>>3):(xn<<xw-3))))0)

#define h(n) ({P(p-s<n,ed0());C*r=s;s+=n;r;})
#define h1 (*(C*)h(1))
#define h4 (*(I*)h(4))
#define h8 (*(L*)h(8))
Z A0 g;Z C*s,*p;
A1(des,XC(s=xV;p=s+xn;C ver=h1;P(ver!=VER,ed(x))x(g()))ed(x))
Z A0(gA,I n=h4;P(!(n<(W)(1ll<<45)),ed0())A x=aA(n);F(n|!n,P(!(xa=g()),mrn(i,xA);AZ(x);x(0)))x)
Z A0(g,C t=h1;P(!(t<(W)tn),ed0())
 S(t,
  RA(gA())
  RE(L i,j;i=h8;j=h8;A x=aE(i,j);AT(t,x))
  R2(tm,tM,A x=N(gA());P(!xtA||xn-2||_t(xx)>tM||_t(xy)>tM||_N(xx)-_N(xy),ed(x))AT(t,mut(x)))
  RS(I n=h4;P(!(n<(W)(1ll<<45)),ed0())A x=aS(n);F(n,xi=Nx(g()))x)
  Rs(N n=h1;sym(h(n)))
  R6(ti,tc,tu,tv,tw,tx,h8)
  R4(to,tp,tq,tr,C c=h1;A x=N(gA());AT(t,AK(c&7,t==tr?AW(c>>3,x):x)))
  R_(I n=h4;P(!(n<(W)(1ll<<45)),ed0())P(t==tB,I m=n+7>>3;AN(n,AT(tB,aV(tC,m,h(m)))))aV(t,n,h(n*(1<<Tw[t]-3)))))0)
