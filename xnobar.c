#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
Display *Dpy;
Window   Root;
int      Scr;
Window   TopLevel;
unsigned long White;
unsigned long Black;
GC gc;

/* configurations */
const int bar_height = 14;
const int char_height = 10;
const char *default_string = "X no bar";

void
mainloop ()
{
   XEvent e;
   XTextProperty name;
   const char *text;

   for (;;) {
      XNextEvent (Dpy, &e);
      if ((e.type == Expose && e.xexpose.count == 0) ||
          (e.type == PropertyNotify)) {
         text = XGetWMName (Dpy, Root, &name) ? (char *)name.value
                                              : default_string;
         XClearWindow (Dpy, TopLevel);
         XDrawString (Dpy, TopLevel, gc, 0, char_height, text,
               strlen (text));
      }
   }
}

int
main ()
{
   int dpy_width;
   XSetWindowAttributes attr = { .override_redirect = True };

   Dpy = XOpenDisplay (NULL);
   if (!Dpy) return 1;
   Root = DefaultRootWindow (Dpy);
   Scr = DefaultScreen (Dpy);
   White = WhitePixel (Dpy, Scr);
   Black = BlackPixel (Dpy, Scr);

   dpy_width = DisplayWidth (Dpy, Scr);
   TopLevel = XCreateSimpleWindow (Dpy, Root,
         0, 0, dpy_width, bar_height, 0, Black, White);
   XChangeWindowAttributes (Dpy, TopLevel, CWOverrideRedirect, &attr);

   gc = XCreateGC (Dpy, TopLevel, 0, NULL);
   XSetForeground (Dpy, gc, Black);

   XSelectInput (Dpy, TopLevel, ExposureMask);
   XSelectInput (Dpy, Root, PropertyChangeMask);
   XMapWindow (Dpy, TopLevel);
   mainloop ();

   XCloseDisplay (Dpy);
   return 0;
}
