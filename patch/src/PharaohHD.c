/* Pharaoh Ascent 1.4: process-local Win32 GDI compatibility adapter.
 * Build as x86 DLL. See BUILD.txt and patch.py for the exact supported executable.
 * Logical canvas: 1024x768. Output: aspect-preserving window / borderless fullscreen.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>
#define LW 1024
#define LH 768
static HWND mainwin;
static int ow=1920,oh=1080,fullscreen=0,vx,vy,vw=1440,vh=1080;
static char ini[MAX_PATH];
typedef struct {HWND win;HWND parent;WNDPROC proc;HDC dc;HBITMAP bmp;int x,y,w,h,native;} SURFACE;
static FILE *logfile;
static SURFACE surfaces[128];static int count=0;
static SURFACE *find(HWND w){if(!w)return NULL;for(int i=0;i<count;i++)if(surfaces[i].win==w)return &surfaces[i];return NULL;}
static SURFACE *control(HWND w){SURFACE*s=find(w);if(s)return s;HWND parent=GetParent(w);if(!find(parent)||!(GetWindowLongA(w,GWL_STYLE)&WS_CHILD))return NULL;int slot;for(slot=0;slot<count;slot++)if(!surfaces[slot].win)break;if(slot>=128)return NULL;if(slot==count)count++;s=&surfaces[slot];memset(s,0,sizeof(*s));RECT r;GetWindowRect(w,&r);POINT pt={r.left,r.top};ScreenToClient(parent,&pt);s->win=w;s->parent=parent;s->native=1;s->x=pt.x;s->y=pt.y;s->w=r.right-r.left;s->h=r.bottom-r.top;return s;}
static SURFACE *bydc(HDC d){if(!d)return NULL;for(int i=0;i<count;i++)if(surfaces[i].win&&surfaces[i].dc==d)return &surfaces[i];return NULL;}
static void layout(void){RECT r;GetClientRect(mainwin,&r);vw=r.right;vh=vw*3/4;if(vh>r.bottom){vh=r.bottom;vw=vh*4/3;}vx=(r.right-vw)/2;vy=(r.bottom-vh)/2;}
static void allocdc(SURFACE *s){if(s->dc){DeleteDC(s->dc);DeleteObject(s->bmp);}HDC screen=GetDC(NULL);s->dc=CreateCompatibleDC(screen);s->bmp=CreateCompatibleBitmap(screen,s->w>0?s->w:1,s->h>0?s->h:1);SelectObject(s->dc,s->bmp);ReleaseDC(NULL,screen);PatBlt(s->dc,0,0,s->w,s->h,BLACKNESS);}
static void present(SURFACE *s){if(!s||s->native||!IsWindow(s->win)||IsIconic(mainwin))return;RECT r;GetClientRect(s->win,&r);HDC d=GetDC(s->win);int x=0,y=0,w=r.right,h=r.bottom;if(s->win==mainwin){layout();PatBlt(d,0,0,r.right,r.bottom,BLACKNESS);x=vx;y=vy;w=vw;h=vh;}SetStretchBltMode(d,COLORONCOLOR);StretchBlt(d,x,y,w,h,s->dc,0,0,s->w,s->h,SRCCOPY);ReleaseDC(s->win,d);}
static void placechild(SURFACE *s){if(!s->win||s->win==mainwin)return;int x=s->x*vw/LW,y=s->y*vh/LH,w=s->w*vw/LW,h=s->h*vh/LH;if(s->parent==mainwin){x+=vx;y+=vy;}MoveWindow(s->win,x,y,w,h,TRUE);}
static void place(void){int sw=GetSystemMetrics(SM_CXSCREEN),sh=GetSystemMetrics(SM_CYSCREEN);int w=fullscreen?sw:ow,h=fullscreen?sh:oh;if(w>sw)w=sw;if(h>sh)h=sh;SetWindowPos(mainwin,HWND_TOP,(sw-w)/2,(sh-h)/2,w,h,SWP_FRAMECHANGED|SWP_SHOWWINDOW);layout();if(logfile){RECT z;GetWindowRect(mainwin,&z);fprintf(logfile,"place full=%d rect=%ld,%ld,%ld,%ld viewport=%d,%d,%d,%d\n",fullscreen,z.left,z.top,z.right,z.bottom,vx,vy,vw,vh);fflush(logfile);}for(int i=0;i<count;i++)placechild(&surfaces[i]);}
static LRESULT CALLBACK proc(HWND h,UINT m,WPARAM w,LPARAM l){SURFACE *s=find(h);if(logfile&&(m==WM_KEYDOWN||m==WM_SYSKEYDOWN||m==WM_SETFOCUS||m==WM_KILLFOCUS)){fprintf(logfile,"msg %p %x %x\n",h,m,w);fflush(logfile);}if(!s)return DefWindowProcA(h,m,w,l);if((m==WM_KEYDOWN&&w==VK_F11)||(m==WM_SYSKEYDOWN&&w==VK_RETURN)){fullscreen=!fullscreen;place();return 0;}if(m==WM_KEYDOWN&&w==VK_F10){ShowWindow(mainwin,SW_MINIMIZE);return 0;}
 if(m>=WM_MOUSEFIRST&&m<=WM_MOUSELAST&&m!=WM_MOUSEWHEEL&&m!=0x020e){int x=(short)LOWORD(l),y=(short)HIWORD(l);if(h==mainwin){x-=vx;y-=vy;}x=x*LW/vw;y=y*LH/vh;l=MAKELPARAM(x,y);}
 if(m==WM_SIZE&&w!=SIZE_MINIMIZED)l=MAKELPARAM(s->w,s->h);
 if(m==WM_DRAWITEM&&l){DRAWITEMSTRUCT *original=(DRAWITEMSTRUCT*)l;SURFACE *b=find(original->hwndItem);if(b&&b->native&&b->w>0&&b->h>0){DRAWITEMSTRUCT copy=*original;HDC buffer=CreateCompatibleDC(original->hDC);HBITMAP bitmap=CreateCompatibleBitmap(original->hDC,b->w,b->h);SelectObject(buffer,bitmap);copy.hDC=buffer;SetRect(&copy.rcItem,0,0,b->w,b->h);LRESULT result=CallWindowProcA(s->proc,h,m,w,(LPARAM)&copy);SetStretchBltMode(original->hDC,COLORONCOLOR);StretchBlt(original->hDC,original->rcItem.left,original->rcItem.top,original->rcItem.right-original->rcItem.left,original->rcItem.bottom-original->rcItem.top,buffer,0,0,b->w,b->h,SRCCOPY);DeleteDC(buffer);DeleteObject(bitmap);return result;}}
 LRESULT r=CallWindowProcA(s->proc,h,m,w,l);if(m==WM_PAINT)present(s);return r;}
static HWND WINAPI hCreate(DWORD ex,LPCSTR cls,LPCSTR title,DWORD style,int x,int y,int w,int h,HWND parent,HMENU menu,HINSTANCE inst,LPVOID arg){int ismain=((ULONG_PTR)cls>65535&&!strcmp(cls,"Pharaoh Class"));int managed=ismain||find(parent)!=NULL;int lx=x,ly=y,lw=w,lh=h;if(managed&&!ismain&&w==CW_USEDEFAULT){x=y=lx=ly=0;w=lw=LW;h=lh=LH;}if(ismain){style=WS_POPUP|WS_MINIMIZEBOX|WS_CLIPCHILDREN;x=0;y=0;w=ow;h=oh;lx=ly=0;lw=LW;lh=LH;}
 if(managed&&!ismain){x=x*vw/LW;y=y*vh/LH;w=w*vw/LW;h=h*vh/LH;if(parent==mainwin){x+=vx;y+=vy;}}
 HWND win=CreateWindowExA(ex,cls,title,style,x,y,w,h,parent,menu,inst,arg);if(managed&&win&&count<128){int slot;for(slot=0;slot<count;slot++)if(!surfaces[slot].win)break;if(slot==count)count++;SURFACE *s=&surfaces[slot];memset(s,0,sizeof(*s));s->win=win;s->parent=parent;if(logfile){fprintf(logfile,"create %p class=%s parent=%p xy=%d,%d size=%d,%d\n",win,(ULONG_PTR)cls>65535?cls:"atom",parent,lx,ly,lw,lh);fflush(logfile);}s->x=lx;s->y=ly;s->w=lw>0?lw:1;s->h=lh>0?lh:1;s->native=((ULONG_PTR)cls<=65535 || !strcmp(cls,"BUTTON") || !strcmp(cls,"EDIT") || !strcmp(cls,"STATIC"));if(!s->native)allocdc(s);if(!s->native)s->proc=(WNDPROC)SetWindowLongA(win,GWL_WNDPROC,(LONG)proc);if(ismain){mainwin=win;place();}}
 return win;}
static HDC WINAPI hGetDC(HWND w){SURFACE*s=find(w);return s&&!s->native?s->dc:GetDC(w);}
static int WINAPI hReleaseDC(HWND w,HDC dc){SURFACE*s=find(w);if(s&&s->dc==dc){present(s);return 1;}return ReleaseDC(w,dc);}
static HDC WINAPI hBegin(HWND w,LPPAINTSTRUCT p){HDC d=BeginPaint(w,p);SURFACE*s=find(w);if(s&&!s->native){p->hdc=s->dc;SetRect(&p->rcPaint,0,0,s->w,s->h);return s->dc;}return d;}
static BOOL WINAPI hEnd(HWND w,const PAINTSTRUCT*p){BOOL r=EndPaint(w,p);present(find(w));return r;}
static BOOL WINAPI hBlt(HDC dst,int x,int y,int w,int h,HDC src,int sx,int sy,DWORD rop){BOOL r=BitBlt(dst,x,y,w,h,src,sx,sy,rop);present(bydc(dst));return r;}
static BOOL WINAPI hStretch(HDC dst,int x,int y,int w,int h,HDC src,int sx,int sy,int sw,int sh,DWORD rop){BOOL r=StretchBlt(dst,x,y,w,h,src,sx,sy,sw,sh,rop);present(bydc(dst));return r;}
static int WINAPI hMetrics(int n){if(n==SM_CXSCREEN||n==SM_CXVIRTUALSCREEN)return LW;if(n==SM_CYSCREEN||n==SM_CYVIRTUALSCREEN)return LH;return GetSystemMetrics(n);}
static BOOL WINAPI hClientRect(HWND w,LPRECT r){SURFACE*s=find(w);if(s){SetRect(r,0,0,s->w,s->h);return TRUE;}return GetClientRect(w,r);}
static POINT origin(SURFACE*s){POINT p={0,0};while(s&&s->win!=mainwin){p.x+=s->x;p.y+=s->y;s=find(s->parent);}return p;}
static BOOL WINAPI hWindowRect(HWND w,LPRECT r){SURFACE*s=find(w);if(s){POINT p=origin(s);SetRect(r,p.x,p.y,p.x+s->w,p.y+s->h);return TRUE;}return GetWindowRect(w,r);}
static BOOL WINAPI hCursor(LPPOINT p){BOOL r=GetCursorPos(p);if(mainwin){POINT q={0,0};ClientToScreen(mainwin,&q);p->x=(p->x-q.x-vx)*LW/vw;p->y=(p->y-q.y-vy)*LH/vh;}return r;}
static BOOL WINAPI hSetCursor(int x,int y){if(mainwin){POINT q={0,0};ClientToScreen(mainwin,&q);return SetCursorPos(q.x+vx+x*vw/LW,q.y+vy+y*vh/LH);}return SetCursorPos(x,y);}
static BOOL WINAPI hScreenClient(HWND w,LPPOINT p){SURFACE*s=find(w);if(s){POINT q=origin(s);p->x-=q.x;p->y-=q.y;return TRUE;}return ScreenToClient(w,p);}
static BOOL WINAPI hClientScreen(HWND w,LPPOINT p){SURFACE*s=find(w);if(s){POINT q=origin(s);p->x+=q.x;p->y+=q.y;return TRUE;}return ClientToScreen(w,p);}
static BOOL WINAPI hMove(HWND w,int x,int y,int cx,int cy,BOOL paint){if(logfile){fprintf(logfile,"move %p %d,%d %d,%d\n",w,x,y,cx,cy);fflush(logfile);}SURFACE*s=control(w);if(!s)return MoveWindow(w,x,y,cx,cy,paint);if(w==mainwin){place();return TRUE;}s->x=x;s->y=y;if(cx!=s->w||cy!=s->h){s->w=cx>0?cx:1;s->h=cy>0?cy:1;if(!s->native)allocdc(s);}placechild(s);return TRUE;}
static BOOL WINAPI hPos(HWND w,HWND after,int x,int y,int cx,int cy,UINT flags){if(logfile){fprintf(logfile,"pos %p %d,%d %d,%d flags=%x\n",w,x,y,cx,cy,flags);fflush(logfile);}SURFACE*s=control(w);if(!s)return SetWindowPos(w,after,x,y,cx,cy,flags);if(w==mainwin)return SetWindowPos(w,after,0,0,0,0,flags|SWP_NOMOVE|SWP_NOSIZE);if(!(flags&SWP_NOMOVE)){s->x=x;s->y=y;}if(!(flags&SWP_NOSIZE)&&(s->w!=cx||s->h!=cy)){s->w=cx>0?cx:1;s->h=cy>0?cy:1;if(!s->native)allocdc(s);}x=s->x*vw/LW;y=s->y*vh/LH;if(s->parent==mainwin){x+=vx;y+=vy;}return SetWindowPos(w,after,x,y,s->w*vw/LW,s->h*vh/LH,flags);}
static LONG WINAPI hDisplay(DEVMODEA*d,DWORD f){return DISP_CHANGE_SUCCESSFUL;}
static BOOL WINAPI hDestroy(HWND w){SURFACE*s=find(w);BOOL r=DestroyWindow(w);if(s){s->win=NULL;DeleteDC(s->dc);DeleteObject(s->bmp);s->dc=NULL;}return r;}
static HWND WINAPI hDialog(HINSTANCE i,LPCDLGTEMPLATE t,HWND p,DLGPROC f,LPARAM a){HWND w=CreateDialogIndirectParamA(i,t,p,f,a);if(logfile){fprintf(logfile,"dialog %p parent=%p style=%lx\n",w,p,GetWindowLongA(w,GWL_STYLE));fflush(logfile);}if(w&&mainwin){RECT r,m;GetWindowRect(w,&r);GetWindowRect(mainwin,&m);SetWindowPos(w,HWND_TOP,m.left+((m.right-m.left)-(r.right-r.left))/2,m.top+((m.bottom-m.top)-(r.bottom-r.top))/2,0,0,SWP_NOSIZE);}return w;}
static LONG WINAPI hLong(HWND w,int index,LONG value){SURFACE*s=find(w);if(s&&!s->native&&index==GWL_WNDPROC){LONG old=(LONG)s->proc;s->proc=(WNDPROC)value;return old;}return SetWindowLongA(w,index,value);}
static HDWP WINAPI hDefer(HDWP d,HWND w,HWND after,int x,int y,int cx,int cy,UINT flags){SURFACE*s=find(w);if(s){if(!(flags&SWP_NOMOVE)){s->x=x;s->y=y;}if(!(flags&SWP_NOSIZE)){s->w=cx>0?cx:1;s->h=cy>0?cy:1;if(!s->native)allocdc(s);}x=s->x*vw/LW;y=s->y*vh/LH;if(s->parent==mainwin){x+=vx;y+=vy;}cx=s->w*vw/LW;cy=s->h*vh/LH;}return DeferWindowPos(d,w,after,x,y,cx,cy,flags);}
static BOOL WINAPI hShow(HWND w,int cmd){if(w==mainwin&&cmd==SW_SHOWMAXIMIZED)cmd=SW_SHOWNORMAL;BOOL r=ShowWindow(w,cmd);return r;}
static LONG WINAPI hGetLong(HWND w,int index){SURFACE*s=find(w);if(s&&!s->native&&index==GWL_WNDPROC)return (LONG)s->proc;return GetWindowLongA(w,index);}
static DWORD WINAPI hMessagePos(void){DWORD v=GetMessagePos();POINT p={(short)LOWORD(v),(short)HIWORD(v)},q={0,0};if(mainwin){ClientToScreen(mainwin,&q);p.x=(p.x-q.x-vx)*LW/vw;p.y=(p.y-q.y-vy)*LH/vh;}return MAKELONG(p.x,p.y);}
static HWND WINAPI hFromPoint(POINT p){if(mainwin){POINT q={0,0};ClientToScreen(mainwin,&q);p.x=q.x+vx+p.x*vw/LW;p.y=q.y+vy+p.y*vh/LH;}return WindowFromPoint(p);}
static void hook(DWORD slot,void*fn){DWORD old;VirtualProtect((LPVOID)slot,4,PAGE_READWRITE,&old);*(void**)slot=fn;VirtualProtect((LPVOID)slot,4,old,&old);}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved){if(reason!=DLL_PROCESS_ATTACH)return TRUE;DisableThreadLibraryCalls(instance);BOOL(WINAPI*dpi)(void)=(void*)GetProcAddress(GetModuleHandleA("user32.dll"),"SetProcessDPIAware");if(dpi)dpi();GetModuleFileNameA(instance,ini,MAX_PATH);char*slash=strrchr(ini,'\\');if(slash)strcpy(slash+1,"PharaohHD.ini");ow=GetPrivateProfileIntA("Display","Width",1920,ini);oh=GetPrivateProfileIntA("Display","Height",1080,ini);fullscreen=GetPrivateProfileIntA("Display","Fullscreen",0,ini);if(ow<640||ow>7680)ow=1920;if(oh<480||oh>4320)oh=1080;
 if(GetPrivateProfileIntA("Display","Debug",0,ini))logfile=fopen("PharaohHD-debug.log","w");hook(0x464a00,hGetLong);hook(0x4649f8,hMessagePos);hook(0x464928,hFromPoint);hook(0x4648b8,hShow);hook(0x464a04,hLong);hook(0x464974,hDefer);hook(0x4649cc,hCreate);hook(0x464a80,hGetDC);hook(0x464a84,hReleaseDC);hook(0x4648d8,hBegin);hook(0x4648dc,hEnd);hook(0x46460c,hBlt);hook(0x464644,hStretch);hook(0x464a6c,hMetrics);hook(0x464a8c,hClientRect);hook(0x464a44,hWindowRect);hook(0x464a24,hCursor);hook(0x464a68,hSetCursor);hook(0x464984,hScreenClient);hook(0x46492c,hClientScreen);hook(0x4648b4,hMove);hook(0x464a08,hPos);hook(0x464a14,hDisplay);hook(0x464aa0,hDestroy);hook(0x464944,hDialog);return TRUE;}

