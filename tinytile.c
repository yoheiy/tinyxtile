#include <X11/Xlib.h>
#include <stdio.h>
Display *Dpy;
Window   Root, client[256];
int      Scr;
int      w, h, s, n, bw[256];

/* configuration */
const int gap = 1;
const int barh = 16;
const int topbar = 1;

void
arrange ()
{
   Window master;
   int i, b, m = n - 1;

   if (!n) return;
   master = client[m];

   for (i = 0; i < m; i++) {
      b = bw[i];
      XMoveResizeWindow (Dpy, client[i],
            w, h - (h + gap) / m * (i + 1) + topbar * barh + gap,
            s - w - 2 * b, (h + gap) / m - 2 * b - gap);
   }
   b = bw[m];
   XMoveResizeWindow (Dpy, master, 0, topbar * barh, w - 2 * b - gap,
         h - 2 * b);
}

void
newwindow (Window wn)
{
   XWindowAttributes wattr;

   XGetWindowAttributes (Dpy, wn, &wattr);
   if (wattr.map_state == IsViewable &&
       wattr.override_redirect == False) {
      client[n] = wn;
      bw[n++] = wattr.border_width;
   }
}

void
listwindow ()
{
   Window r_root, r_parent, *r_ch;
   unsigned int n_ch, i;

   if (!XQueryTree (Dpy, Root, &r_root, &r_parent, &r_ch, &n_ch))
      return;

   n = 0;
   for (i = 0; i < n_ch; i++)
      newwindow (r_ch[i]);

   XFree (r_ch);
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
         if ((e.xconfigurerequest.value_mask & CWStackMode) &&
             (!e.xconfigurerequest.above) &&
             (e.xconfigurerequest.detail == Above)) {
            XRaiseWindow (Dpy, wn);
            listwindow ();
            arrange ();
         } else if (e.xconfigurerequest.value_mask & CWWidth) {
            w = e.xconfigurerequest.width;
            arrange ();
         }
         break;
      case MapRequest:
         wn = e.xmaprequest.window;
         listwindow ();
         newwindow (wn);
         arrange ();
         XMapRaised (Dpy, wn);
         break;
      case UnmapNotify:
         listwindow ();
         arrange ();
         break;
      }
   }
}

int
xerror (Display *dpy, XErrorEvent *e)
{
   return 0;
}

int
main ()
{
   Dpy = XOpenDisplay (NULL);
   if (!Dpy) return 1;
   Scr = DefaultScreen (Dpy);
   Root = DefaultRootWindow (Dpy);
         s = DisplayWidth (Dpy, Scr);
         w = s / 2;
         h = DisplayHeight (Dpy, Scr) - barh;
   XSetErrorHandler (xerror);
   listwindow ();
   arrange ();
   mainloop ();
   XCloseDisplay (Dpy);
   return 0;
}
