#include <X11/Xlib.h>
#include <stdio.h>
Display *Dpy;
Window   Root;
int      Scr;

void
mainloop ()
{
   XEvent e;
   Window wn;
   unsigned int w, h;

   XSelectInput (Dpy, Root, SubstructureRedirectMask);
   for (;;) {
      XNextEvent (Dpy, &e);
      switch (e.type) {
      case CirculateRequest:
         puts ("Circulate");
         break;
      case ConfigureRequest:
         puts ("Configure");
         break;
      case MapRequest:
         wn = e.xmaprequest.window;
         printf ("Map %08x\n", wn);
         w = DisplayWidth (Dpy, Scr) / 2;
         h = DisplayHeight (Dpy, Scr);
         XMoveResizeWindow (Dpy, wn, 0, 0, w, h);
         XMapRaised (Dpy, wn);
         break;
      default:
         puts ("e");
      }
   }
}

int
main ()
{
   Dpy = XOpenDisplay (NULL);
   if (!Dpy) return 1;
   Scr = DefaultScreen (Dpy);
   Root = DefaultRootWindow (Dpy);
   mainloop ();
   XCloseDisplay (Dpy);
   return 0;
}
