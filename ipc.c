#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
// Kona-compatible IPC - Inter-Process Communication protocol
// Wire format compatible with Kona (https://github.com/kevinlawler/kona)
//
// Usage:
//   fd:<"host:port"   / open connection
//   fd 3: value       / serialize and send (async)
//   fd 4: "expr"      / serialize and send, wait for response (sync)
//   >fd               / close connection
//
// For in-memory serialization:
//   (3:)[0;value]     / serialize to bytes
//   (4:)[0;bytes]     / deserialize from bytes

#ifdef _WIN32
 #include<winsock2.h>
 #define sock_read(fd,buf,n) recv(fd,buf,n,0)
 #define sock_write(fd,buf,n) send(fd,buf,n,0)
#else
 #include<unistd.h>
 #include<sys/socket.h>
 #include<errno.h>
 #define sock_read(fd,buf,n) read(fd,buf,n)
 #define sock_write(fd,buf,n) write(fd,buf,n)
#endif

// Kona message types: {0,1,2} -> {async 3:, sync 4:, response}
enum{MSG_ASYNC=0,MSG_SYNC=1,MSG_RESP=2};

// IPC header format configuration
// Define IPC_HDR32 for 32-bit K3 format (8-byte header)
// Default is 64-bit Kona format (16-byte header)
//
// 32-bit format: [endian:1][b:1][d:1][reserved:1][n:4] = 8 bytes
// 64-bit format: [endian:1][b:1][c:5][d:1][n:8] = 16 bytes
#ifdef IPC_HDR32
 #define HDRSZ 8
 #define HDRLENOFF 4
 #define HDRMSGOFF 2
 typedef I HDRLEN;
#else
 #define HDRSZ (1+1+(sizeof(L)-3)+1+sizeof(L))
 #define HDRLENOFF (HDRSZ-sizeof(L))
 #define HDRMSGOFF (sizeof(L)-1)
 typedef L HDRLEN;
#endif

// Kona data types (from kona/src/k.h)
// -4=sym list, -3=char list, -2=float list, -1=int list
// 0=generic list, 1=int atom, 2=float atom, 4=sym atom, 5=dict

// === Buffer helpers ===

Z V apnd(A*p,CO V*d,N n){A x=*p;
 N c=(HD<<xb)-HD;
 I(xn+n>c,N sz=2*c+n+HD;A y=aC(sz);yn=xn;MC(yV,xV,xn);mr(x);*p=x=y)
 MC(xC+xn,d,n);xn+=n;}

Z V pad8(A*p){N r=(8-(_n(*p)&7))&7;I(r,UC z[8]={0};apnd(p,z,r))}

Z V emitL(A*p,L v){apnd(p,&v,sizeof(L));}

// === Serialization (Kona wrep format) ===

Z N wrep(A*,A);

// Calculate serialized size
Z N rep(A x){
 UC t=_t(x);N n=_N(x);
 N r=2*sizeof(L);// type + count header
 I(t==ti||t==tl,return r)// int/long atom - value in count field
 I(t==tf,return r+sizeof(F))// float atom
 I(t==tc,return r)// char atom - value in count field
 I(t==ts,return r+1+SL(su(xv)))// symbol atom
 I(t==tC,N m=(1+n);I((m%8)>0)m=8*(m/8)+8;return r+m)// char list + null + padding
 I(t==tS,N sz=r;F(n,sz+=1+SL(su(xI[i])))I((sz%8)>0)sz=8*(sz/8)+8;return sz)// symbol list
 I(LH(tE,t,tL),return r+n*sizeof(L))// int list (all widths, serialize as 64-bit)
 I(t==tF,return r+n*sizeof(F))// float list
 I(t==tA,F(n,r+=rep(_A(x)[i]))return r)// generic list
 I(t==tM||t==tm,A k=xx,v=xy;N m=_N(k);N ksz=0;F(m,ksz+=rep(_A(k)[i]))return r+ksz+rep(v))// dict
 return r;}

// Write K value in Kona format, return bytes written
Z N wrep(A*p,A x){
 N start=_n(*p);
 UC t=_t(x);N n=_N(x);

 // Int atom (ti, tl)
 I(t==ti||t==tl,emitL(p,1);emitL(p,_v(x));return 2*sizeof(L))
 // Float atom (value stored in count field as raw bits)
 I(t==tf,emitL(p,2);L fb;F v=*xF;MC(&fb,&v,sizeof(F));emitL(p,fb);mr(x);return 2*sizeof(L))
 // Char atom - store in count field
 I(t==tc,emitL(p,3);emitL(p,_v(x));return 2*sizeof(L))
 // Symbol atom
 I(t==ts,S s=su(xv);N len=SL(s);emitL(p,4);apnd(p,s,len+1);pad8(p);return _n(*p)-start)
 // Char list (string)
 I(t==tC,emitL(p,-3);emitL(p,n);apnd(p,xV,n);UC z=0;apnd(p,&z,1);pad8(p);mr(x);return _n(*p)-start)
 // Symbol list
 I(t==tS,emitL(p,-4);emitL(p,n);F(n,S s=su(xI[i]);N len=SL(s);apnd(p,s,len+1))pad8(p);mr(x);return _n(*p)-start)
 // Int list (all integer list types: tE,tB,tG,tH,tI,tL) - convert to 64-bit
 I(LH(tE,t,tL),emitL(p,-1);emitL(p,n);N w=_W(x);F(n,L v;I(w==1,v=((UC*)xV)[i])else I(w==2,v=((short*)xV)[i])else I(w==4,v=((int*)xV)[i])else v=xL[i];emitL(p,v))mr(x);return _n(*p)-start)
 // Float list
 I(t==tF,emitL(p,-2);emitL(p,n);apnd(p,xV,n*sizeof(F));mr(x);return _n(*p)-start)
 // Generic list
 I(t==tA,emitL(p,0);emitL(p,n);F(n,wrep(p,_R(xa)))mr(x);return _n(*p)-start)
 // Dictionary
 I(t==tM||t==tm,emitL(p,5);emitL(p,_N(xx));wrep(p,_R(xx));wrep(p,_R(xy));mr(x);return _n(*p)-start)
 // Null/other
 emitL(p,6);emitL(p,0);mr(x);return 2*sizeof(L);}

// Create IPC message with header
Z A k3msg(A x,UC msgtype){
 A r=aC(256);_n(r)=0;
 // Header: a=endian, b=0, [c[]=0 for 64-bit], d=msgtype, n=payload length
 UC hdr[HDRSZ];MS(hdr,0,HDRSZ);
 hdr[0]=1;// little endian
 hdr[HDRMSGOFF]=msgtype;
 apnd(&r,hdr,HDRSZ);
 // Write payload
 N dlen=wrep(&r,x);
 // Update length in header
 *(HDRLEN*)(_C(r)+HDRLENOFF)=dlen;
 return r;}

// === Deserialization (Kona rrep format) ===

Z L readL(CO UC*b,N*p){L v;MC(&v,b+*p,sizeof(L));*p+=sizeof(L);_(v)}
Z S readSym(CO UC*b,N*p){S s=(S)(b+*p);W(b[*p],(*p)++)*p+=1;_(s)}

Z A rrep(CO UC*,N,N*);

// Read K value from buffer
Z A rrep(CO UC*b,N len,N*p){
 P(*p+sizeof(L)>len,ed0())
 L t=readL(b,p);

 // Symbol atom - no count field, string follows type directly
 I(t==4,S s=readSym(b,p);I((*p%8)>0)*p+=8-(*p%8);return sym(s))

 // All other types have a count/value field
 P(*p+sizeof(L)>len,ed0())
 L n=readL(b,p);

 // Int atom (value stored in n field)
 I(t==1,return ai(n))
 // Float atom (value stored in n field as raw bits)
 I(t==2,F v;MC(&v,&n,sizeof(F));return af(v))
 // Char atom
 I(t==3,return ac(n))
 // Char list
 I(t==-3,A r=aCn((S)(b+*p),n);*p+=n+1;I((*p%8)>0)*p+=8-(*p%8);return r)
 // Symbol list
 I(t==-4,A r=aS(n);F(n,S s=readSym(b,p);_I(r)[i]=us(s))I((*p%8)>0)*p+=8-(*p%8);return r)
 // Int list
 I(t==-1,A r=aL(n);MC(_V(r),b+*p,n*sizeof(L));*p+=n*sizeof(L);return r)
 // Float list
 I(t==-2,A r=aF(n);MC(_V(r),b+*p,n*sizeof(F));*p+=n*sizeof(F);return r)
 // Generic list
 I(t==0,A r=aA(n);F(n,_A(r)[i]=rrep(b,len,p))return r)
 // Dictionary
 I(t==5,A k=rrep(b,len,p),v=rrep(b,len,p);return am(k,v))
 // Null
 return _R(cn[ti]);}

// Parse IPC message with header
Z A k3parse(A x,UC*msgtype){
 P(!xtC,et(x))
 P(xn<(N)HDRSZ,el(x))
 CO UC*b=(CO UC*)xV;
 // Parse header
 I(msgtype,*msgtype=b[HDRMSGOFF])
 HDRLEN payloadLen=*(HDRLEN*)(b+HDRLENOFF);
 P(payloadLen<0||xn<HDRSZ+payloadLen,el(x))
 N p=HDRSZ;
 A r=rrep(b,xn,&p);
 mr(x);return r;}

// === Network I/O ===

Z I readn(I fd,V*buf,N n){
 C*p=buf;
 W(n>0,L k=sock_read(fd,p,n);P(k<=0,-1)p+=k;n-=k)
 _(0)}

Z I writen(I fd,CO V*buf,N n){
 CO C*p=buf;
 W(n>0,L k=sock_write(fd,(CO C*)p,n);P(k<=0,-1)p+=k;n-=k)
 _(0)}

Z A k3send(I fd,A x,UC msgtype){
 A msg=k3msg(x,msgtype);
 I r=writen(fd,_V(msg),_n(msg));
 mr(msg);
 P(r<0,eo0())
 au;}

Z A k3recv(I fd){
 UC hdr[HDRSZ];
 P(readn(fd,hdr,HDRSZ)<0,eo0())
 HDRLEN msgLen=*(HDRLEN*)(hdr+HDRLENOFF);
 P(msgLen<0||msgLen>100000000,el0())
 A buf=aC(HDRSZ+msgLen);
 MC(_V(buf),hdr,HDRSZ);
 P(readn(fd,_C(buf)+HDRSZ,msgLen)<0,mr(buf);eo0())
 _n(buf)=HDRSZ+msgLen;
 k3parse(buf,0);}

// === Public API ===

// x 3: y - async send (or serialize if x=0)
A2(v3c,
 P(!xtz,mr(y);et(x))
 I fd=gl(x);
 P(!fd,k3msg(y,MSG_ASYNC))
 k3send(fd,y,MSG_ASYNC))

// x 4: y - sync send/receive (or deserialize if x=0)
A2(v4c,
 P(!xtz,mr(y);et(x))
 I fd=gl(x);
 P(!fd,k3parse(y,0))
 mr(y);
 k3recv(fd))
