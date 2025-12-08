#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
// K3 IPC - Inter-Process Communication protocol
// Compatible with kx systems K3 for network communication between K processes
//
// Usage:
//   fd:<"host:port"   / open connection
//   fd 3: value       / serialize and send (SET message)
//   fd 4: ()          / receive and deserialize response
//   >fd               / close connection
//
// For in-memory serialization:
//   0 3: value        / serialize to bytes
//   0 4: bytes        / deserialize from bytes

#ifdef _WIN32
 #include<winsock2.h>
 // Windows uses recv/send for sockets
 #define sock_read(fd,buf,n) recv(fd,buf,n,0)
 #define sock_write(fd,buf,n) send(fd,buf,n,0)
#else
 #include<unistd.h>
 #include<sys/socket.h>
 #include<errno.h>
 // Unix can use read/write for sockets
 #define sock_read(fd,buf,n) read(fd,buf,n)
 #define sock_write(fd,buf,n) write(fd,buf,n)
#endif

// K3 message types
enum{K3SET=0,K3GET=1,K3RES=2};

// K3 type constants
enum{K3LST=0,K3INT=1,K3FLT=2,K3CHR=3,K3SYM=4,K3DCT=5,K3NUL=6,K3FUN=7};

// Message header: [endian:1][reserved:2][msgtype:1][length:4]
// Chunk header: [type:4][count:4 for lists/strings/dicts]

// === Buffer helpers ===

// Append bytes to buffer, growing if needed
Z V apnd(A*p,CO V*d,N n){A x=*p;
 N c=(HD<<xb)-HD;// capacity from bucket index
 I(xn+n>c,N sz=2*c+n+HD;A y=aC(sz);yn=xn;MC(yV,xV,xn);mr(x);*p=x=y)
 MC(xC+xn,d,n);xn+=n;}

// Append padding to 8-byte boundary
Z V pad8(A*p,N len){N r=(8-(len&7))&7;
 I(r,UC z[8]={0};apnd(p,z,r))}

// Emit int32 in little-endian
Z V emitI(A*p,I v){apnd(p,&v,4);}

// === Serialization (K value to bytes) ===

Z N emitBd(A*,A);

// Emit K value recursively, return byte count written
Z N emitBd(A*p,A x){
 N start=_n(*p);
 C t=_t(x);
 // Integer atom (ti or tl) - packed, no mr needed
 I(t==ti||t==tl,I v=_v(x);emitI(p,K3INT);emitI(p,v);return 8)
 // Float atom - heap allocated
 I(t==tf,emitI(p,K3FLT);emitI(p,1);F v=*xF;apnd(p,&v,8);mr(x);return 16)
 // Char atom (tc) - packed, no mr needed
 I(t==tc,emitI(p,K3CHR);UC v[4]={_v(x),0,0,0};apnd(p,v,4);return 8)
 // Symbol atom (ts) - packed, no mr needed
 I(t==ts,S s=su(xv);N n=SL(s);emitI(p,K3SYM);apnd(p,s,n+1);return 4+n+1)
 // Character list (string)
 I(t==tC,N n=xn;emitI(p,-K3CHR);emitI(p,n);apnd(p,xV,n);UC z=0;apnd(p,&z,1);mr(x);return 8+n+1)
 // Symbol list
 I(t==tS,emitI(p,-K3SYM);emitI(p,xn);N sz=8;F(xn,S s=su(xI[i]);N n=SL(s);apnd(p,s,n+1);sz+=n+1)mr(x);return sz)
 // Generic list
 I(t==tA,emitI(p,K3LST);emitI(p,xn);N sz=8;
    F(xn,N len=emitBd(p,_R(xa));sz+=len;pad8(p,len);sz+=(8-(len&7))&7)
    mr(x);return sz)
 // Dictionary (tM or tm)
 I(t==tM||t==tm,A k=xx,v=xy;emitI(p,K3DCT);N n=_N(k);emitI(p,n);N sz=8;
    F(n,
      A ky=ii(k,i),vy=ii(v,i);
      A e=aA(3);_A(e)[0]=ky;_A(e)[1]=vy;_A(e)[2]=_R(cn[ti]);// null attr
      N len=emitBd(p,e);sz+=len)
    mr(x);return sz)
 // Null/other - use K3NUL
 emitI(p,K3NUL);emitI(p,0);mr(x);return 8;
}

// Create K3 IPC message with header
// msgtype: K3SET=0 (send/set), K3GET=1 (query), K3RES=2 (response)
Z A k3msg(A x,UC msgtype){
 A r=aC(256);_n(r)=0;
 // Message header: [endian:1][reserved:2][msgtype:1][length:4]
 UC hdr[8]={1,0,0,msgtype,0,0,0,0};
 apnd(&r,hdr,8);
 // Emit payload
 N dlen=emitBd(&r,x);
 // Write length (little endian, does not include header)
 *(I*)(_C(r)+4)=dlen;
 return r;}

// === Deserialization (bytes to K value) ===

// Read int32 from buffer
Z I readI(CO UC*b,N*p){I v;MC(&v,b+*p,4);*p+=4;_(v)}

// Read null-terminated string, return pointer
Z S readSym(CO UC*b,N*p){S s=(S)(b+*p);W(b[*p],(*p)++)*p+=1;_(s)}

Z A readDb(CO UC*,N*,B);

// Read K value from buffer
Z A readDb(CO UC*b,N*p,B align){
 I typ=readI(b,p);
 I cnt=0;
 I(typ<=0||typ==K3DCT,cnt=readI(b,p))
 N startPos=*p;
 S(typ,
  C(K3INT,ai(readI(b,p)))
  C(K3FLT,*p+=4;/*skip padding*/F v;MC(&v,b+*p,8);*p+=8;af(v))
  C(K3CHR,UC v=b[*p];I(align,*p+=4)E(*p+=1)ac(v))
  C(-K3CHR,A r=aCn((S)(b+*p),cnt);*p+=cnt+1;
    I(align,N dlen=cnt+1;N pd=(8-(dlen&7))&7;*p+=pd)r)
  C(K3SYM,S s=readSym(b,p);I(align,N dlen=*p-startPos+4;N pd=(8-(dlen&7))&7;*p+=pd)sym(s))
  C(-K3SYM,A r=aS(cnt);F(cnt,S s=readSym(b,p);_I(r)[i]=us(s))r)
  C(K3LST,A r=aA(cnt);F(cnt,_A(r)[i]=readDb(b,p,1))r)
  C(K3DCT,A k=aS(cnt),v=aA(cnt);
    F(cnt,
      A e=readDb(b,p,0);
      P(!_tA(e)||_n(e)<2,mr(k);mrn(i,_A(v));AZ(v);mr(v);mr(e);ed0())
      A ky=_A(e)[0],vy=_A(e)[1];
      I(_ts(ky),_I(k)[i]=_v(ky))E(_I(k)[i]=us(su(_v(ky))))
      _A(v)[i]=_R(vy);
      mr(e))
    am(k,v))
  C(K3NUL,_R(cn[ti]))
  D(ed0()))
}

// Parse K3 IPC message, returns value and optionally msgtype
Z A k3parse(A x,UC*msgtype){
 P(!xtC,et(x))
 P(xn<8,el(x))
 CO UC*b=(CO UC*)xV;
 // Parse header
 // UC endian=b[0]; // 1=little endian
 I(msgtype,*msgtype=b[3])
 // I msgLen=*(I*)(b+4);
 N p=8;
 A r=readDb(b,&p,0);
 mr(x);return r;}

// === Network I/O ===

// Read exactly n bytes from socket, returns 0 on success, -1 on error
Z I readn(I fd,V*buf,N n){
 C*p=buf;
 W(n>0,
   L k=sock_read(fd,p,n);
   P(k<=0,-1)
   p+=k;n-=k)
 _(0)}

// Write exactly n bytes to socket
Z I writen(I fd,CO V*buf,N n){
 CO C*p=buf;
 W(n>0,
   L k=sock_write(fd,(CO C*)p,n);
   P(k<=0,-1)
   p+=k;n-=k)
 _(0)}

// Send K3 IPC message over socket
// fd: file descriptor, x: value to send, msgtype: message type
Z A k3send(I fd,A x,UC msgtype){
 A msg=k3msg(x,msgtype);
 I r=writen(fd,_V(msg),_n(msg));
 mr(msg);
 P(r<0,eo0())
 au;}

// Receive K3 IPC message from socket
// fd: file descriptor, returns value (or error)
Z A k3recv(I fd){
 // Read 8-byte header
 UC hdr[8];
 P(readn(fd,hdr,8)<0,eo0())
 // Parse header
 // UC endian=hdr[0]; // 1=little endian
 // UC msgtype=hdr[3];
 I msgLen=*(I*)(hdr+4);
 P(msgLen<0||msgLen>100000000,el0())// sanity check
 // Read payload
 A buf=aC(8+msgLen);
 MC(_V(buf),hdr,8);
 P(readn(fd,_C(buf)+8,msgLen)<0,mr(buf);eo0())
 _n(buf)=8+msgLen;
 // Parse message
 k3parse(buf,0);}

// === Public API ===

// x 3: y - K3 IPC serialize/send
// If x is 0: serialize y to bytes (in-memory)
// If x is fd (>0): serialize y and send over socket, return ()
// NOTE: Use (0) 3: y not 0 3: y due to parser treating adjacent numbers specially
A2(v3c,
 P(!xtz,y(et(x)))// type error if x is not integer, must free y too
 I fd=gl(x);// gl() consumes x
 P(!fd,k3msg(y,K3SET))// mode 0: just serialize
 // mode fd: send over socket
 k3send(fd,y,K3SET))

// x 4: y - K3 IPC receive/deserialize
// If x is 0: deserialize y from bytes
// If x is fd (>0): receive from socket and deserialize (y ignored)
A2(v4c,
 P(!xtz,y(et(x)))// type error if x is not integer, must free y too
 I fd=gl(x);// gl() consumes x
 P(!fd,k3parse(y,0))// mode 0: just deserialize
 // mode fd: receive from socket
 mr(y);
 k3recv(fd))
