#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "FPToolkit.c"
#include "M3d_matrix_toolsS.c"

int numoo, gsize[5][2], tp[5];
double bb,bg,br, xpoints[2][640000], ypoints[2][640000],zpoints[2][640000] ;

typedef
struct {
  double r ;
  double g ;
  double b ;
}
THING ;

THING thing[5][640000]  ;


void read_file(FILE *w, int c){
double tr,tg,tb;
//take in number of pixels (should be first thing in each file, x pixels then y pixels)
fscanf(w, "%d", &gsize[c][0]);
fscanf(w, "%d", &gsize[c][1]);
//gsize[c][0] = gsize[c][1] = 600;

int k = 0;
//go through each pixel and take the rgb info and add it to list
for (int i = 0; i < gsize[c][0]; i++){
	for (int j = 0; j < gsize[c][1]; j++){
	fscanf(w, "%lf %lf %lf", &tr, &tg, &tb);
	thing[c][k].r = tr;
	thing[c][k].g = tg;
	thing[c][k].b = tb;
	xpoints[c][k]=i;
	ypoints[c][k]=j;
	zpoints[c][k]=10;
  k++;
	}
}

xpoints[c][k] = gsize[c][0]/2;
ypoints[c][k] = gsize[c][1]/2;
zpoints[c][k]=10;
tp[c]=k;

}

void draw(){

double h,H,x,y,z,x1,y1,x2,y2;
h=45*(M_PI/180);
H = tan(h);

G_rgb(1,0,0);


for (int k = 0; k < numoo; k++){
	for (int i = 0; i < tp[k]; i++){
			x = xpoints[k][i];
			y = ypoints[k][i];
			z = zpoints[k][i]; //each layer should be farther along the z axis
			//20 is just a placeholder, we'll need to actually calucalte the right increment at some point
			y1=(y/z);
			x1=(x/z);
			x2=(400/H)*x1+400;
			y2=(400/H)*y1+400;

			  x = x2;
			  y = y2;
// if (i==0) {printf("%lf %lf\n", x,y);}
			// if (i==1) {printf("%lf %lf\n", x,y);}
			// if (i==tp[k]-1) {printf("%lf %lf\n", x,y);}
			//if (i==tp[k]/2) {printf("%lf %lf\n", x,y);}
			G_rgb(thing[k][i].r,thing[k][i].g,thing[k][i].b);
			G_fill_circle(x,y,4);
		}}

}
void find_range(int c, double r[]){
	double x,y,z,x1,y1,x2,y2,point1[2],point2[2],H,h;
	h=45*(M_PI/180);
	H = tan(h);
	x = xpoints[c][0];
	y = ypoints[c][0];
	z = zpoints[c][0]; 
	y1=(y/z);
	x1=(x/z);
	x2=(400/H)*x1+400;
	y2=(400/H)*y1+400;

	point1[0] = x2;
	point1[1] = y2;

	x = xpoints[c][tp[c]-1];
	y = ypoints[c][tp[c]-1];
	z = zpoints[c][tp[c]-1]; 
	y1=(y/z);
	x1=(x/z);
	x2=(400/H)*x1+400;
	y2=(400/H)*y1+400;

	point2[0] = x2;
	point2[1] = y2;
	r[0] = point2[0]-point1[0];
	r[1] = point2[1]-point1[1];


}
void adjust_points(int c){
double t1[4][4], s1[4][4], t2[4][4],res[4][4],z[900],sx,sy,r[2];
M3d_make_translation(t1, -xpoints[c][tp[c]],-ypoints[c][tp[c]], -10);
//M3d_make_scaling(s1, gsize[c][0]/800.0,gsize[c][1]/800.0,1);

// M3d_make_translation(t2, 400,400, 10); --right translation for 2d
M3d_make_translation(t2, 0,0, 10); //right translation for 3d
find_range(c, r);
if (r[0]>r[1]){
sx = 800/r[0];
sy = 800/r[0];
}
else {
sx = 800/r[1];
sy = 800/r[1];
}
// printf("%lf %lf\n", sx,sy);
M3d_make_scaling(s1, sx,sy,1);
//M3d_make_scaling(s1, 0.01,0.01,1);

M3d_mat_mult_points(xpoints[c],ypoints[c], zpoints[c],t1,xpoints[c],ypoints[c],zpoints[c], tp[c]+1);
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
FILE *w;
w=fopen(argv[i], "r");
if (w==NULL){printf("unable to open file.\n"); exit(0);}
read_file(w,i-1);
adjust_points(i-1);
i++;
}
numoo = argc-1;

// printf("what is the background color?\n");
// scanf("%lf %lf %lf", &br,&bg,&bb);


G_init_graphics (800,800);
G_rgb(1,0,0);
G_clear();
draw();
G_wait_key();
//will eventually need to rotate 2nd and 3rd layers about the z axis at the right rate of speed.

