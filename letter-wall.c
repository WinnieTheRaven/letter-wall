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
/*personal modules created for this project UwUr*/
#include "film.h"
#include "config.h"

#define CELL_W 8
#define CELL_H 16
#define FRAME_USEC 66000

#define PALETTE_N (sizeof(PALETTE) / sizeof(PALETTE[0]))

int main(void) {
  Frame snow_roll[4] =
    {{snow_shape0,snow_color0,snow_width,snow_height,snow_cx,snow_cy},
     {snow_shape1,snow_color1,snow_width,snow_height,snow_cx,snow_cy},
     {snow_shape2,snow_color2,snow_width,snow_height,snow_cx,snow_cy},
     {snow_shape3,snow_color3,snow_width,snow_height,snow_cx,snow_cy}};

  Display *dpy;
  int screen;
  Window root;
  int scr_w, scr_h;
  unsigned int cols, rows;
  Pixmap pixmap;
  GC gc;
  XFontStruct *font;
  Colormap cmap;
  XColor xcolors[PALETTE_N];
  char* grid_shape;
  unsigned int* grid_color;
  Frame grid;
  int i;
  unsigned int snow_frame = 0;

  srand(time(NULL));

  dpy = XOpenDisplay(NULL);
  if (!dpy) {
    fprintf(stderr,"letter-wall: Dunno why I couldn't open X display,"
	    " help?!\n");
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

  /*In this section we initiallize the background*/
  grid_shape = malloc(sizeof(char) * cols * rows);
  grid_color = malloc(sizeof(unsigned int) * cols * rows);
  grid = {grid_shape,grid_color,cols,rows, cols / 2, rows / 2};
  
  if (!grid_shape || !grid_color) {
    fprintf(stderr, "letter-wall: out of memory\n");
    return 1;
  }

  /*This for cycle contains the actual logic of the stuff
   that is going to happen in the desktop background UwUr*/
  
  for (;;) {
    snow_frame++;
    snow_frame = snow_frame == snow_length ? 0 : snow_frame;
    Window ret_root, ret_child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    int px, py;
    unsigned int x, y;
    
    /*XQueryPointer is a function that captures in the values of
     root_x, root_y, win_x and win_y the position of the cursor
     by reference*/
    
    XQueryPointer(dpy, root, &ret_root, &ret_child, &root_x, &root_y, &win_x,&win_y, &mask);

    /*this is just a coordinate tranformation to capture into
     both px and py the position of the cursor from the point of
    view of the characters UwUr*/
    
    px = root_x / CELL_W;
    py = root_y / CELL_H;

    /*Inside these loops the pointer information gets set up*/

    for (y = 0; y < snow_width; y++) {
      for (x = 0; x < snow_height; x++) {
	/*getting the coordinates for the relevant cell*/
        int dx = px - x + snow_cx, dy = py - y + snow_cy;
	/*getting the addresses for the relevan cell*/
        char *l = &grid.shape[dy * cols + dx];
	unsigned int *c = &grid.color[dy * cols + dx];

	/*assigning the corresponding color and letter to
	  the cell UwUr*/

	*l = snow_roll[snow_frame].shape[y * snow_width + x];
	*c = snow_roll[snow_frame].color[y * snow_width + x];
      }
    }

    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
    XFillRectangle(dpy, pixmap, gc, 0, 0, scr_w, scr_h);

    for (y = 0; y < rows; y++) {
      for (x = 0; x < cols; x++) {
	unsigned int *c = &grid.color[y * cols + x];
        char s[2] = {' ',' '};
        XSetForeground(dpy, gc, xcolors[*c].pixel);
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
  free(grid_color);
  free(grid_shape);
  XCloseDisplay(dpy);
  return 0;
}
