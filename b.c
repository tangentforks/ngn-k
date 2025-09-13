#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
#define C2(x,a...) case x:C(a)
#define C6(u,v,w,x,y,z,a...) case u:case v:case w:case x:case y:case z:{a;break;}
#define C16(x,a...) case x:case x+1:case x+2:case x+3:case x+4:case x+5:case x+6:case x+7:case x+8:case x+9:case x+10:case x+11:case x+12:case x+13:case x+14:case x+15:{a;break;}
#define C32(x,a...) C16(x,C16(x+16,a))
#define U(x,a...) I(!(x),a;goto l)
#define OFF 4 //offset of constants in a function object
#define n1 -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
#define p1  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
enum {bu,  bv=32,bs=64,bg=80,bd=96,ba=112,bp,bm,bM,bx,bX,by,bY,bG,bS,bl,bL,bz,bj,bo,bP,bV,bn,bc};        //opcodes
Z CO C di[]={                     [ba]= 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 2, 2, 0},        //extra bytes after opcode
ds[]={[bv]=n1,n1,n1,   p1,   p1,  [ba]= 1, 1,-1,-1,-1,-1,-1,-1, 1,-1, 1, 0,-1, 0, 1,-1, 0, 0, 1},        //stack size delta
ks[]={                            [ba]=-1,-1, 0, 0, 0, 0, 0, 0, 0, 0,-1, 1, 0, 0, 0, 0, 0, 0, 0};        //stack size delta (coefficient for the next byte)
AX(run,Q(xto)Z I d;P(++d>2048,es8(a,n))P(n-xk,er8(a,n))UC*b=_V(xy),*start=b,c,nl=_n(xA[3]);A l[nl+*b++],*s=l+L(l);MS(l,0,SZ l);MC(l,a,8*n);//virtual machine
 W((c=*b++),S(c,                                                                                         //          |BYTES |          STACK        |         EFFECT
  C32(bu,U(*s=v1[c-bu](*s)))                                                                             //monad     |bu+m  |.. x -> monads[m][x]   |
  C32(bv,A x=*s++;U(*s=x(v2[c-bv](x,*s))))                                                               //dyad      |bv+d  |.. y x -> dyads[d][x;y]|
  C16(bs,A*p=l+c%16;I(*p,mr(*p))*p=*s++)                                                                 //set local |bs+i  |.. x -> ..             |locals[i]:x
  C16(bg,A*p=l+c%16,x=*p;U(*--s=x)xR)                                                                    //get local |bg+i  |.. -> .. locals[i]     |
  C16(bd,A*p=l+c%16,x=*p;U(*--s=x)*p=0)                                                                  //del local |bd+i  |.. -> .. locals[i]     |locals[i]:NULL (freed)
  C2(ba,bp,UC n=*b++;A x=*s;s+=n;U(*s=x((c==ba?_8:prj)(x,s-n+1,n))))                                     //apply|proj|ba,n  |.. z y x -> .. x[y;z]  |
  C6(bm,bM,bx,bX,by,bY,A*p=(c&1?gv:l)+*b++,x=*p;U(x,*s=ev(*s))A y=*s++;                                  //          |      |                       |
   I(c==bm||c==bM,y=v2[*b++](x,y);U(y,*--s=0)*p=x(y))                                                    //mod asgn  |bm,i,d|.. x -> ..             |vars[i]:dyads[d][vars[i];x]
   E(x=*p=d4(x,y,av+*b++,*s);mr(*s);I(c==bx||c==bX,mr(y);U(x,*s=0)s++)                                   //ind asgn  |bx,i,d|.. z y -> ..           |vars[i]:  .[vars[i];y;dyads[d];z]
                                    E(U(x,*s=y(0))U(*s=dot(x,y)))))                                      //ind asgn  |by,i,d|.. z y -> .. r         |vars[i]:r:.[vars[i];y;dyads[d];z]
  C(bG,A x=*--s=gv[*b++];U(x,ev0())xR)                                                                   //get global|bG,i  |.. -> .. globals[i]    |
  C(bS,A*p=gv+*b++,x=*s++,y=*p;*p=y?y(x):x)                                                              //set global|bS,i  |.. x -> ..             |globals[i]:x
  C(bl,UC n=*b++;s+=n-1;*s=sqz(aV(tA,n,s-n+1)))                                                          //list      |bl,n  |.. y x -> .. (x;y)     |
  C(bL,UC n=*b++;A x=*s;U(xtt||xN==n,*s=el(x))F(n,*--s=ii(x,n-1-i)))                                     //unlist    |bL,n  |.. x -> .. x[0] x[1]   |
  C(bj,UC n=*b++;b+=n)                                                                                   //jump      |bj,n  |.. x -> ..             |PC+:n
  C(bz,UC n=*b++;b+=n*!tru(*s++))                                                                        //branch    |bz,n  |.. x -> ..             |if x is falsy, PC+:n
  C(bo,I(!*b,b=start+1)E(*--s=xR))                                                                       //recur     |bo    |.. -> .. o             |o is the current lambda
  C(bP,mr(*s++))                                                                                         //pop       |bP    |.. x -> ..             |
  C(bn,b+=2)                                                                                             //filler    |bn    |.. x -> ..             |
  C(bV,UC i=*b++;U(*s=v2[*b++](_R(xA[i+OFF]),*s),mr(xA[i+OFF]))mr(xA[i+OFF]))                            //const dyad|bV,i,d|.. x -> .. r           |r:dyads[d][consts[i];x]
  D(*--s=_R(xA[c-bc+OFF]))))                                                                             //const     |bc+i  |.. -> .. consts[i]     |
   l:d--;A u=*s;MS(l+nl,0,s-l-nl+1<<3);F(L(l),A x=l[i];I(x,mr(x)))I(!u,H f;UC*e=_C(xy)+1;W(e<b,I(*e==bn,f=*(H*)(e+1))e+=1+di[MIN(bc,*e)])eS(xx,f+(UH)_C(xz)[(C*)b-1-_C(xy)]))u)
#define Nr(a...) {I r_=cr(a);P(r_-OK,r_);}                                                               //compile rvalue; return on error
#define Nl(a...) {I r_=cl(a);P(r_-OK,r_);}                                                               //compile lvalue; return on error
#define OK -1                                                                                            //returned by cl() and cr() on success
#define MB 256                                                                                           //max bytecode size
Z A u,lu;Z UC *b,*m;Z I nb,nl,l[16],cr(A,B);                                                             //u:lambda(src;b:bytes;m:map;l:locals;consts..)
#define M(a) {_C(uy)[nb]=a;_C(uz)[nb]=o;nb+=nb<_n(uy)-1;}                                                //append byte
B gr(I r)_(A y=uy,z=uz;P(nb>yn-4,uy=aa(2*yn,y);uz=aa(2*zn,z);nb=r;b=_C(uy);m=_C(uz);1)0)                 // grow byte code buffer
ZN I li(I v)_(U i=fI(l,nl,v);P(i==nl,-1);i)                                                              //index of a local variable (returns -1 if not found)
ZN V rl(I v,U m){I j=0,*p=_I(lu);F(_n(lu),                                                               //reference local variable (m:mode 0=write, 1=read, 2=delete)
 I((b[p[i]]&15)!=v,p[j++]=p[i])J(m&&LH(bd,b[p[i]],bd+15),b[p[i]]+=bg-bd));AN(j,lu);I(m==2,PSH(lu,ai(nb)))}
ZN A2(mu/*11*/,A z=an(xn+yn,tI);I *p=zI;U i=0,j=0,n=xn,m=yn;W(i<n&&j<m,                                  //merge uses of local variables
 I(xI[i]<yI[j],*p++=xI[i++])J(yI[j]<xI[i],*p++=yI[j++])E(*p++=xI[i++],j++))W(i<n,*p++=xI[i++])W(j<m,*p++=yI[j++]);zn=p-zI;x(y(z)))
Z B cm(A x/*0*/){X(Rv(!xv)Ru(1)RS(P(xn-1,0)S s=su(*xI);U n=SL(s);n&&s[n-1]==':')R_(0))}                  //is x a valid modifier? i.e. :: or primitive monad or symbol ending with ":"
Z V cc(A x/*0*/,I o){U n=un,i=OFF;W(i<n&&!mtc_(x,ua),i++)I(i>=n,PSH(u,xR))M(i+bc-OFF)}                   //append a "load constant" instruction
Z I cl(A x,A y/*00*/,B r){I ns=nb;Q(cm(xx))I v=_v(xx),o=xo;                                              //compile lvalue (x:assignmentNode,y:tree,r:wantResult)
 Y(R_(o)
   RS(I(yn==1,I w=*yI,i=li(w);P(xx==av&&nl,I(i<0,i=nl;P(i>15,o)l[nl++]=w)rl(i,0);M(bs+i)I(r,o=yo;rl(i,2);M(bd+i))OK)P(i>=0,rl(i,1);M(bm)M(i)M(v)I(r,o=yo;rl(i,2);M(bd+i))OK))
      UC i=gi(y);M(v?bM:bS)M(i)I(v,M(v))I(r,M(bG)M(i))OK)
   RA(I n=yn-1;P(!n||n>8u,o)A z=yx;P(z==MKL&&(xx==av||_t(xx)==tu)||n==1&&z==ENL,M(bL)M(n)F(n,Nl(x,yA[i+1],0))I(r,P(xx-av,o))E(M(bP))OK)
      ZS(F(n,Nr(yA[n-i],1))M(bl)M(n)I i=zn-1?-1:li(*zI);I(i>=0,M(r?by:bx)rl(i,1))E(i=gi(z);M(r?bY:bX))M(i)M(v)OK)o))}
Z I cr(A x/*0*/,B r)_(I o=xo;                                                                            //compile rvalue (x:tree,r:wantResult)
 XS(I i=xn-1?-1:li(*xI);I(i>=0,rl(i,2);M(bd+i))J(xn==1&&*xI=='o',M(bo))E(M(bG)M(gi(x)))I(!r,M(bP))OK)    // x.y      variable (possibly qualified)
 P(!xtA||!xn,I(r,cc(x-GAP?x:au,o))OK)                                                                    // 0        constant
 U n=xn;A y=xx;                                                                                          //
 P(y==GAP,F(n-1,A z=xA[i+1];I((C)xo!=-1,H rf=_v(zy);M(bn)M(((UC*)&rf)[0])M(((UC*)&rf)[1]))Nr((C)xo!=-1?zx:z,i==n-2&&r))OK)                                                          // [x;y]    block
 P(n==1,I(r,cc(y,o))OK)                                                                                  // `a       quoted
 P(n==3&&cm(y)&&_tsSA(xy),
  YS(Nr(xz,1);Nr(xy,1);A z=enl(cS(drp(-1,str(ii(y,0)))));Nr(z,1);mr(z);M(ba)M(2)z=aA1(au);Nl(z,xy,r);z(0);OK)
  Nr(xz,1);Nl(x,xy,r);OK)// x[y]+:z     assignment
 P(n>3&&(y==av||y==DLR),n--;I p[n];A*a=xA,v=an(0,tH);                                                    // :[x;y;z] cond
  F(n/2,Nr(*++a,1);M(bz)p[2*i]=nb;M(0)A w=aV(tI,_n(lu),_V(lu));Nr(*++a,r)v=mu(v,lu),lu=w;M(bj)p[2*i+1]=nb;M(0))Nr(n&1?*++a:au,r)lu=mu(lu,v);
  F(n&~1,I d=(i&1?nb-1:p[i+1])-p[i];I(i&1,I j=(n&~1)-1;W(i<j&&d>255,d=p[j]-1-p[i];j-=2))P(d>255,o)b[p[i]]=d)OK)
 I(n==2&&y==FIR,A z=xy;I(ztA&&zn==2,P(zx-REV<3u,Nr(zy,1);M(bu+zx-REV+LAS-au)I(!r,M(bP))OK)))             // *|x      recognized idioms
 I p=0;F(n-1,A z=xA[n-1-i];I(z-GAP,Nr(z,1))E(p=1;cc(GAP,o)))I(p,Nr(xx,1);M(bp)M(n-1))                    // x[y;]    projection
 J(y==MKL,n--;P(n>255u,o)M(bl)M(n))                                                                      // (x;y)    list
 J(n==2&&ytu,M(bu+yv)I(!yv,AN(0,lu)))                                                                    // +x       monad
 J(n==3&&ytv,I(!p&&!_tSA(xy),Q(b[nb-1]>=bc);I i=b[nb-1]-bc;b[nb-1]=bV;M(i)M(yv)m[nb-3]=m[nb-1];)E(M(bv+yv)))             // x+y      dyad
 E(P(n>9,o)Nr(xx,1)M(ba)M(n-1))                                                                          // x[y]     application
 I(!r,M(bP))OK)
A1(qte,/*1*/xtS||xtA?aA1(x):x)                                                                           //quote
Z A2(c2,/*00*/P(xtw&&!ytSA,1)/*P(x==TIL&&ytZ&&yn<4,F(yn,P(gl(ii(y,i))>100u,0))1)*/0)                     //constant folding
Z A3(c3,/*000*/P(ADD<=x&&x<=MUL&&ytzZ&&ztzZ&&(ytt||ztt||yn==zn)&&MAX(xN,yN)<101,1)0)                     //constant folding
Z A1(cf,P(!xtA||!xn,x)P(xx==MKL,F(xn,A y=xa;YSA(x))qte(N(drp(1,x))))P(xn==2?c2(xx,xy):xn==3?c3(xx,xy,xz):0,qte(N(val(x))))P(xx==GAP&&(C)xo!=-1,A y=rsz(xn,au);yx=GAP;F(xn-1,A u=xA[i+1];yA[i+1]=aA2(cf(_R(ux)),uy);P(!yA[i+1],die("CF")))AO(xo,x(y)))A y=rsz(xn,au);F(xn,ya=cf(xa);xa=au;P(!ya,die("CF")))AO(xo,x(y)))
Z I mxs(I i,I s)_(I r=s;W(1,UC c=MIN(bc,b[i++]);r=MAX(r,s);P(!c,r)s+=ds[c]+ks[c]*b[i];i+=di[c]+(c==bj)*b[i];I(c==bz,r=MAX(r,mxs(i+b[i-1],s))))r)//max stack
Z V tco(C k){I i=1,l=nb;B o=0,e=0;W(i<l,I(b[i]==bo&&b[i+1]==ba,UC *c=b+i+3;W(c-b<l&&*c==bj,c+=2+*(c+1))I(*c==bu&&l-i-2<256,I(!e,e=1;F(k,M(bs+i))M(bo)M(bu))b[i]=bj;b[i+1]=l-i-2;b[i+2]=bu))i+=1+di[MIN(bc,b[i])])}
Z B shy(A x/*0*/)_(P(!xtA||xn<2,0)P(xx==GAP&&(C)xo!=-1,A u=xA[xn-1];shy(ux))xn==3&&cm(xx)&&_tSA(xy))     //is last expr an assignment?
A3(cpl,/*111*/u=aA(OFF);uy=aC(MB);uz=aC(MB);b=_C(uy);m=_C(uz);nb=1;I ns=nb;lu=an(0,tI);I r,k=0;I(z,k=zn;MC(l,zV,OFF*k);z(0))nl=k;y=Nx(cf(y));ux=x;uA[3]=au;B s=shy(y);B p=1;W(p,r=cr(y,!s);p=gr(ns);AN(0,lu))y(0);P(r-OK,ec0();eS(ux,r);mr(lu);u(0))
 I o=0;I(s,cc(au,o))P(un>255||nl>L(l)-2,ez0();eS(ux,0);mr(lu);u(0))M(bu);ns=nb;p=1;W(p,tco(k);p=gr(ns))P(un>255-bc,eS(ux,0);u(0);ez0())
 mr(lu);*b=mxs(1,0);_C(uz)[0]=-1;AN(nb,uy);AN(nb,uz);uA[3]=aV(tS,nl,l);AK(k,AT(to,u)))
#undef M
