#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include "FPToolkit.c"
#include "M3d_matrix_toolsS.c"

#define sWIDTH 800
#define sHEIGHT 800


int numoo, gsize[5][2], tp[5];
double bb,bg,br, xpoints[2][810000], ypoints[2][810000],zpoints[2][810000] ;
int height[5], width[5];
typedef struct {
  double r ;
  double g ;
  double b ;
  double alpha;
} THING ;

THING thing[5][810000]  ;

int writePNG(unsigned char *data, int w, int h, int numob){
  int x,y, k ;
  
  size_t i = 0;
  k = 0;
  for(int y = 0; y < h; y++){
    
    for(int x = 0; x < w; x++){
      xpoints[numob][k] = x;
      ypoints[numob][k] = y;
      zpoints[numob][k] = 10;

      
      
      double hex = data[i];
      hex /= 255;
      //printf("%lf \n", hex);
      
      thing[numob][k].r = hex;
      i++;

      hex = data[i];
      hex /= 255;

      thing[numob][k].g = hex;
      i++;

      hex = data[i];
      hex /= 255;

      thing[numob][k].b = hex;
      i++;

      hex = data[i];
      thing[numob][k].alpha = hex;
      
      i++;
      k++;

      //printf("%d \n", i);
      
    }
  }
  
  xpoints[numob][k] = w/2;
  ypoints[numob][k] = h/2;
  //printf("%lf %lf \n", xpoints[numob][k], ypoints[numob][k]);
  zpoints[numob][k]=10;
  tp[numob] = k;
  //printf("%d \n", tp[numob]);
  return 0;
}


int writeJPG(unsigned char *data, int w, int h, int numob){
  int x,y, k ;
  
  size_t i = 0;
  k = 0;

  double hex;
  for(int y = 0; y < h; y++){
    
    for(int x = 0; x < w; x++){
      xpoints[numob][k] = x;
      ypoints[numob][k] = y;
      zpoints[numob][k] = 10;
      
      
      hex = data[i];
      hex /= 255;
      
      
      
      thing[numob][k].r = hex;

      if(thing[numob][k].r > 1){
	printf("%lf \n", hex);

      }
      
      i++;

      hex = data[i];
      hex /= 255;

      thing[numob][k].g = hex;
      i++;

      hex = data[i];
      hex /= 255;

      thing[numob][k].b = hex;
      i++;
      
      thing[numob][k].alpha = 1;
      
      
      k++;

      //printf("%d \n", i);
      
    }
  }
  
  xpoints[numob][k] = w/2;
  ypoints[numob][k] = h/2;
  //printf("%lf %lf \n", xpoints[numob][k], ypoints[numob][k]);
  zpoints[numob][k]=10;
  tp[numob] = k;
  //printf("%d \n", tp[numob]);
  return 0;
}


void read_file(char name[], int c){
  int comp, x, y, n;
  
  int ok = stbi_info(name, &width[c], &height[c], &n);
  if(ok == 0){
    printf("unsupported format");
    return;
  }
  unsigned char *data = stbi_load(name, &width[c], &height[c], &comp, 0);
  if(data){
    gsize[c][0] = width[c];
    gsize[c][0] = height[c];
  }

  if(comp == 3){
    writeJPG(data, width[c], height[c], c);
  }
  else if(comp == 4){
    writePNG(data, width[c], height[c], c);
  }
  else{
    printf("unsupported format, we only support jpgs");
  }
  

}



void draw(){

  double h,H,x,y,z,x1,y1,x2,y2;
  h=45*(M_PI/180);
  H = tan(h);

  //G_rgb(1,0,0);
  
  //printf("%d %d \n", numoo, tp[0]);
  for (int k = 0; k < numoo; k++){
    for (int i = 0; i < tp[k]; i++){

      if(thing[k][i].alpha > 0){
	x = xpoints[k][i];
	y = ypoints[k][i];
	z = zpoints[k][i]; //each layer should be farther along the z axis
	//20 is just a placeholder, we'll need to actually calucalte the right increment at some point

	//printf("%lf %lf %lf \n", x, y, z);

	
	y1=(y/z);
	x1=(x/z);
	x2=((sWIDTH/2)/H)*x1+(sWIDTH/2);
	y2=((sHEIGHT/2)/H)*y1+(sHEIGHT/2);

	x = x2;
	y = y2;
	// if (i==0) {printf("%lf %lf\n", x,y);}
	// if (i==1) {printf("%lf %lf\n", x,y);}
	// if (i==tp[k]-1) {printf("%lf %lf\n", x,y);}
	//if (i==tp[k]/2) {printf("%lf %lf\n", x,y);}


	
	
	G_rgb(thing[k][i].r,thing[k][i].g,thing[k][i].b);
	G_fill_circle(x,y,1);
      }
      
    }}

}


void adjust_points(int c){
  
  double t1[4][4], s1[4][4], t2[4][4],res[4][4],z[900],sx,sy,r[2], r1[4][4];
  M3d_make_identity(r1);
  M3d_make_translation(t1, -xpoints[c][tp[c]],-ypoints[c][tp[c]], -10);

  r1[1][1] = -1;

  
  //M3d_make_scaling(s1, gsize[c][0]/800.0,gsize[c][1]/800.0,1);

  // M3d_make_translation(t2, 400,400, 10); //right translation for 2d
  M3d_make_translation(t2, 0,0, 10); //right translation for 3d
  
  sx = 0.02;
  sy = 0.02;
  // printf("%lf %lf\n", sx,sy);
  M3d_make_scaling(s1, sx,sy,1);
  //M3d_make_scaling(s1, 0.01,0.01,1);

  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],t1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],r1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],s1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],t2,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);

}





int main(int argc, char **argv) {

  char list[10][100];
  int i;
  double br, bg,bb;
  i=1;

  if (argc>5){printf("too many objects.\n"); exit(0);}

  while (i < argc){
    read_file(argv[i],i-1);
    adjust_points(i-1);
    //printf("adjust points, succeeds \n");
    i++;
  }
  numoo = argc-1;
  
  // printf("what is the background color?\n");
  // scanf("%lf %lf %lf", &br,&bg,&bb);


  G_init_graphics (sWIDTH,sHEIGHT);
  G_rgb(1,0,0);
  G_clear();
  draw();
  G_wait_key();
  //will eventually need to rotate 2nd and 3rd layers about the z axis at the right rate of speed.


}