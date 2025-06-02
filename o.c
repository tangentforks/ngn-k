#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE

#define M(tx,ty) R(CTZ(SZ(tx))<<2|CTZ(SZ(ty)),F(xn*SZ(ty)+31>>5, \
 L mm VS(32)=~(m*(L)!(yn-(i<<(8-yw))>>8-yw)); \
 ty VS(32)a=CVV(((tx VS(32*SZ(tx)/SZ(ty))*)xV)[i],TY(a)),b=((ty VS(32)*)yV)[i];L c VS(32)=(a&mm)!=(b&mm);P(c[0]|c[1]|c[2]|c[3],0))1)
Z B mtcZ(A x,A y)_(I(xt>yt,SW(x,y))L m VS(32)={0};I md=(yn<<yw)&~-256;I j=md>>6;m[j]=-1ull<<(md&~-64ll);F(4-j-1,m[i+j+1]=-1)
 S(xw-3<<2|yw-3,M(G,G)M(G,H)M(G,I)M(G,L)M(H,H)M(H,I)M(H,L)M(I,I)M(I,L)M(L,L))0)
#undef M

NI B mtc_(A x,A y/*00*/)_(
 P(x==y,1)
 P(xt==yt&&((1<<ti|1<<tc|1<<ts|1<<tu|1<<tv|1<<tw|1<<tx)&1<<xt),xv==yv)
 P(xtz&&ytz,gl_(x)==gl_(y))
 XE(x=gZ(xR);x(mtc_(x,y)))
 YE(mtc_(y,x))
 P(xtZ&&ytZ&&xt-yt&&xn==yn,mtcZ(x,y))
 P(xt-yt||xtP||(xtr&&xE-yE)||xn-yn,0)
 XB(U n=((W)xn<<Tw[xt])>>3;B r=!memcmp(xV,yV,n);P(xn&7,UC m=~(-1<<(xn&7));r&&(xG[n]&m)==(yG[n]&m))r)
 P(!xtR||(LH(tG,xt,tS)),!memcmp(xV,yV,((W)xn<<Tw[xt])+7>>3))
 F(xn|!xn,P(!mtc_(xa,ya),0))1)

A2(mtc,/*01*/y(ai(mtc_(x,y))))
Z CO W o=(-1ull>>12)-1;Z L t(L v)_(v^(W)(v>>63)>>1);Z A of_(A,I);
Z L o0(L v)_(t(v-o))Z V of0LL(CO L*a,L*r,N n){F(n+3&~3,r[i]=o0(a[i]))}A1(of0,Q(xtlL);of_(x,0))
Z L o1(L v)_(t(v)+o)Z V of1LL(CO L*a,L*r,N n){F(n+3&~3,r[i]=o1(a[i]))}A1(of1,Q(xtfF);of_(x,1))
Z A of_(A x,I f)_(N n=xn;C t=xt+(tf-tl)*(1-2*f);A y=MINE(x)?AT(t,xR):an(n,t);Mx((f?of1LL:of0LL)(xV,yV,n))y)
Z I ql(L i,L j)_(i<j?-1:i>j)
I qf(F u,F v)_(ql(o1(*(L*)&u),o1(*(L*)&v)))
I qA(A x,A y/*00*/)_(I v=TS[xt]-TS[yt];P(v,v)
 X(Ril(ql(gl_(x),gl_(y)))
   Rf(qf(*xF,*yF))
   Rs(S s=su(xv);C t[8];U n=SL(s);I(n<5,s=MC(t,s,n+1))strcmp(s,su(yv)))
   RT(F(MIN(xn,yn),A z=ii(x,i),u=ii(y,i);I d=qA(z,u);mr(z(u));P(d,d))ql(xn,yn))
   Ropqr(x=str(xR);y=str(yR);I r=qA(x,y);x(y(0));r)
   R_(ql(x,y)))0)
Z I*ascZ(CO UC*v,UC*g,I*a,I*b,I n,I w)_(U cb[257+128];U*c=cb+128;UC o=0;tilV(a,0,n,2);Fj(w,MS(cb,0,SZ cb);F(n,g[i]=v[w*a[i]+j])F(n,c[g[i]+1]++)I(c[1+*g]-n,I(j==w-1,MC(cb+1,cb+257,128<<2);c=cb;o=128)F(255,c[i+1]+=c[i])F(n,b[c[(g[i]+o)&~-256]++]=a[i])SW(b,a);o=0))a)
Z A grdm(A x/*1*/,A1 f)_(A y=kv(&x);x(x1(Nx(f(y)))))
Z V mrg(A x/*0*/,I*p,I*q,I*b,I*d,I k){I*r=p-q+b;W(1,I(qA(xA[*p],xA[*b])<k,*r++=*p++;P(p==q))E(*r++=*b++;B(b==d)))MC(r,p,q-p<<2);}//merge(k=1),mergeR(k=0)
Z V cis(A x/*0*/,I*p,N n,I*r){F(n,I j=0,k=i,v=p[i];A y=xA[v];W(j<k,I m=j+k>>1;I(qA(y,xA[r[m]])<0,k=m)E(j=m+1))memmove(r+j+1,r+j,i-j<<2);r[j]=v)}//copying_insertionsort
Z V cms(A x/*0*/,I*p,N n,I*r){P(n<17,cis(x,p,n,r);)N m=n/2;cms(x,p+m,n-m,r+m);cms(x,p,m,p+m);mrg(x,p+m,p+2*m,r+m,r+n,1);}//copying_mergesort [[http://kicia.ift.uni.wroc.pl/algorytmy/mergesortpaper.pdf]]
A1(ascA,N n=xn;A z=aI(n);I*p=zI;tilV(p,0,n,2);P(n<17,cis(x,p,n,p);x(z))N m=n/2;A y=aI(n-m);I*t=yI;cms(x,p+m,n-m,t);cms(x,p,m,p+n-m);mrg(x,t,t+n-m,p+n-m,p+n,0);x(y(z)))
X1(asc,Rt(opn(x))Rm(grdm(x,asc))RM(K1("{(!#x){x@<y x}/|.+x}",x))RS(asc(str(x)))RF(asc(of1(x)))RA(P(xn-(I)xn,ez(x))ascA(x))RE(Lij x(0);aE(0,j-i))
 RGC(P(xn-(I)xn,ez(x))N n=xn;I c[257]={},*c129=c+129;F(n,c129[xg]++)F(256,c[i+1]+=c[i])A y=aI(n);I*c128=c+128;Mx(F(n,yI[c128[xg]++]=i))ct(tZ(n-1),y))
 R_(P(xn-(I)xn,ez(x))N n=xn;A y=aC(n),z=aI(n),u=aI(n);Mx(My(u=ascZ(xV,yV,zV,uV,n,(1ll<<xw)+7>>3)==zV?u(z):z(u)))u))
X1(dsc,RMT(x=rev(asc(rev(x)));sub(ai(xN-1),x))Rm(grdm(x,dsc))Ril(cls(gl(x)))R_(et(x)))
X1(grp,Ril(K1("=/:/2#,!:",x))Rm(A y=kv(&x);y=Nx(grp(y));yy=x(i1(x,yy));y)R_(et(x))
 RGC(A r[  256]={};UC b[  256];U nb=0;U c[  256]={};F(xn,UC v=xg;I(!c[v]++,b[nb++]=v))A z=aA(nb);F(nb,za=r[b[i]]=aI(c[b[i]]))I(!nb,*zA=emp(tG))MS(c,0,SZ c);F(xn,UC v=xg;_I(r[v])[c[v]++]=i)x(am(aV(xt,nb,b),z)))
 RH( A r[65536]={};UH b[65536];U nb=0;U c[65536]={};F(xn,UH v=xh;I(!c[v]++,b[nb++]=v))A z=aA(nb);F(nb,za=r[b[i]]=aI(c[b[i]]))I(!nb,*zA=emp(tG))MS(c,0,SZ c);F(xn,UH v=xh;_I(r[v])[c[v]++]=i)x(am(aV(xt,nb,b),z)))
 RI(K1("{$[x;x[*'g]!g@:<g:(&~(~*s)=':s:x i)_i:<x;x!0#,!0]}",x))
 R6(tA,tE,tL,tF,tS,tM,K1("{$[#x;x[*'g]!g@:<g:(&~x~':x i)_i:<x;x!0#,!0]}",x)))
Z A1(cSI,Q(xtS||xtI)C t=tS^tI^xt;MINE(x)?AT(t,x):x(aV(t,xn,xV)))
X1(unq,RM(en(x))Rm(unq(val(x)))RE(x)RS(cSI(unq(cSI(x))))Ril(rndF(gl(x)))R_(et(x))RB(unq(cG(x)))
 RGC(C a[256]={},r[256],t=xt;U n=0;Mx(F(xn,UC v=xg;I(!a[v],a[v]=1;r[n++]=v)))aV(t,n,r))
 R5(tA,tH,tI,tL,tF,P(xn<2,x)P(xn<<xw-3<pg&&!xtA,K1("{x@&(x?x)=!#x}",x))K1("{x@i@<i@:&@[;0;:;1]@~~':x@i:<x}",x)))
