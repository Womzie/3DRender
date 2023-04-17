#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include "FPToolkit.c"
#include "M3d_matrix_toolsS.c"

#define sWIDTH 800
#define sHEIGHT 800

#define PI 3.14159


int numoo, gsize[5][2], tp[5];
double bb,bg,br, xpoints[4][810000], ypoints[4][810000],zpoints[4][810000],psize[4],specpow,ambient, h, mdiffuse;
int height[5], width[5], L[3];
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
      zpoints[numob][k] = 2000 - (numob*500);

      
      
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
  zpoints[numob][k]= 2000 - (numob*500);
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
      zpoints[numob][k] = 2000 - (numob*500);
      
      
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
  zpoints[numob][k]= 2000 - (numob*500);
  tp[numob] = k;
  
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
    gsize[c][1] = height[c];
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

void find_vector(double x1,double y1, double z1, double x2, double y2, double z2,double v[]){
v[0]= x2-x1;
v[1]= y2-y1;
v[2]= z2-z1;

}

/*
int normal(double x,double y, double z, double res[]){
double v1[3];

find_vector(x,y,z,x,y,0,v1);

//M3d_x_product(res,v1,v1);
//printf("%lf %lf %lf\n", res[0],res[1],res[2]);
}
*/

int normal(double x,double y, double z, double res[]){
double v1[3],v2[3];

// find_vector(x,y,z,x-1,y-1,z,v1);
// find_vector(x,y,z,x+1,y+1,z,v2);

// M3d_x_product(res,v1,v2);
 res[0]=res[1]=0;
 res[2]=1;
}

double length(double v[]){
return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

void unitVector(double res[]){
double len = length(res);
res[0]/=len;
res[1]/=len;
res[2]/=len;

}

double intensity(double x,double y, double z){
double specular, diffuse, lv[3], len, eye[3],refl[3];
double norm[3], cent[3];
cent[0]=x;
cent[1]=y;
cent[2]=z;

normal(x,y,z,norm);
find_vector(L[0],L[1],L[2],cent[0],cent[1],cent[2],lv);
find_vector(0,0,0,cent[0],cent[1],cent[2],eye);
unitVector(norm);
unitVector(eye);
unitVector(lv);
refl[0]=((2*M3d_dot_product(norm, lv))*norm[0])-lv[0];
refl[1]=((2*M3d_dot_product(norm, lv))*norm[1])-lv[1];
refl[2]=((2*M3d_dot_product(norm, lv))*norm[2])-lv[2];

if (M3d_dot_product(norm,lv)*M3d_dot_product(norm,eye) < 0){ return ambient;}

if (M3d_dot_product(norm,lv) < 0 && M3d_dot_product(norm,eye) < 0) {

norm[0]*=-1;
norm[1]*=-1;
norm[2]*=-1;
find_vector(L[0],L[1],L[2],cent[0],cent[1],cent[2],lv);
find_vector(0,0,0,cent[0],cent[1],cent[2],eye);
unitVector(refl);
unitVector(eye);
unitVector(lv);
refl[0]=((2*M3d_dot_product(norm, lv))*norm[0])-lv[0];
refl[1]=((2*M3d_dot_product(norm, lv))*norm[1])-lv[1];
refl[2]=((2*M3d_dot_product(norm, lv))*norm[2])-lv[2];

}


diffuse = mdiffuse*(M3d_dot_product(norm, lv));
specular = pow(M3d_dot_product(eye, refl),specpow);
specular = (1-(mdiffuse+ambient))*specular;
return ambient+specular+diffuse;

}

void color(double intn, int k, int i, double rgb[]){
 double r,g,b,border,s;
 r=thing[k][i].r;
 g=thing[k][i].g;
 b=thing[k][i].b;

if (k==0){intn -= 0.07;}
  if (k==1){intn -= 0.05;}
  if (k==2){intn -= 0.03;}

border = ambient + mdiffuse;
s = border - intn;
s=s/border;
if (s<0){
s = 1-s;
rgb[0]=r*s;
rgb[1]=g*s;
rgb[2]=b*s;

}
else {
s *= -1;
s+=1;
rgb[0]=r*s;
rgb[1]=g*s;
rgb[2]=b*s;

}


}

void draw(){

  double h,H,x,y,z,x1,y1,x2,y2,rgb[3];
  h=45*(PI/180);
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


	
  double intn = intensity(xpoints[k][i], ypoints[k][i], zpoints[k][i]);
  //printf("%lf\n", intn);
 color(intn, k,i, rgb);
 G_rgb(rgb[0],rgb[1],rgb[2]);

	//G_rgb(thing[k][i].r,thing[k][i].g,thing[k][i].b);
	//G_fill_circle(x,y, psize[k]); //psize = longer side of window / longer side of image. Why is math off??
    G_fill_rectangle(x-(psize[k]/2),y-(psize[k]/2),psize[k],psize[k]);
      }
      
    }
    
  }

}

void find_range(int c, double r[]){
  double x,y,z,x1,y1,x2,y2,point1[2],point2[2],H,h;
  h=45*(PI/180);
  H = tan(h);
  x = xpoints[c][0];
  y = ypoints[c][0];
  z = zpoints[c][0]; 
  y1=(y/z);
  x1=(x/z);
  x2=((sWIDTH/2)/H)*x1+(sWIDTH/2);
  y2=((sWIDTH/2)/H)*y1+(sWIDTH/2);

  point1[0] = x2;
  point1[1] = y2;

  x = xpoints[c][tp[c]-1];
  y = ypoints[c][tp[c]-1];
  z = zpoints[c][tp[c]-1]; 
  y1=(y/z);
  x1=(x/z);
  x2=((sWIDTH/2)/H)*x1+(sWIDTH/2);
  y2=((sWIDTH/2)/H)*y1+(sWIDTH/2);

  point2[0] = x2;
  point2[1] = y2;
  r[0] = point2[0]-point1[0];
  r[1] = point2[1]-point1[1];


}



void adjust_points(int c){
  
  double t1[4][4], s1[4][4], t2[4][4],res[4][4],z[900],sx,sy,r[2], r1[4][4];
  M3d_make_translation(t1, -xpoints[c][tp[c]],-ypoints[c][tp[c]], -zpoints[c][tp[c]]);
  M3d_make_identity(r1);

  r1[1][1] = -1;
  
  find_range(c, r);

  if (r[0]>r[1]){
    sx = sy = sWIDTH/r[0]; 
    psize[c] = ceil(sWIDTH/(gsize[c][0]*1.0)); //size of pixel
    /*right now all images have their own scaling so they end up the same size regardless of
    difference in numbers of pixels, once we move to watch images we'll want all images to be
    scaled the same amount. But if they are actually all the same size (same number of pixels), 
    then they'll already be scaled the same amount, so no changes in the code are necessary.*/ 
  }
  else {
    sx = sy = sWIDTH/r[1];
    psize[c] = ceil(sWIDTH/(gsize[c][1]*1.0));
  }


  // M3d_make_translation(t2, 400,400, 10); //right translation for 2d
  M3d_make_translation(t2, 0,0, zpoints[c][tp[c]]); //right translation for 3d

  // printf("%lf %lf\n", sx,sy);
  M3d_make_scaling(s1, sx,sy,1);
  //M3d_make_scaling(s1, 0.01,0.01,1);

  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],t1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],r1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
  
  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],s1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
  M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],t2,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);

}

void minute(){
int c = 2;
int temp;
double rotate[4][4], t1[4][4],t2[4][4],cs,sn;
double r = -2 * PI/180;
cs = cos(r); sn = sin(r);
M3d_make_translation(t1, -xpoints[c][tp[c]],-ypoints[c][tp[c]],-zpoints[c][tp[c]]);

M3d_make_z_rotation_cs(rotate, cs,sn);
M3d_make_translation(t2, xpoints[c][tp[c]],ypoints[c][tp[c]],zpoints[c][tp[c]]);

M3d_mat_mult(t2,t2,rotate);
M3d_mat_mult(t2, t2,t1);
M3d_mat_mult_points(xpoints[c],ypoints[c],zpoints[c],t2,xpoints[c],ypoints[c],zpoints[c],tp[c]+1);

}

void hour(){
  int c = 1;
int temp;
double rotate[4][4], t1[4][4],t2[4][4],cs,sn;
double r = (-2.0/12.0) * PI/180;
cs = cos(r); sn = sin(r);
M3d_make_translation(t1, -xpoints[c][tp[c]],-ypoints[c][tp[c]],-zpoints[c][tp[c]]);

M3d_make_z_rotation_cs(rotate, cs,sn);
M3d_make_translation(t2, xpoints[c][tp[c]],ypoints[c][tp[c]],zpoints[c][tp[c]]);

M3d_mat_mult(t2,t2,rotate);
M3d_mat_mult(t2, t2,t1);
M3d_mat_mult_points(xpoints[c],ypoints[c],zpoints[c],t2,xpoints[c],ypoints[c],zpoints[c],tp[c]+1);

}



int main(int argc, char **argv) {

  char list[10][100];
  int i, key;
  double br, bg,bb;
  i=1;
  h = 45 *(M_PI/180);
  h = tan(h);
  L[0]=0;
  L[1]=0;
  L[2]=-500;
  specpow=2;
  mdiffuse=0.5;
  ambient=0.3;


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
  while (key!='q'){
    G_rgb(1,0,0);
    G_clear();
    hour();
    minute();
    draw();
    key = G_wait_key();

  }
  
}
