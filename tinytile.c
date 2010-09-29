#include <X11/Xlib.h>
#include <stdio.h>
Display *Dpy;
Screen   Scr;
Window   Root;

void
mainloop ()
{
   XEvent e;

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
         puts ("Map");
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
   Root = XDefaultRootWindow (Dpy);
   mainloop ();
   XCloseDisplay (Dpy);
   return 0;
}
