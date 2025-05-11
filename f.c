#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
I rnk(A x/*0*/){X(RA(I v=rnk(xx);P(v<0,v)F(xn,P(v-rnk(xa),-1))v+1)RmM(rnk(xy))RT_A(1)R_(0))}//-1 for mixed rank
Z U urnk(A x/*0*/){X(RA(urnk(xx)+1)RmM(urnk(xy))RT_A(1)R_(0))}//assuming unirank

  U fG(CO G*a,U n,G v)_(U i=0;W(i<n&&a[i]!=v,i++)i)
Z U fH(CO H*a,U n,H v)_(U i=0;W(i<n&&a[i]!=v,i++)i)
  U fI(CO I*a,U n,I v)_(U i=0;W(i<n&&a[i]!=v,i++)i)
  U fL(CO L*a,U n,L v)_(U i=0;W(i<n&&a[i]!=v,i++)i)

Z A1(fN,A y=_R(cn[tl]);x(xtt?y:rsz(xN,y)))

Z CO W msk[]={0x8080808080808080ll,0x8000800080008000ll,0x8000000080000000ll,0x8000000000000000ll};
A2(fndl,YmMA(r2f(fndl,x,y))YE(fndl(x,gZ(y)))YB(fndl(x,cG(y)))Yzfs(fir(N(fndl(x,enl(y)))))Yt(y(_R(cn[tl])))
 P(xt==TT[yt]||xtZ&&ytZ,C w=xw-3;A z=aL(yn);F_(k,yn,L v=gl(ii(y,k));
  S4(w,B((G)v!=v,zL[k]=NL)v=(UC)v,B((H)v!=v,zL[k]=NL)v=(UH)v,B((I)v!=v,zL[k]=NL)v=(U)v,v=(W)v)
  I(w==3,U i=fL(xV,xn,v);zL[k]=i<xn?i:NL)E(CO W m=msk[w],u=v*(m>>(-1+(1<<w+3)));B b=false;
  F(xn-1+(1<<5-w)>>5-w,W h=0,r[4];Fj(4,W e=~(xL[4*i+j]^u);L s=(m&e)>>-1+(1<<w+3);h|=r[j]=m&(s+(e&~m)))
   B(h,b=true;Fj(4,B(r[j],v=(4*i+j<<3-w)+(CTZ(r[j])>>w+3);zL[k]=v<xn?v:NL))))I(!b,zL[k]=NL)))y(sqzZ(z)))
 fN(y))
Z X2(fndGx,
 R_(fN(x))
 RmMA(mRn(xn,xA);l2f(fndGx,x,y))
 Rilc(L v=gl(x);az(v==(C)v?yL[(UC)v]:NL))
 RE(fndGx(gZ(x),y))
 RB(fndGx(cG(x),y))
 RGHILC(U m=xn;A z=aL(m);
  S4(xw-3,F(m,zl=yL[(UC)xc]),
   F(m,H v=xh;zl=v==(C)v?yL[(UC)v]:NL),
   F(m,I v=xi;zl=v==(C)v?yL[(UC)v]:NL),
   F(m,L v=xl;zl=v==(C)v?yL[(UC)v]:NL))
  x(0);sqzZ(z)))
Z L eyt_(L*d,A x,L i,L k)_(P(k>xn,i)i=eyt_(d,x,i,2*k);S4(xw-3,d[k]=xg,d[k]=xh,d[k]=xi,d[k]=xl);eyt_(d,x,i+1,2*k+1))
Z X1(eyt,R_(et(x))RE(eyt(gZ(x)))RB(eyt(cG(x)))RGHIL(A y=aL(xn+1);*yL=0;eyt_(yL,x,0,1);x(y)))
A2(fndA,Z L*g=0;Z U k;U l=urnk(y);
 P(g,P(l>k-1,r2f(fndA,x,y))P(l<k-1,er(y))L i=1,n=xn;W(i<n,i=2*i+(qA(y,xa)>0))i>>=1+CTZ(~i);y(al(i&&mtc_(y,xa)?g[i]:NL)))
 k=urnk(x);P(l+1<k,er(y))P(l>=k,A z=eyt(asc(xR));x=i1(x,zR),g=zL,z=x(z(fndA(x,y))),g=0;z)
 F(xn,P(mtc_(y,xa),y(al(i))))y(al(NL)))
A2(fndZFS,Z L*g=0;Z C t;
 P(g,YmMA(r2f(fndZFS,x,y))YE(fndZFS(x,gZ(y)))YB(fndZFS(x,cG(y)))Yt(fir(N(fndZFS(x,enl(y)))))
  P(t==TT[yt]||ytZ&&LH(tG,t,tL),U wx=xw-3,wy=yw-3,n=xn;A z=an(yn,tL);
   Fj(yn,L u,v,i=1;S4(wy,u=yG[j],u=yH[j],u=yI[j],u=yL[j])W(i<n,S4(wx,,v=xh,v=xi,v=xl)i=2*i+(u>v))i>>=1+CTZ(~i);S4(wx,,v=xh,v=xi,v=xl)zL[j]=i&&u==v?g[i]:NL)y(z))
  fN(y))
 Yt(P(xt==TT[yt]||ytz&&xtZ,L v=gl(y);S4(xw-3,,F(xn,P(v==xh,ai(i))),F(xn,P(v==xi,ai(i))),F(xn,P(v==xl,ai(i))))al(NL))y(al(NL)))
 t=xt;I(xtF||xtS,xT=G(tG,tH,tI,tL)[xw-3])
 A z=eyt(asc(xR));xT=t;x=i1(x,zR);g=zL;z=x(z(fndZFS(x,y)));g=0;z)
X2(fnd,
 R_(et(y))
 Rm(i1(xx,N(fnd(xy,y))))
 RM(en(y))
 RE(x=gZ(xR);x(fnd(x,y)))
 RA(fndA(x,y))
 RB(x=cG(xR);x(fnd(x,y)))
 RGC(A z=aL(256);F(256,zl=NL);UC*a=xV;U n=xn;F(n,zL[a[n-1-i]]=n-1-i)z(fndGx(y,z)))
 R5(tH,tI,tL,tF,tS,P(xn>2000,fndZFS(x,y))fndl(x,y)))
X2(que,Rs(Z CO C s[][4]={"j","k","hex"};G(&js0,val,unh,ed)[fI((V*)s,L(s),xv)](y))Ril(rnd(gl_(x),y))R_(fnd(x,y)))

Z A2 binF;
Z Y2(binZ,
 R_(et(y))
 RF(x=cF(xR);x(binF(x,y)))
 Rt(YU(ed(y))fir(N(binZ(x,enl(y)))))
 RmMA(r2f(binZ,x,y))
 RE(binZ(x,gZ(y)))
 RB(binZ(x,cG(y)))
 RGHILC(
  XE(x=gZ(xR);x(binZ(x,y)))
  P(xn-(I)xn,ez(y))U wx=xw-3,wy=yw-3;P(!wx,wy?K2("{@[x'`c$127&y;&-128>y;:;-1]}",x,y):K2("{(-1+\\@[&256;128+x;+;1])128+y}",x,y))A z=an(yn,tZ(xn-1));I wz=zw-3,k[2];
  F(yn,L v;S4(wy,v=yg,v=yh,v=yi,v=yl)*k=-1;k[1]=xn;S4(wx,,W(*k+1<k[1],I i=*k+k[1]>>1;k[v<xh]=i),W(*k+1<k[1],I i=*k+k[1]>>1;k[v<xi]=i),W(*k+1<k[1],I i=*k+k[1]>>1;k[v<xl]=i))
       S4(wz,zg=*k,zh=*k,zi=*k,zl=*k))y(z)))
Z Y2(binF,RF(x=of1(xR);x(binZ(x,of1(y))))REBGHILC(binF(x,N(cF(y))))Rt(YU(ed(y))fir(N(binF(x,enl(y)))))RmMA(r2f(binF,x,y))R_(ed(y)))
X2(bin,REBGHILC(binZ(x,y))RF(binF(x,y))Rm(_1(xx,N(bin(xy,y))))R_(et(y)))
