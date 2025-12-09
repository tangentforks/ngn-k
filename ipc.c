#include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
// Kona-compatible IPC - Inter-Process Communication protocol
// Wire format compatible with Kona (https://github.com/kevinlawler/kona)
//
// In-memory serialization:
//   ` 3: value        / serialize to bytes
//   ` 4: bytes        / deserialize from bytes
//
// Client usage:
//   (`host;port) 3: value  / connect, send async message (fire-and-forget)
//   (`host;port) 4: value  / connect, send sync message, wait for response
//   (`;1234) 3: value      / empty host = localhost
//   ("";1234) 3: value     / string host also works
//
// Server usage (command line):
//   ./k -l 1234        / start listening on port 1234
//   .m.s:{[x] ...}     / define async message handler (3: messages)
//   .m.g:{[x] ...}     / define sync handler (4: messages, return value sent back)
//
// Connections are cached - same (host;port) reuses existing socket

#ifdef _WIN32
 #include<winsock2.h>
 #include<ws2tcpip.h>
 #define sock_read(fd,buf,n) recv(fd,buf,n,0)
 #define sock_write(fd,buf,n) send(fd,buf,n,0)
 #define sock_close closesocket
#else
 #include<unistd.h>
 #include<sys/socket.h>
 #include<netinet/in.h>
 #include<netinet/tcp.h>
 #include<errno.h>
 #include<poll.h>
 #define sock_read(fd,buf,n) read(fd,buf,n)
 #define sock_write(fd,buf,n) write(fd,buf,n)
 #define sock_close close
#endif

#ifdef _WIN32
// Initialize Winsock (call before any socket operations)
Z V wsa_init(){Z WSADATA wsa;Z B init;I(!init,init=1;WSAStartup(MAKEWORD(2,2),&wsa))}
#else
#define wsa_init()
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
 I(t==tE,return rep(gZ(_R(x))))// enumeration - expand to calculate size
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

 // Enumeration (range) - expand to actual list before serializing
 I(t==tE,return wrep(p,gZ(x)))// gZ expands and consumes x

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

Z A k3send(I fd,A x,UC msgtype)_(
 A msg=k3msg(x,msgtype);
 I r=writen(fd,_V(msg),_n(msg));
 mr(msg);
 P(r<0,eo0())
 au)

Z A k3recv(I fd)_(
 UC hdr[HDRSZ];
 P(readn(fd,hdr,HDRSZ)<0,eo0())
 HDRLEN msgLen=*(HDRLEN*)(hdr+HDRLENOFF);
 P(msgLen<0||msgLen>100000000,el0())
 A buf=aC(HDRSZ+msgLen);
 MC(_V(buf),hdr,HDRSZ);
 P(readn(fd,_C(buf)+HDRSZ,msgLen)<0,mr(buf);eo0())
 _n(buf)=HDRSZ+msgLen;
 k3parse(buf,0))

// === Connection Table ===
// Maps (host,port) pairs to socket file descriptors

#define MAX_CONNS 64
Z struct{U host;UH port;I fd;}ipc_conns[MAX_CONNS];
Z I ipc_nconn=0;

// Find or create connection for host:port
// Returns fd or -1 on error
Z I ipc_connect(U host,UH port){
 // Create new connection (no caching for now)
 wsa_init();
 I fd=socket(AF_INET,SOCK_STREAM,0);
#ifdef _WIN32
 P(fd==INVALID_SOCKET,-1)
 I yes=1;setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(CO C*)&yes,sizeof(yes));
 ST sockaddr_in a;MS(&a,0,sizeof(a));
 a.sin_family=AF_INET;
 a.sin_addr.s_addr=host;
 a.sin_port=htons(port);
 I(connect(fd,(ST sockaddr*)&a,sizeof(a))==SOCKET_ERROR,sock_close(fd);return -1)
#else
 P(fd<0,-1)
 I yes=1;setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(CO C*)&yes,sizeof(yes));
 ST sockaddr_in a;MS(&a,0,sizeof(a));
 a.sin_family=AF_INET;
 a.sin_addr.s_addr=host;
 a.sin_port=htons(port);
 I(connect(fd,(ST sockaddr*)&a,sizeof(a))<0,sock_close(fd);return -1)
#endif
 _(fd)}

// Close connection and remove from table
Z V ipc_disconnect(I fd){
 F(ipc_nconn,I(ipc_conns[i].fd==fd,sock_close(fd);ipc_conns[i]=ipc_conns[--ipc_nconn];return))}

// Parse host from symbol or string, returns network byte order address
// Empty/null = localhost (127.0.0.1), 0 = parse error
Z U ipc_addr(A x){
 S s=0;
 I(_t(x)==ts,s=su(_v(x)))
 else I(_t(x)==tC,I(!_n(x),return 0x0100007f)s=(S)_V(x))
 else return 0;
 I(!*s,return 0x0100007f)// empty = localhost
 UC v[4];
 F(4,I(i,I(*s!='.',return 0)s++)v[i]=pu(&s);I(v[i]>255,return 0))
 return *(U*)v;}

// === Public API ===

// x 3: y - async send (or serialize if x=`)
// x can be:
//   ` (empty symbol) - serialize y to bytes (no send)
//   (host;port) - connect to host:port and send y async
//   fd (integer) - send y async on existing fd
A2(v3c,
 // Empty symbol `: serialize-only
 I(xts&&!xv,return k3msg(y,MSG_ASYNC))
 // Integer x: fd for send
 I(xtz,I fd=gl(x);return k3send(fd,y,MSG_ASYNC))
 // List x: (host;port)
 P(!xtA||xn!=2,mr(y);et(x))
 A h0=xx,p0=xy;  // get refs (don't modify refcounts - elements are owned by list)
 P(!_tz(p0),mr(y);et(x))
 U host=ipc_addr(h0);
 P(!host&&_N(h0),mr(y);ed(x))// invalid host
 I(!host,host=0x0100007f)// default localhost
 UH port=gl_(p0);  // get value WITHOUT freeing (gl_ vs gl)
 I fd=ipc_connect(host,port);
 P(fd<0,eo0())
 A r=k3send(fd,y,MSG_ASYNC);
 sock_close(fd);
 r)
 // NOTE: x is NOT freed here - ngn/k evaluator handles argument cleanup

// x 4: y - sync send/receive (or deserialize if x=`)
// x can be:
//   ` (empty symbol) - deserialize y from bytes
//   (host;port) - connect to host:port, send y sync, receive response
//   fd (integer) - send y sync on existing fd, receive response
A2(v4c,
 // Empty symbol `: deserialize-only
 I(xts&&!xv,return k3parse(y,0))
 // Integer x: fd for receive
 I(xtz,I fd=gl(x);mr(y);return k3recv(fd))
 // List x: (host;port)
 P(!xtA||xn!=2,mr(y);et(x))
 A h0=xx,p0=xy;  // get refs (don't modify refcounts - elements are owned by list)
 P(!_tz(p0),mr(y);et(x))
 U host=ipc_addr(h0);
 P(!host&&_N(h0),mr(y);ed(x))// invalid host
 I(!host,host=0x0100007f)// default localhost
 UH port=gl_(p0);  // get value WITHOUT freeing (gl_ vs gl)
 I fd=ipc_connect(host,port);
 P(fd<0,eo0())
 A msg=k3msg(y,MSG_SYNC);
 I r=writen(fd,_V(msg),_n(msg));
 mr(msg);
 P(r<0,eo0())
 k3recv(fd))
 // NOTE: x is NOT freed here - ngn/k evaluator handles argument cleanup

// === Server functionality ===

// Create listening socket bound to port
Z I ipc_listen(I port){
 I fd=socket(AF_INET,SOCK_STREAM,0);
#ifdef _WIN32
 P(fd==INVALID_SOCKET,-1)
 I yes=1;setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(CO C*)&yes,sizeof(yes));
 ST sockaddr_in a;MS(&a,0,sizeof(a));
 a.sin_family=AF_INET;
 a.sin_addr.s_addr=INADDR_ANY;
 a.sin_port=htons(port);
 I(bind(fd,(ST sockaddr*)&a,sizeof(a))==SOCKET_ERROR,sock_close(fd);return -1)
 I(listen(fd,16)==SOCKET_ERROR,sock_close(fd);return -1)
#else
 P(fd<0,-1)
 I yes=1;setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(CO C*)&yes,sizeof(yes));
 ST sockaddr_in a;MS(&a,0,sizeof(a));
 a.sin_family=AF_INET;
 a.sin_addr.s_addr=INADDR_ANY;
 a.sin_port=htons(port);
 I(bind(fd,(ST sockaddr*)&a,sizeof(a))<0,sock_close(fd);return -1)
 I(listen(fd,16)<0,sock_close(fd);return -1)
#endif
 _(fd)}

// Accept connection on listening socket
Z I ipc_accept(I listener){
 ST sockaddr_in a;
 socklen_t len=sizeof(a);
 I fd=accept(listener,(ST sockaddr*)&a,&len);
#ifdef _WIN32
 P(fd==INVALID_SOCKET,-1)
#else
 P(fd<0,-1)
#endif
 I yes=1;setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(CO C*)&yes,sizeof(yes));
 _(fd)}

// Poll fd for readability, returns 1 if ready, 0 if timeout, -1 on error
Z I ipc_poll(I fd,I timeout_ms){
#ifdef _WIN32
 fd_set r;FD_ZERO(&r);FD_SET(fd,&r);
 ST timeval tv;tv.tv_sec=timeout_ms/1000;tv.tv_usec=(timeout_ms%1000)*1000;
 _(select(fd+1,&r,0,0,&tv))
#else
 ST pollfd pfd;pfd.fd=fd;pfd.events=POLLIN;
 _(poll(&pfd,1,timeout_ms))
#endif
}

// Look up handler in .m namespace using gg() (get global)
// name: 's' for .m.s (async), 'g' for .m.g (sync)
// Returns handler function or 0 if not defined
Z A ipc_handler(C name){
 // Build symbol list `m`<name> for .m.<name>
 C nm[2]={name,0};
 A path=aS(2);_I(path)[0]=us("m");_I(path)[1]=us(nm);
 A h=gg(path);// gg() consumes path, returns 0 if not found
 _(h)}

// Receive and dispatch message with handler callbacks
Z A ipc_recv_dispatch(I fd){
 UC hdr[HDRSZ];
 P(readn(fd,hdr,HDRSZ)<0,eo0())
 UC msgtype=hdr[HDRMSGOFF];
 HDRLEN msgLen=*(HDRLEN*)(hdr+HDRLENOFF);
 P(msgLen<0||msgLen>100000000,el0())
 A buf=aC(HDRSZ+msgLen);
 MC(_V(buf),hdr,HDRSZ);
 P(readn(fd,_C(buf)+HDRSZ,msgLen)<0,mr(buf);eo0())
 _n(buf)=HDRSZ+msgLen;
 A val=k3parse(buf,0);
 P(!val,val)
 // Dispatch based on message type
 I(msgtype==MSG_RESP,return val)// response - just return value
 // Look up handler
 A h=ipc_handler(msgtype==MSG_ASYNC?'s':'g');
 A r;
 I(h,r=dot(h,enl(val)))// call handler with value
 J(_t(val)==tC,val=str0(val);r=evs(_V(val),0);mr(val);I(!r,r=au))// string: evaluate as K code
 E(mr(val);r=au)// anything else: return null
 // For sync messages, send response (k3send consumes r via wrep)
 I(msgtype==MSG_SYNC,k3send(fd,r,MSG_RESP);return au)
 _(r)}

// === Server mode (for -l command line option) ===
// On Windows, stdin can't be mixed with sockets in select().
// Solution: REPL runs in a separate thread, sends lines via a socket pair.

#define MAX_CLIENTS 64
Z I ipc_listener=-1;           // listener socket fd
Z I ipc_clients[MAX_CLIENTS];  // client socket fds
Z I ipc_nclient=0;             // number of active clients

#ifdef _WIN32
#include<process.h>
Z I repl_sock[2]={-1,-1};      // socket pair: [0]=main reads, [1]=repl writes

// Create a socket pair (Windows doesn't have socketpair)
Z I make_sockpair(I sv[2]){
 I ls=socket(AF_INET,SOCK_STREAM,0);P(ls==INVALID_SOCKET,-1)
 ST sockaddr_in a;MS(&a,0,sizeof(a));
 a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(0x7f000001);a.sin_port=0;
 P(bind(ls,(ST sockaddr*)&a,sizeof(a))==SOCKET_ERROR,sock_close(ls);-1)
 socklen_t len=sizeof(a);getsockname(ls,(ST sockaddr*)&a,&len);
 P(listen(ls,1)==SOCKET_ERROR,sock_close(ls);-1)
 sv[1]=socket(AF_INET,SOCK_STREAM,0);
 P(sv[1]==INVALID_SOCKET,sock_close(ls);-1)
 P(connect(sv[1],(ST sockaddr*)&a,sizeof(a))==SOCKET_ERROR,sock_close(ls);sock_close(sv[1]);-1)
 sv[0]=accept(ls,0,0);sock_close(ls);
 P(sv[0]==INVALID_SOCKET,sock_close(sv[1]);-1)
 _(0)}

// REPL thread: reads stdin, sends lines to main thread
Z V __cdecl repl_thread(V*arg){
 C buf[4096];
 W(1,
   I n=read(0,buf,sizeof(buf));
   I(n<=0,break)
   send(repl_sock[1],buf,n,0))
 sock_close(repl_sock[1]);repl_sock[1]=-1;}
#endif

// Start listening on port (called from main with -l option)
I ipc_start(I port){
 wsa_init();
 I fd=ipc_listen(port);
 P(fd<0,-1)
 ipc_listener=fd;
#ifdef _WIN32
 // Create socket pair and start REPL thread
 I(make_sockpair(repl_sock)<0,repl_sock[0]=repl_sock[1]=-1)// continue without repl thread if sockpair fails
 E(_beginthread(repl_thread,0,0))
#endif
 _(0)}

// Get fd to read REPL input from (socket on Windows server mode, -1 otherwise)
I ipc_repl_fd(){
#ifdef _WIN32
 _(repl_sock[0])
#else
 _(-1)
#endif
}

// Read from REPL fd (use recv on Windows since it's a socket)
L ipc_repl_read(V*buf,N n){
#ifdef _WIN32
 I fd=repl_sock[0];
 P(fd<0,-1)
 _(recv(fd,buf,n,0))
#else
 _(-1)
#endif
}

// Read from stdin - handles IPC while waiting in server mode
L ipc_stdin_read(V*buf,N n){
#ifdef _WIN32
 I(repl_sock[0]>=0,_(recv(repl_sock[0],buf,n,0)))
#else
 // In server mode, poll stdin + IPC and handle IPC while waiting
 I(ipc_listener>=0,
   W(1,
     ST pollfd pfd[2+MAX_CLIENTS];
     pfd[0].fd=0;pfd[0].events=POLLIN;// stdin
     pfd[1].fd=ipc_listener;pfd[1].events=POLLIN;
     F(ipc_nclient,pfd[2+i].fd=ipc_clients[i];pfd[2+i].events=POLLIN)
     I r=poll(pfd,2+ipc_nclient,-1);// blocking wait
     P(r<0,-1)
     // Handle new connections
     I(pfd[1].revents&POLLIN,
       I fd=ipc_accept(ipc_listener);
       I(fd>=0&&ipc_nclient<MAX_CLIENTS,ipc_clients[ipc_nclient++]=fd))
     // Handle client messages
     F(ipc_nclient,
       I(pfd[2+i].revents&(POLLIN|POLLHUP|POLLERR),
         A v=ipc_recv_dispatch(ipc_clients[i]);
         I(!v,sock_close(ipc_clients[i]);ipc_clients[i]=ipc_clients[--ipc_nclient])// no i-- (pfd stale)
         E(mr(v))))
     // If stdin ready, break to read
     I(pfd[0].revents&POLLIN,break)))
#endif
 _(read(0,buf,n))}

// Flag to disable stdin in server mode (set when stdin returns EOF)
Z B ipc_no_stdin=0;

// Called from REPL to check for IPC activity
// Returns: 0=stdin has data, 1=IPC handled (continue), -1=error
I ipc_check(){
 P(ipc_listener<0,0)// not in server mode
#ifdef _WIN32
 // On Windows, monitor sockets only (stdin comes via repl_sock[0])
 fd_set r;FD_ZERO(&r);
 FD_SET(ipc_listener,&r);
 I(repl_sock[0]>=0,FD_SET(repl_sock[0],&r))
 I maxfd=ipc_listener;
 I(repl_sock[0]>maxfd,maxfd=repl_sock[0])
 F(ipc_nclient,FD_SET(ipc_clients[i],&r);I(ipc_clients[i]>maxfd,maxfd=ipc_clients[i]))
 ST timeval tv={0,50000};// 50ms timeout
 I n=select(maxfd+1,&r,0,0,&tv);
 P(n<0,-1)
 // Check listener for new connections
 I(n>0&&FD_ISSET(ipc_listener,&r),
   I fd=ipc_accept(ipc_listener);
   I(fd>=0&&ipc_nclient<MAX_CLIENTS,ipc_clients[ipc_nclient++]=fd))
 // Check clients for incoming data
 F(ipc_nclient,
   I(n>0&&FD_ISSET(ipc_clients[i],&r),
     A v=ipc_recv_dispatch(ipc_clients[i]);
     I(!v,sock_close(ipc_clients[i]);ipc_clients[i]=ipc_clients[--ipc_nclient])// connection closed (no i-- due to stale fd_set)
     E(mr(v))))// value handled by .m.s/.m.g
 // Check if stdin data available (via repl socket)
 _(n>0&&repl_sock[0]>=0&&FD_ISSET(repl_sock[0],&r)?0:1)
#else
 ST pollfd pfd[2+MAX_CLIENTS];
 I nfd=0;
 I stdin_idx=-1;
 I(isatty(0)&&!ipc_no_stdin,stdin_idx=nfd;pfd[nfd].fd=0;pfd[nfd++].events=POLLIN)// stdin only if tty
 I listener_idx=nfd;pfd[nfd].fd=ipc_listener;pfd[nfd++].events=POLLIN;
 I client_base=nfd;
 F(ipc_nclient,pfd[nfd].fd=ipc_clients[i];pfd[nfd++].events=POLLIN)
 I n=poll(pfd,nfd,-1);// blocking wait until something ready
 P(n<0,-1)
 // Check listener for new connections
 I(pfd[listener_idx].revents&POLLIN,
   I fd=ipc_accept(ipc_listener);
   I(fd>=0&&ipc_nclient<MAX_CLIENTS,ipc_clients[ipc_nclient++]=fd))
 // Check clients for incoming data
 F(ipc_nclient,
   I(pfd[client_base+i].revents&(POLLIN|POLLHUP|POLLERR),
     A v=ipc_recv_dispatch(ipc_clients[i]);
     I(!v,sock_close(ipc_clients[i]);ipc_clients[i]=ipc_clients[--ipc_nclient])// remove (no i--, pfd stale)
     E(mr(v))))
 // Return whether stdin is ready (or always 1 if stdin disabled)
 _(stdin_idx>=0&&pfd[stdin_idx].revents&POLLIN?0:1)
#endif
}
