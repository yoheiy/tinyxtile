#include <X11/Xlib.h>
#include <stdio.h>
Display *Dpy;
Window   Root;
int      Scr;
unsigned int w, h;

void
listwindow (Window master)
{
   Window r_root, r_parent, *r_ch;
   int n_ch;
   XWindowAttributes wattr;

   if (XQueryTree (Dpy, Root, &r_root, &r_parent, &r_ch, &n_ch)) {
      int i, j = 0, k = 0;
      for (i = 0; i < n_ch; i++) {
         XGetWindowAttributes (Dpy, r_ch[i], &wattr);
         if (wattr.map_state == IsViewable &&
             wattr.override_redirect == False)
            printf ("%03d %08x\n", j++, r_ch[i]);
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
            XMoveResizeWindow (Dpy, r_ch[i], w, h - h / j * k, w, h / j - 1);
         }
      }
      XMoveResizeWindow (Dpy, master, 0, 0, w - 1, h);
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
      case CirculateRequest:
         puts ("Circulate");
         break;
      case ConfigureRequest:
         wn = e.xconfigurerequest.window;
         printf ("Configure %08x\n", wn);
         printf (" vmask : %08x\n StackMode = %08x\n",
                 e.xconfigurerequest.value_mask, CWStackMode);
         printf (" above : %08x\n",
                 e.xconfigurerequest.above);
         printf (" detail : %08x\n Above = %08x\n TopIf = %08x\n",
                 e.xconfigurerequest.detail, Above, TopIf);
         if (e.xconfigurerequest.value_mask & CWStackMode)
            if (!e.xconfigurerequest.above)
               if (e.xconfigurerequest.detail == Above) {
                  XRaiseWindow (Dpy, e.xconfigurerequest.window);
                  puts ("Raise");
               }
         listwindow (0);
         break;
      case MapRequest:
         wn = e.xmaprequest.window;
         printf ("Map %08x\n", wn);
         listwindow (wn);
         XMapRaised (Dpy, wn);
         break;
      case UnmapNotify:
         wn = e.xmaprequest.window;
         printf ("Unmap %08x\n", wn);
         listwindow (0);
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
         w = DisplayWidth (Dpy, Scr) / 2;
         h = DisplayHeight (Dpy, Scr);
   listwindow (0);
   mainloop ();
   XCloseDisplay (Dpy);
   return 0;
}
