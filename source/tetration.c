#include "builder/builder.c"
#include "tetration.h"






/**
 *
 *  Main program
 *
 */

float modulus(float a, float b) {
	return sqrt(a*a+b*b);
}



int main() {
	
	const userinfo_t usr_fractal = {
        .width  = USR_WIDTH  * USR_AAF,
    	.height = USR_HEIGHT * USR_AAF,
    	.center_x = USR_CENTER_X,
    	.center_y = USR_CENTER_Y,
    	.log_zoom = USR_LOGZOOM,
    	.std_diagonal = 10.0 * M_SQRT2
    };
	
	const float 
	    diagonal      = usr_fractal.std_diagonal / pow(2.0, usr_fractal.log_zoom),
		diag_to_width = diagonal / sqrt(1.0 + pow(usr_fractal.height / usr_fractal.width, 2)),		
		half_width    = diag_to_width * 0.5,
		half_height   = diag_to_width * (usr_fractal.height / usr_fractal.width) * 0.5;
	
	
	const sysinfo_t sys_fractal = {
        .x1 = usr_fractal.center_x - half_width,
        .x2 = usr_fractal.center_x + half_width,
        .y1 = usr_fractal.center_y - half_height,
        .y2 = usr_fractal.center_y + half_height,
        .pixel_step = diag_to_width / usr_fractal.width,
        .iterations = ITERATIONS
    };
	
	
	
	
	FILE *fp = fopen(FILE_RAWOUT, "wb");
	fseek(fp, 0, SEEK_SET);
	fwrite(&usr_fractal, sizeof(userinfo_t), 1, fp);
	
	int 
	    img_idx = 0,
        t = 0,
        r = 0;
    
	float
	    a, b, x, y, m, p, i, j,
	    *img         = NULL,
        angle        = 0.0, 
        maxAngle     = 0.0, 
        normMaxAngle = 0.0;
    
    img = malloc(usr_fractal.width * usr_fractal.height * sizeof(float));

	for(j = sys_fractal.y1 ; j < sys_fractal.y2 - (sys_fractal.pixel_step / 2) ; j += sys_fractal.pixel_step) {
    
		printf("%04d\n",t++);
        
		for(i = sys_fractal.x1 ; i < sys_fractal.x2 - (sys_fractal.pixel_step / 2) ; i += sys_fractal.pixel_step) {
        
			x = i;
			y = j;
			m = 0.5 * log(i*i + j*j);
			p = atan2(j,i);
            angle = 0;
			maxAngle = 0;
            
			for(r = 0 ; (r < sys_fractal.iterations) && (modulus(x,y) < DBL_MAX) ; r++) {
                
				a = exp(x*m - p*y);
				b = y*m + p*x;
				x = a * cos(b);
				y = a * sin(b);
                
                angle = fabs(atan(y/x));
                
                if(r > 0 && angle > maxAngle) 
                    maxAngle = angle;
                
			}
			normMaxAngle = maxAngle / M_PI_2;
			fwrite(&normMaxAngle, sizeof(float), 1, fp);
			*(img + (img_idx++)) = normMaxAngle;
		}
	}
	
	fclose(fp);
	
	fwrite(img, sizeof(float), 1, fp);
	builder_DataBuild(usr_fractal, img, BUILD_AAF, BUILD_COLORMAP);
	system("start " FILE_BITMAP);
	return 0;
}
