#include <X11/Xlib.h>
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
      puts ("e");
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
