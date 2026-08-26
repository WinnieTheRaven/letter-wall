#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <err.h>

#define CELL_W 8
#define CELL_H 16
#define FRAME_USEC 66000

#define PALETTE_N (sizeof(PALETTE) / sizeof(PALETTE[0]))

typedef struct {
  char ch;
  int color_idx;
} Cell;

int main(void) {

  Display *dpy;
  int screen;
  Window root;
  int scr_w, scr_h;
  int cols, rows;
  Pixmap pixmap;
  GC gc;
  XFontStruct *font;
  Colormap cmap;
  XColor xcolors[PALETTE_N];
  Cell *grid;
  int i;

  srand(time(NULL));

  dpy = XOpenDisplay(NULL);
  if (!dpy) {
    char *disErr = "letter-wall: Dunno why I couldn't open X display,"
      "help?!\n";
    fprintf(stderr,disErr);
    return 1;
  }

  screen = DefaultScreen(dpy);
  root = RootWindow(dpy, screen);
  scr_w = DisplayWidth(dpy, screen);
  scr_h = DisplayHeight(dpy, screen);
  cmap = DefaultColormap(dpy, screen);

  cols = scr_w / CELL_W;
  rows = scr_h / CELL_H;

  font = XLoadQueryFont(dpy, fontname);
  if (!font) {
    fprintf(stderr, "letter-wall:could not load font '%s'\n",fontname);
    XCloseDisplay(dpy);
    return 1;
  }

  for (i = 0; i < (int)PALETTE_N; i++) {
    if (!XParseColor(dpy, cmap, PALETTE[i], &xcolors[i]) ||
        !XAllocColor(dpy, cmap, &xcolors[i])) {
      fprintf(stderr, "letter-wall: failed to allocate color %s\n", PALETTE[i]);
      XCloseDisplay(dpy);
      return 1;
    }
  }

  gc = XCreateGC(dpy, root, 0, NULL);
  XSetFont(dpy, gc, font->fid);

  pixmap = XCreatePixmap(dpy, root, scr_w, scr_h, DefaultDepth(dpy, screen));

  grid = malloc(sizeof(Cell) * cols * rows);
  if (!grid) {
    fprintf(stderr, "letter-wall: out of memory\n");
    return 1;
  }

  /*This for cycle contains the actual logic of the stuff
   that is going to happen in the desktop background UwUr*/
  
  for (;;) {
    Window ret_root, ret_child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    int px, py;
    int x, y;
    
    /*XQueryPointer is a function that capptures in the values of
     root_x, root_y, win_x and win_y the position of the cursor
     by reference*/
    
    XQueryPointer(dpy, root, &ret_root, &ret_child, &root_x, &root_y, &win_x,&win_y, &mask);

    /*this is just a coordinate tranformation to capture into
     both px and py the position of the cursor from the point of
    view of the characters UwUr*/
    
    px = root_x / CELL_W;
    py = root_y / CELL_H;

    for (y = 0; y < rows; y++) {
      for (x = 0; x < cols; x++) {
        int dx = x - px, dy = y - py;
        int dist2 = dx * dx + dy * dy;
        Cell *c = &grid[y * cols + x];

        if (dist2 < 25) {
          c->ch = CHARSET[rand() % (sizeof(CHARSET) - 1)];
	  c->color_idx = rand() % PALETTE_N;
        } else if (rand() % 400 == 0) {
	  c->ch = CHARSET[rand() % (sizeof(CHARSET )- 1)];
        }
      }
    }

    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
    XFillRectangle(dpy, pixmap, gc, 0, 0, scr_w, scr_h);

    for (y = 0; y < rows; y++) {
      for (x = 0; x < cols; x++) {
        Cell *c = &grid[y * cols + x];
        char s[2] = {c->ch, 0};
        XSetForeground(dpy, gc, xcolors[c->color_idx].pixel);
	XDrawString(dpy,pixmap,gc,x * CELL_W,y * CELL_H + CELL_H - 4,s,1);
      }
    }

    XSetWindowBackgroundPixmap(dpy, root, pixmap);
    XClearWindow(dpy, root);
    XFlush(dpy);

    usleep(FRAME_USEC);
  }

  XFreeFont(dpy, font);
  XFreeGC(dpy, gc);
  XFreePixmap(dpy, pixmap);
  free(grid);
  XCloseDisplay(dpy);
  return 0;
}
