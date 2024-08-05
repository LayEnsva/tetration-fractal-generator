/**
 *
 *  User defined constants
 *  for image generation
 *  and fractal navigation
 *
 */
 
 

// Number of sequence iterations
#define ITERATIONS      64

// Image size (in pixels)
#define USR_WIDTH       64
#define USR_HEIGHT      64

// Fractal center point (on the complex plane)
#define USR_CENTER_X    -4.11
#define USR_CENTER_Y    0

// Zoom in log2 scale (+1 log. zoom = x2 normal zoom)
#define USR_LOGZOOM     6

// Anti-Aliasing Filter for data generation (upscales image size)
#define USR_AAF         1

// Anti-Aliasing Filter for image generation (shrinks image size) 
#define BUILD_AAF       1

// Color scheme (name or number from 1 to 12)
#define BUILD_COLORMAP  INFERNO
