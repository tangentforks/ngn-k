#ifdef _WIN32
 #include"a.h" // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
 #include<winsock2.h>
 #include<ws2tcpip.h>
 #include<fcntl.h>
 #include<sys/stat.h>
 #include<stdio.h>
 #define RTLD_LAZY 0
 #ifndef SEEK_SET
  #define SEEK_SET 0
  #define SEEK_CUR 1
  #define SEEK_END 2
 #endif
 #ifndef TCP_NODELAY
  #define TCP_NODELAY 1
 #endif
 #ifndef IPPROTO_TCP
  #define IPPROTO_TCP 6
 #endif
 #ifndef AF_INET
  #define AF_INET 2
 #endif
 #ifndef SOCK_STREAM
  #define SOCK_STREAM 1
 #endif
 Z V*dlopen(S s,I m){(void)m;_(LoadLibraryA(s))}
 Z V*dlsym(V*h,S s)_(GetProcAddress(h,s))
 Z I dlclose(V*h)_(FreeLibrary(h)?0:-1)
 Z C w_dlerr[64];Z C*dlerror(){FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,w_dlerr,SZ w_dlerr,0);w_dlerr;}
 #define PROT_READ 1
 #define PROT_WRITE 2
 #define MAP_SHARED 1
 Z V*w_mmap_file(V*a,N n,I pr,I fl,I f,long o){(void)a;(void)fl;
  DWORD pg=(pr&PROT_WRITE)?PAGE_READWRITE:PAGE_READONLY;
  DWORD fm=(pr&PROT_WRITE)?FILE_MAP_ALL_ACCESS:FILE_MAP_READ;
  HANDLE h=CreateFileMapping((HANDLE)_get_osfhandle(f),0,pg,0,n,0);P(!h,(V*)-1)
  V*p=MapViewOfFile(h,fm,0,o,n);CloseHandle(h);_(p?p:(V*)-1)}
 Z I w_munmap_file(V*p,N n){(void)n;_(UnmapViewOfFile(p)?0:-1)}
 #define mmap w_mmap_file
 #define munmap w_munmap_file
 //directory listing stub - Windows can't open directories as fds
 TD ST{I d;}DIR;ST dirent{C d_name[260];};
 Z DIR*fdopendir(I f){(void)f;_((DIR*)0)}
 Z ST dirent*readdir(DIR*d){(void)d;_((ST dirent*)0)}
 Z I closedir(DIR*d){(void)d;_(0)}
 //time
 Z L w_now(){FILETIME ft;GetSystemTimeAsFileTime(&ft);L t=((L)ft.dwHighDateTime<<32)|ft.dwLowDateTime;_((t-116444736000000000LL)/10)}
 //subprocess with CreateProcess
 Z A w_frk(A x){P(!_tA(x)||_n(x)-2,et(x))
  A y=kv(&x);x=Ny(e1f(str0,x));
  //build command line
  C cmd[4096]={0};C*p=cmd;F(xn,S s=_V(_A(x)[i]);W(*s,*p++=*s++)I(i<xn-1,*p++=' '))
  //create pipes
  SECURITY_ATTRIBUTES sa={SZ(SECURITY_ATTRIBUTES),0,1};
  HANDLE si_r,si_w,so_r,so_w;
  CreatePipe(&si_r,&si_w,&sa,0);CreatePipe(&so_r,&so_w,&sa,0);
  SetHandleInformation(si_w,HANDLE_FLAG_INHERIT,0);
  SetHandleInformation(so_r,HANDLE_FLAG_INHERIT,0);
  STARTUPINFOA sui={SZ(STARTUPINFOA)};sui.dwFlags=STARTF_USESTDHANDLES;
  sui.hStdInput=si_r;sui.hStdOutput=so_w;sui.hStdError=so_w;
  PROCESS_INFORMATION pi;
  P(!CreateProcessA(0,cmd,0,0,1,0,0,0,&sui,&pi),CloseHandle(si_r);CloseHandle(si_w);CloseHandle(so_r);CloseHandle(so_w);x(y(eo0())))
  CloseHandle(si_r);CloseHandle(so_w);
  //write input
  DWORD w;WriteFile(si_w,_V(y),_n(y),&w,0);CloseHandle(si_w);y(0);
  //read output
  A o=aC(256-HD);L m=0;DWORD k;
  W(ReadFile(so_r,_V(o)+m,_n(o)-m,&k,0)&&k,m+=k;I(m+1000>_n(o),A z=aC(2*_n(o)+HD);MC(_V(z),_V(o),m);mr(o);o=z))
  CloseHandle(so_r);WaitForSingleObject(pi.hProcess,INFINITE);
  CloseHandle(pi.hProcess);CloseHandle(pi.hThread);_(x(AN(m,o)))}
#else
 #include<dlfcn.h> // ngn/k, (c) 2019-2024 ngn, GNU AGPLv3 - https://codeberg.org/ngn/k/raw/branch/master/LICENSE
 #include<sys/socket.h>
 #include<sys/wait.h>
 #include<netinet/in.h>
 #include<netinet/tcp.h>
 #include<fcntl.h>
 #include<arpa/inet.h>
 #include<unistd.h>
 #include<sys/time.h>
 #undef __USE_EXTERN_INLINES
 #include<sys/stat.h>
 #include<sys/mman.h>
 #include<dirent.h>
 #include"a.h"
#endif
Z U addr(S*p)_(S s=*p;P(!*s,0x0100007f)UC v[4];F(4,I(i,P(*s-'.',ed0())s++)v[i]=pu(&s);P(v[i]>255,ed0()))*p=s;*(U*)v)
Z I skt(U h,UH p)_(I f=socket(AF_INET,SOCK_STREAM,0);P(f<0,eo0())I v=setsockopt(f,IPPROTO_TCP,TCP_NODELAY,(CO C*)(I[]){1},4);P(v<0,eo0())
ST sockaddr_in a;a.sin_family=AF_INET;a.sin_addr.s_addr=h;a.sin_port=(UH)(p<<8|p>>8);P(connect(f,(ST sockaddr*)&a,SZ a)<0,eo0())f)
Z I osf(S s,L fl)_(P(!strchr(s,':'),I f=open(s,fl,0666);P(f<3/*fbsd*/,eo0())f)U h=addr(&s);P(*s-':',ed0())s++;W p=pu(&s);P(*s,ed0())skt(h,p))
Z I o(A x/*1*/,I fl)_(Xz(gl(x))Xs(xv?osf(su(xv),fl):(fl&3)?1:0)XC(x=str0(x);I v;Mx(v=osf(xV,fl));v)et(x))
Z I fm(I f)_(ST stat s;fstat(f,&s)<0?0:s.st_mode)                                                                                                                 // get file mode
Z A frd(I f,N i,N n)_(P(i||n+1,en0())DIR*a=fdopendir(f);P(!a,ei0())A x=emp(tC);ST dirent*e;W((e=readdir(a)),S s=e->d_name;x=apc(cts(x,s,SL(s)),10))closedir(a);x) // read dir
Z A frS(I f,N n)_(C b[1024];I m=fm(f);B r=S_ISREG(m);A x=emp(tC);I k=1;W(n&&k,k=read(f,b,MIN(SZ b,n));P(k<0,eo(x))n-=k;x=cts(x,b,k);P((!r&&-1==n+k||f<3)&&k-SZ b,x))x)                                    // read stream (only length)
Z A frs(I f,N i,N n)_(I(i&&lseek(f,i,SEEK_CUR)<0,mr(N(frS(f,i))))frS(f,n))                                                                                        // read stream (offset too)
Z A frm(I f,N i,N n)_(L m=lseek(f,0,SEEK_END);P(m<0,eo0())n=MIN(n,MAX(m-i,0));n?mf(f,i,n):emp(tC))                                                                // read through mmap
A fr(A x/*1*/,N i,N n)_(Xz(frs(gl(x),i,n))P(xts&&!xv,frs(0,i,n))I f=N(o(x,O_RDONLY));f=f-1?f:0;P(f<3,frs(f,i,n))I m=fm(f);x=(S_ISDIR(m)?frd:S_ISREG(m)?frm:frs)(f,i,n);close(f);x)              // read
Z A fws(I f,S s,N n)_(W(n>0,L k=write(f,s,n);P(k<0,eo0())P(!k,au)s+=k;n-=k)au)                                                                                    // write stream
Z A fwm(I f,S s,N n)_(ftruncate(f,n);V*p=mmap(0,n,PROT_READ|PROT_WRITE,MAP_SHARED,f,0);MC(p,s,n);munmap(p,n);au)                                                  // write through mmap
Z X2(fw,Ril(I f=gl_(x);My(x=(f<3||!S_ISREG(fm(f))?fws:fwm)(f,yV,yn))x)R_(I f=N(o(xR,O_RDWR|O_CREAT|O_TRUNC));A z=v1c(ai(f),y);f>2&&close(f);z))                   // write
ZN A dle()_(C*e=dlerror();I(e,os(e);os("\n"))eo0())
A1(opn,Xz(x)I f=N(o(x,O_RDWR|O_CREAT));ai(f-1?f:0))                                                                                     // <s
A cls(L n)_(close(n);au)                                                                                                    // >i
A1(u0c,spl(N(u1c(x))))                                                                                                      // 0:x
X1(u1c,RA(P(xn-2,el(x))P(!_tZ(xy),et(x))P(_n(xy)-2,el(x))A y=kv(&x);N i=gl(ii(y,0)),n=gl(ii(y,1));fr(x,i,n))R_(fr(x,0,-1))) // 1:x
A1(u2c,en(x))                                                                                                               // 2:x
Y2(v0c,RA(v0c(x,N(jc(10,y))))RC(v1c(x,apc(y,10)))R_(et(y)))                                                                 // x 0:y
Y2(v1c,RC(fw(x,y))R_(et(y)))                                                                                                // x 1:y
A2(v2c,P(!xts||!ytA,et(y))P(yn-2,el(y))P(!_ts(yx)||!_ti(yy),et(y))I k=_v(yy);y(0);P(!k||k>8u,ed0())                         // x 2:y
 V*l=dlopen(su(xv),RTLD_LAZY);P(!l,dle())V*f=dlsym(l,su(_v(yx)));P(!f,dle())ax(f,k))

Z A rda(I f)_(A x=aC(256-HD);L m=0,k;W((k=read(f,xV+m,xn-m))>0,m+=k;I(m+1000000>xn&&2*m>xn,A y=aC(2*xn+HD);MC(yV,xV,m);x=x(y)))close(f);AN(m,x))
Z I lC(A x)_(XA(F(xn,P(_t(xa)-tC,0))1)0)//list of strings?
#ifdef _WIN32
A1(frk,w_frk(x))
L now()_(w_now())
#else
A1(frk,P(!xtA||xn-2,et(x))A y=kv(&x);P(!lC(x)||!ytC,y(ed(x)))x=Ny(e1f(str0,x));S a[xn+1];F(xn,a[i]=_V(xa))a[xn]=0;I p[4];pipe(p);pipe(p+2);I pid=fork();
 P(!pid,dup2(*p,0);dup2(p[3],1);F(4,close(p[i]))exit(execve(*a,(C**)a,(C*CO*)env));0)close(*p);close(p[3]);N(v1c(ai(p[1]),x(y)));close(p[1]);A x=rda(p[2]);wait4(pid,0,0,0);x)
L now()_(ST timeval t;gettimeofday(&t,0);1000000ll*t.tv_sec+t.tv_usec)
#endif
