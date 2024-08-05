#include "builder.h"
#include "../bitmap/bitmap.c"
#include "../color-maps/colormap.c"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

enum {
    NONE,
	INFERNO,
    MAGMA,
    PLASMA,
    VIRIDIS,
    FAST,
    BLACKBODY,
    EXTBLACKBODY,
    KINDLMANN,
    EXTKINDLMANN,
    
	SHADEWOB,
	SHADEBOW,
	CHELIX
};

void initColorMap(int grad) {
    switch(grad) {
        
        case 1: readCSV(PATH_COLORMAP "inferno-table-float-1024.csv"); break;
        case 2: readCSV(PATH_COLORMAP "magma-table-float-1024.csv"); break;
        case 3: readCSV(PATH_COLORMAP "plasma-table-float-1024.csv"); break;
        case 4: readCSV(PATH_COLORMAP "viridis-table-float-1024.csv"); break;
        case 5: readCSV(PATH_COLORMAP "fast-table-float-1024.csv"); break;
        case 6: readCSV(PATH_COLORMAP "black-body-table-float-1024.csv"); break;
        case 7: readCSV(PATH_COLORMAP "extended-black-body-table-float-1024.csv"); break;
        case 8: readCSV(PATH_COLORMAP "kindlmann-table-float-1024.csv"); break;
        case 9: readCSV(PATH_COLORMAP "extended-kindlmann-table-float-1024.csv"); break;
            
        default: return;
    }
}

void cubehelix(float lam, float s, float r, float h, float gam, float* red, float* green, float* blue) {
	const float phi = 2.0*M_PI*((s/3.0)+(r*lam));
	const float lamtogam = pow(lam,gam);
    const float a = h*lamtogam*(1.0-lamtogam)*0.5;

    *red 	= lamtogam + a*(-0.14861*cos(phi) +  1.78277*sin(phi));
    *green 	= lamtogam + a*(-0.29227*cos(phi) + -0.90649*sin(phi));
    *blue 	= lamtogam + a*( 1.97294*cos(phi) +  0.00000*sin(phi));

    *red = fmax(0, fmin(1, *red));
    *green = fmax(0, fmin(1, *green));
    *blue = fmax(0, fmin(1, *blue));
}

uint32_t gradient(float input, int grad) {
    uint32_t output;

    float x = input;
    float red, green, blue;
    
    
    switch(grad) {
    	
    	case SHADEWOB:
			red = x;
			green = x;
			blue = x;
    		break;
    	
		case SHADEBOW:
			red = 1-x;
			green = 1-x;
			blue = 1-x;
    		break;
    		
    	case CHELIX:
    	cubehelix(x,0.5,-1.5,1,0.6,&red,&green,&blue);
//    	cubehelix(x,1.5,-1.5,1,0.8,&red,&green,&blue);
    		break;
    	
    	default:
    		colormap(x, &red, &green, &blue);
	}


    int r = (int)(red * 255.0);
    int g = (int)(green * 255.0);
    int b = (int)(blue * 255.0);

    output = (r << 16) | (g << 8) | b;

    return output;
}

float profile(float x, float a) {
	float b = sqrt((0.25)+(1.0/a)) - (0.5);
	return ((1.0/(a*(1.0-x+b)))-b);
}



void builder_DataBuild(userinfo_t usr_fractal, float *img, int AAF, int grad) {
    
	int i,j,k,l;
	float shade;
	
    initColorMap(grad);
	
	drawInit(usr_fractal.width / AAF, usr_fractal.height / AAF);

	for(j = 0; j < usr_fractal.height; j += AAF) {
		for(i = 0; i < usr_fractal.width; i += AAF) {
		    
			shade = 0.0;
			
			for(l = 0; l < AAF; l++)
				for(k = 0; k < AAF; k++)
					shade += (float) img[(i+k) + (j+l) * (int) usr_fractal.width];
					
			pixelColor(i/AAF, j/AAF, gradient(profile(shade / (AAF*AAF), 20), grad));
		}	
    }
    
	createFile(FILE_BITMAP);
}


int builder_FileBuild(int AAF, int grad) {
    
	long filesize;
	float *img = NULL;
	userinfo_t usr_fractal;
	FILE *file = fopen(FILE_RAWOUT,"rb");
	if (file == NULL) {
        fprintf(stderr, "builder.c: Failed to open data.bin file.\n");
        return EXIT_FAILURE;
    }
    
	fseek(file,0,SEEK_END);
    filesize = ftell(file);
	img = (float *) malloc(filesize);
	if (img == NULL) {
        fprintf(stderr, "builder.c: Failed allocate memory.\n");
        return EXIT_FAILURE;
    }
	
	fseek(file, 0, SEEK_SET);
	fread(&usr_fractal, sizeof(userinfo_t), 1, file);
	fread(img, sizeof(float), usr_fractal.width * usr_fractal.height, file);
	
    builder_DataBuild(usr_fractal, img, AAF, grad);
    
	createFile(FILE_BITMAP);
	
	return EXIT_SUCCESS;
}
