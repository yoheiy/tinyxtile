#include <X11/Xlib.h>
#include <stdio.h>
Display *Dpy;
Window   Root;
int      Scr;
unsigned int w, h;

/* configuration */
const int gap = 1;
const int barh = 16;

void
listwindow (Window master)
{
   Window r_root, r_parent, *r_ch;
   int n_ch;
   XWindowAttributes wattr;

   if (XQueryTree (Dpy, Root, &r_root, &r_parent, &r_ch, &n_ch)) {
      int i, j = 0, k = 0, b;
      for (i = 0; i < n_ch; i++) {
         XGetWindowAttributes (Dpy, r_ch[i], &wattr);
         if (wattr.map_state == IsViewable &&
             wattr.override_redirect == False)
            j++;
      }
      if (!master) { j--; }
      for (i = 0; i < n_ch; i++) {
         XGetWindowAttributes (Dpy, r_ch[i], &wattr);
         if (wattr.map_state == IsViewable &&
             wattr.override_redirect == False)
         {
            k++;
            if (k > j) {
               master = r_ch[i];
               break;
            }
            b = wattr.border_width;
            XMoveResizeWindow (Dpy, r_ch[i], w, h - h / j * k + barh,
                  w - 2 * b, h / j - 2 * b - 1);
         }
      }
      XGetWindowAttributes (Dpy, r_ch[i], &wattr);
      b = wattr.border_width;
      XMoveResizeWindow (Dpy, master, 0, barh, w - 2 * b - gap,
                                               h - 2 * b);
      XFree (r_ch);
   }
}

void
mainloop ()
{
   XEvent e;
   Window wn;

   XSelectInput (Dpy, Root, SubstructureRedirectMask |
                            SubstructureNotifyMask);
   for (;;) {
      XNextEvent (Dpy, &e);
      switch (e.type) {
      case ConfigureRequest:
         wn = e.xconfigurerequest.window;
         if (e.xconfigurerequest.value_mask & CWStackMode)
            if (!e.xconfigurerequest.above)
               if (e.xconfigurerequest.detail == Above)
                  XRaiseWindow (Dpy, e.xconfigurerequest.window);
         listwindow (0);
         break;
      case MapRequest:
         wn = e.xmaprequest.window;
         listwindow (wn);
         XMapRaised (Dpy, wn);
         break;
      case UnmapNotify:
         listwindow (0);
         break;
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
         w = DisplayWidth (Dpy, Scr) / 2;
         h = DisplayHeight (Dpy, Scr) - barh;
   listwindow (0);
   mainloop ();
   XCloseDisplay (Dpy);
   return 0;
}
