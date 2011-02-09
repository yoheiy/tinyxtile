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
arrange (Window master)
{
   int i, b, m = n;
   XWindowAttributes wattr;

   if (!master) { master = client[--m]; }

   for (i = 0; i < m; i++) {
      b = bw[i];
      XMoveResizeWindow (Dpy, client[i],
            w, h - (h + gap) / m * (i + 1) + topbar * barh + gap,
            s - w - 2 * b, (h + gap) / m - 2 * b - gap);
   }
   XGetWindowAttributes (Dpy, master, &wattr);
   b = wattr.border_width;
   XMoveResizeWindow (Dpy, master, 0, topbar * barh, w - 2 * b - gap,
         h - 2 * b);
}

void
listwindow (Window master)
{
   Window r_root, r_parent, *r_ch;
   unsigned int n_ch, i;
   XWindowAttributes wattr;

   if (!XQueryTree (Dpy, Root, &r_root, &r_parent, &r_ch, &n_ch))
      return;

   n = 0;
   for (i = 0; i < n_ch; i++) {
      XGetWindowAttributes (Dpy, r_ch[i], &wattr);
      if (wattr.map_state == IsViewable &&
          wattr.override_redirect == False) {
         client[n] = r_ch[i];
         bw[n++] = wattr.border_width;
      }
   }
   XFree (r_ch);

   arrange (master);
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
            listwindow (0);
         } else if (e.xconfigurerequest.value_mask & CWWidth) {
            w = e.xconfigurerequest.width;
	    arrange (0);
         }
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
   listwindow (0);
   mainloop ();
   XCloseDisplay (Dpy);
   return 0;
}
