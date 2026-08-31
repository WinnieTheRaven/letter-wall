#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <unistd.h>
#include <err.h>
/*personal modules created for this project UwUr*/
#include "film.h"
#include "config.h"

#define PALETTE_N (sizeof(PALETTE) / sizeof(PALETTE[0]))

/*function to get the timestamp and convert to microseconds since
 usleep works at that scale*/

static uint64_t clock_watch(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC,&ts);
  return ((uint64_t) ts.tv_sec * 1000000ULL) + ((uint64_t) ts.tv_nsec / 1000ULL);
}

/*Function to convert between fps world and microsecond world*/

static uint64_t fps_to_us(int fps) {
  return (uint64_t) 1000000 / (uint64_t) fps;
}

int main(void) {
  
  const uint64_t target_fps = fps_to_us(which_fps);
  uint64_t next_frame_marker = clock_watch();
  
  Frame const snow_roll[4] =
    {{snow_shape0,snow_color0,snow_width,snow_height,snow_cx,snow_cy},
     {snow_shape1,snow_color1,snow_width,snow_height,snow_cx,snow_cy},
     {snow_shape2,snow_color2,snow_width,snow_height,snow_cx,snow_cy},
     {snow_shape3,snow_color3,snow_width,snow_height,snow_cx,snow_cy}};

  Display *dpy;
  int screen;
  Window root;
  unsigned int scr_w, scr_h;
  unsigned int cols, rows;
  Pixmap pixmap;
  GC gc;
  XFontStruct *font;
  Colormap cmap;
  XColor xcolors[PALETTE_N];
  char* grid_shape;
  unsigned int* grid_color;
  unsigned int i;
  unsigned int snow_frame = 0;
  unsigned int CELL_W;
  unsigned int CELL_H;

  dpy = XOpenDisplay(NULL);
  if (!dpy) {
    fprintf(stderr,"letter-wall: Dunno why I couldn't open X display,"
	    " help?!\n");
    return 1;
  }

  screen = DefaultScreen(dpy);
  root = RootWindow(dpy, screen);
  scr_w = (unsigned int) DisplayWidth(dpy, screen);
  scr_h = (unsigned int) DisplayHeight(dpy, screen);
  cmap = DefaultColormap(dpy, screen);

  font = XLoadQueryFont(dpy,fontname);
  if (!font) {
    fprintf(stderr, "letter-wall:could not load font '%s'\n",fontname);
    XCloseDisplay(dpy);
    return 1;
  }

  /*Once the font is loaded then we  setup the font dimensions and
   the resolution of our background in characters UwUr <3*/
  CELL_W = (unsigned int) (font->max_bounds.width);
  CELL_H = (unsigned int) ((font->max_bounds.ascent) +
	    (font->max_bounds.descent));
  
  cols = scr_w / CELL_W;
  rows = scr_h / CELL_H;
  
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

  pixmap = XCreatePixmap(dpy, root, scr_w, scr_h, (unsigned int) DefaultDepth(dpy, screen));

  /*In this section we initiallize the background*/
  size_t total_cells = (size_t) cols * (size_t) rows;
  grid_shape = malloc(total_cells * sizeof *grid_shape);
  grid_color = malloc(total_cells * sizeof *grid_color);
  
  if (!grid_shape || !grid_color) {
    fprintf(stderr, "letter-wall: out of memory\n");
    free(grid_shape);
    free(grid_color);
    XFreePixmap(dpy,pixmap);
    XFreeGC(dpy,gc);
    XFreeFont(dpy,font);
    XCloseDisplay(dpy);
    return 1;
  }
  
  Frame grid = {grid_shape,grid_color,cols,rows, cols / 2, rows / 2};
  
  for (i = 0; i < (cols*rows);i++) {
    grid_color[i] = 0;
    grid_shape[i] = ' ';
  }

  /*This for cycle contains the actual logic of the stuff
   that is going to happen in the desktop background UwUr*/
  
  for (;;) {
    snow_frame = (snow_frame + 1) % snow_length;
    Window ret_root, ret_child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    int px, py;
    int x, y;

    /* There might be a better way. But in this cycle we
     cleanup our baclground before redrawing UwUr <3*/

    for (x = 0; x < (int) cols * (int) rows; x++) {
      grid.shape[x] = ' ';
      grid.color[x] = 0;
    }
    
    /*XQueryPointer is a function that captures in the values of
     root_x, root_y, win_x and win_y the position of the cursor
     by reference*/
    
    XQueryPointer(dpy, root, &ret_root, &ret_child, &root_x, &root_y, &win_x, &win_y, &mask);

    /*this is just a coordinate tranformation to capture into
     both px and py the position of the cursor from the point of
    view of the characters UwUr*/
    
    px = root_x / (int) CELL_W;
    py = root_y / (int) CELL_H;

    /*Inside these loops the pointer information gets set up*/

    for (y = 0; y < (int) snow_height; y++) {
      for (x = 0; x < (int) snow_width; x++) {
	/*getting the coordinates for the relevant cell*/
        int dx = px - x + (int) snow_cx;
	int dy = py - y + (int) snow_cy;

	/*assigning the corresponding color and letter to
	  the cell checking boundaries UwUr*/
	if (dx < 0 || dx >= (int) cols ||
	    dy < 0 || dy >= (int) rows) {
	  continue;
	} else {
	  /*getting the addresses for the relevan cell*/
	  char *l = &grid.shape[(unsigned int) dy * cols + (unsigned int) dx];
	  unsigned int *c = &grid.color[(unsigned int) dy * cols + (unsigned int) dx];
	  *l = snow_roll[snow_frame].shape[(unsigned int) y * snow_width + (unsigned int) x];
	  *c = snow_roll[snow_frame].color[(unsigned int) y * snow_width + (unsigned int) x];
	}
      }
    }

    XSetForeground(dpy, gc, BlackPixel(dpy, screen));
    XFillRectangle(dpy, pixmap, gc, 0, 0, scr_w, scr_h);

    for (y = 0; y < (int) rows; y++) {
      for (x = 0; x < (int) cols; x++) {
	char l = grid.shape[(unsigned int) y * cols + (unsigned int) x];
	unsigned int c = grid.color[(unsigned int) y * cols + (unsigned int) x];
	char s[] = {l,'\0'};
	if (l != ' ') {
	  XSetForeground(dpy, gc, xcolors[c].pixel);
	  XDrawString(dpy,pixmap,gc,x * (int) CELL_W,y * (int) CELL_H + (int) CELL_H,s,1);
	}
      }
    }

    XSetWindowBackgroundPixmap(dpy, root, pixmap);
    XClearWindow(dpy, root);
    XFlush(dpy);

    /*Benchmarking to adjust time UwUr*/
    
    uint64_t current_time = clock_watch();

    /*Are we on schedule?*/
    if (current_time < next_frame_marker) {
      uint64_t sleep_duration = next_frame_marker - current_time;
      usleep((useconds_t) sleep_duration);
    }

    /* Did we lagged badly?*/
    
    if (current_time > next_frame_marker + target_fps) {
      next_frame_marker = current_time;
    }

    /* Next bus stop for correct fps marking*/
    next_frame_marker += target_fps;
    
  }

  XFreeFont(dpy, font);
  XFreeGC(dpy, gc);
  XFreePixmap(dpy, pixmap);
  free(grid_color);
  free(grid_shape);
  XCloseDisplay(dpy);
  return 0;
}
