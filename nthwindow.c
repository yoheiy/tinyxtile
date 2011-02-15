#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
Display *Dpy;
Window   Root;

int
is_viewable (Window w)
{
   XWindowAttributes wattr;

   XGetWindowAttributes (Dpy, w, &wattr);
   return wattr.map_state == IsViewable &&
          wattr.override_redirect == False;
}

Window
find_nth_window (int n)
{
   Window r_root, r_parent, *r_ch, w = 0;
   unsigned int n_ch;
   int i, j = 0;

   if (!XQueryTree (Dpy, Root, &r_root, &r_parent, &r_ch, &n_ch))
      return 0;

   for (i = n_ch; i; i--) {
      //printf ("#%d window %x\n", j, r_ch[i]);
      if (is_viewable (r_ch[i]) && j++ == n) {
          //puts ("found");
          w = r_ch[i];
          break;
      }
   }
   XFree (r_ch);
   return w;
}

int
xerror (Display *dpy, XErrorEvent *e)
{
   return 0;
}

int
main (int argc, char *argv[])
{
   int n;
   Window w;

   if (argc < 2)
      return 2;
   n = atoi (argv[1]);

   Dpy = XOpenDisplay (NULL);
   if (!Dpy)
      return 1;
   Root = DefaultRootWindow (Dpy);

   XSetErrorHandler (xerror);

   if ((w = find_nth_window (n)))
      printf ("0x%x\n", w);

   XCloseDisplay (Dpy);
   return w ? 0 : 1;
}
