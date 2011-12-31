#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
Display *Dpy;
Window   Root;
int      Scr;
unsigned long Black;
GC gc;

/* configurations */
const int char_height = 10;
const char *default_string = "X no bar";

void
draw ()
{
   XTextProperty name;
   const char * text = XGetWMName (Dpy, Root, &name) ? (char *)name.value
                                                     : default_string;
   XClearWindow (Dpy, Root);
   XDrawString (Dpy, Root, gc, 0, char_height, text, strlen (text));
}

void
mainloop ()
{
   XEvent e;

   draw ();
   for (;;) {
      XNextEvent (Dpy, &e);
      if ((e.type == Expose && e.xexpose.count == 0) ||
          (e.type == PropertyNotify))
         draw ();
   }
}

int
main ()
{
   Dpy = XOpenDisplay (NULL);
   if (!Dpy) return 1;
   Root = DefaultRootWindow (Dpy);
   Scr = DefaultScreen (Dpy);
   Black = BlackPixel (Dpy, Scr);
   gc = XCreateGC (Dpy, Root, 0, NULL);
   XSetForeground (Dpy, gc, Black);

   XSelectInput (Dpy, Root, ExposureMask | PropertyChangeMask);
   mainloop ();

   XCloseDisplay (Dpy);
   return 0;
}
