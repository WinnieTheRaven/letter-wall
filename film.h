typedef struct {
  const char* shape;
  const unsigned int* color;
  size_t frame_w;
  size_t frame_h ;
  size_t center_x;
  size_t center_y;
} Frame;

typedef struct {
  const frame *frames;
  size_t length;
} Movie;
