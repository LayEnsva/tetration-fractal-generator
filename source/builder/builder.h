#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define FILE_BITMAP     "out/bitmap.bmp"
#define FILE_RAWOUT     "out/data.bin"
#define PATH_COLORMAP   "color-maps/CSV/"

typedef struct userinfo_t {
    float width;
    float height;
    float center_x;
    float center_y;
    float log_zoom;
    float std_diagonal;
} userinfo_t;

typedef struct sysinfo_t {
    float x1, x2, y1, y2;
    float pixel_step;
    float iterations;
} sysinfo_t;
