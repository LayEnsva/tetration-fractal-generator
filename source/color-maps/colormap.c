#define MAX_ROWS        1024
#define BLANK_LINES     1

typedef struct {
    
    float value;
    float red;
    float green;
    float blue;

} color_t;

color_t colorData[MAX_ROWS];

void readCSV(const char *filename) {
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "ColorMap: Error opening file\n");
        exit(1);
    }
    
    char line[256];
    
    int i;
    for(i=0;i<BLANK_LINES;i++) {
        fgets(line, sizeof(line), file);
    }
    
    int rows = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (rows >= MAX_ROWS) {
            fprintf(stderr, "ColorMap: Exceeded maximum number of rows\n");
            break;
        }
        sscanf(line, "%f,%f,%f,%f", &colorData[rows].value, &colorData[rows].red, &colorData[rows].green, &colorData[rows].blue);
        rows++;
    }

    fclose(file);
}

void colormap(float x, float *red, float *green, float *blue) {
    int i = x * 1023.0;
    *red   = colorData[i].red;
    *green = colorData[i].green;
    *blue  = colorData[i].blue;
}
