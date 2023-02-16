#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "FPToolkit.c"

int numoo, gsize[5][2];
double bb,bg,br;

typedef
struct {
  double r ;
  double g ;
  double b ;
}
THING ;

THING thing[5][800][800]  ;


int read_file(FILE *w, int c){
double tx,ty,tz;
tx=ty=tz=0;
int j =0;
//take in number of pixels (should be first thing in each file, x pixels then y pixels)
fscanf(w, "%d", &gsize[c][0]);
fscanf(w, "%d", &gsize[c][1]);

//go through each pixel and take the rgb info and add it to list
for (int i = 0; i < gsize[c][0]; i++){
	for (int j = 0; j < gsize[c][1]; j++){
	fscanf(w, "%lf %lf %lf", &tr, &tg, &tb);
	thing[c][i][j].r = tr;
	thing[c][i][j].g = tg;
	thing[c][i][j].b = tb;
	}
}


}

void draw(){

double h,H,x,y,z,x1,y1,x2,y2;
h=45*(M_PI/180);
H = tan(h);

for (int k = 0; k < numoo; k++){
	for (int i = 0; i < gsize[c][0]; i++){
		for (int j = 0; j < gsize[c][1]; j++){
			//don't draw it if it's a pixel that's the background color
			if (thing[k][i][j].r != br && thing[k][i][j].g != bg && thing[k][i][j].b != bb){
			x = i;
			y = j;
			z = (k*20) + 20; //each layer should be farther along the z axis
			//20 is just a placeholder, we'll need to actually calucalte the right increment at some point
			y1=(y/z);
			x1=(x/z);
			x2=(400/H)*x1+400;
			y2=(400/H)*y1+400;

			x = x2;
			y = y2;
			
			G_rgb(thing[k][i][j].r,thing[k][i][j].g,thing[k][i][j].b);
			G_point(x,y);
		}
		}
	}

}


}





int main(int argc, char **argv) {


char list[10][100];
int i,c;
double br, bg,bb;
i=1;

if (argc>5){printf("too many objects.\n"); exit(0);}

while (i < argc){
FILE *w;
w=fopen(argv[i], "r");
if (w==NULL){printf("unable to open file.\n"); exit(0);}
read_file(w,i-1);
i++;
}
numoo = argc;

printf("what is the background color?\n")
scanf("%lf %lf %lf", &br,&bg,&bb);


G_init_graphics (800,800);
G_rgb(0,0,0);
G_clear();
draw();
G_wait_key();
//will eventually need to rotate 2nd and 3rd layers about the z axis at the right rate of speed.


}