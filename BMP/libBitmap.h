#ifndef _LIB_BITMAP_H_
#define _LIB_BITMAP_H_

//#define ENABLED_DOUBLE_BUFFERING	1

int read_bmp(char *filename, char **data, int *cols, int *rows);
int close_bmp(void);

int fb_init(int * screen_width, int * screen_height, int * bits_per_pixel, int * line_length);
void fb_close(void);
void fb_clear(void);
void fb_write(char *picData, int picWidth, int picHeight);
void fb_write_b(char *picData, int picWidth, int picHeight, int running);
void fb_write_c(char *picData, int picWidth, int picHeight, int x_move, int y_move);
void fb_write_reverse(char *picData, int picWidth, int picHeight);
void fb_doubleBufSwap(void);
#endif