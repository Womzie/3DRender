#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include  "FPToolkit.c"
#include "M2d_matrix_toolsS.c"



double mlist[10][59000], y[10][59000],x[10][59000], wcenter[2], wx[59000], wy[59000], polygony[10][59000],polygonx[10][59000], r[10][57500], g[10][57500],b[10][57500], cx[10], cy[10], f[10];
int psize[10][57500], con[10][57500][20];
int npt[10],npg[10], wsize,wsw;

int read_file(FILE *w, int c){
int j =0;
fscanf(w, "%d", &npt[c]);

for (int i = 0; i < npt[c]; i++){
fscanf(w, "%lf %lf", &x[c][i], &y[c][i]);
}

fscanf(w, "%d", &npg[c]);

while (j < npg[c]){
fscanf(w, "%d", &psize[c][j]);
for (int k = 0; k < psize[c][j]; k++){
fscanf(w, "%d", &con[c][j][k]);
int x = con[c][j][k];
}
j++;
}

for(int k =0; k< npg[c]; k++){
fscanf(w, "%lf %lf %lf", &r[c][k], &g[c][k], &b[c][k]);
}

}

int build_poly(int c){

for (int k = 0; k < npg[c]; k++ ){

for (int m = 0; m < psize[c][k]; m++){
polygonx[c][m] = x[c][con[c][k][m]];
polygony[c][m] = y[c][con[c][k][m]];
}
}

}



void window_center(){
wcenter[0]=0;
wcenter[1]=0;
for (int i =0; i < wsize; i++){
wcenter[0] += wx[i];
wcenter[1] += wy[i];
}
wcenter[0]=wcenter[0]/wsize;
wcenter[1]=wcenter[1]/wsize;

}


int in_out (int i, double P[2]){

double b, c,a,cy,cx, cv, pv,cntr[2];
cx=cy=0;
int j;
j = i+1 ; if (j == wsize) j = 0 ;


a=  (wy[j]-wy[i]);
b= -(wx[j]-wx[i]);
c= -b*wy[i]-a*wx[i];



cv = a*wcenter[0] + b*wcenter[1] + c;
pv =  a*P[0] + b*P[1] + c;

if (cv*pv < 0){
return 0;
}

return 1;

}

int intersect_2_lines (double A[2], double B[2],
                       double C[2], double D[2],
                       double intersection[2]){
  double a1,b1,a2,b2,c1,c2;
  
  a1=-(A[1]-B[1]);
  b1=A[0]-B[0];
  a2=-(C[1]-D[1]);
  b2=C[0]-D[0];
  c1= -b1*A[1]-a1*A[0];
  c2= -b2*C[1]-a2*C[0];
  
  if ((a1*b2-a2*b1)==0){return 0;}
  
  intersection[0]=(c1*b2-c2*b1)/(a2*b1-a1*b2);
  intersection[1]= (a1*intersection[0] + c1)/-b1;
  //y = (a1*c2-a2*c1)/(a2*b1-a1*b2)
  return 1;
  
  
}

int  window (double px[], double py[], int psize)

{
int signal[2],k,si,pi,m;
double p[2],q[2], pwx[100],pwy[100],a[2],b[2],c[2],d[2],intersection[2];
pi=0;

for (int i = 0; i < wsize; i++){
for (int j = 0; j < psize; j++){
k=j-1; if (k== -1){k=psize-1;}
//k=0=q
//j=1=p
m= i+1; if (m==wsize){m=0;}
p[0]=px[j]; p[1]=py[j];
q[0]=px[k]; q[1]=py[k];
signal[0] = in_out(i,q);
signal[1] = in_out(i,p);


//good to good
if (signal[0] == 1 && signal[1] == 1 ) {pwx[pi]=px[j]; pwy[pi]=py[j]; pi++;}
//good to bad
if (signal[0] == 1 && signal[1] == 0 ) {
a[0]=px[k]; a[1]=py[k];
b[0]=px[j]; b[1]=py[j];
c[0]=wx[i]; c[1]=wy[i];
d[0]=wx[m]; d[1]=wy[m];
si = intersect_2_lines(a,b,c,d, intersection);
if (si == 1){
pwx[pi]= intersection[0]; pwy[pi] = intersection[1];
pi++; }

else {pwx[pi]=px[j]; pwy[pi]=py[j];pi++;}
 }
//bad to good
if (signal[0] == 0 && signal[1] == 1 ) {
a[0]=px[k]; a[1]=py[k];
b[0]=px[j]; b[1]=py[j];
c[0]=wx[i]; c[1]=wy[i];
d[0]=wx[m]; d[1]=wy[m];
si = intersect_2_lines(a,b,c,d, intersection);
if (si == 1){
pwx[pi]= intersection[0]; 
pwy[pi] = intersection[1];pi++;
}
else {
pwx[pi]=px[j]; pwy[pi]=py[j]; pi++;}

pwx[pi]=px[j];
pwy[pi]=py[j];
pi++;
} 

}
  for(int r = 0; r<pi; r++){
px[r]=pwx[r];
py[r]=pwy[r];
}
psize=pi;

if (i < wsize-1) {pi = 0;}

}
return pi;
}

int find_center(int c){
double gx,gy,lx,ly;

gx=x[c][0]; lx=x[c][0];
gy=y[c][0]; ly=y[c][0];

for (int i  = 1; i < npt[c]; i++){
if (gx < x[c][i]){gx=x[c][i];}
if (lx > x[c][i]){lx=x[c][i];}
if (gy < y[c][i]){gy=y[c][i];}
if (ly > y[c][i]){ly=y[c][i];}
}

double xr = gx-lx;
double yr= gy-ly;

cx[c] = (lx + (xr/2));
cy[c] = (ly + (yr/2));
f[c] = ((400/xr)+(400/yr))/2; 
}

int adjust_points(int c){
double t1[3][3], s1[3][3], t2[3][3],res[3][3];

M2d_make_translation(t1, -cx[c],-cy[c]);
M2d_make_scaling(s1, f[c],f[c]);
M2d_make_translation(t2, 400,400);

M2d_mat_mult(res,t2,s1);
M2d_mat_mult(res,res,t1);
M2d_mat_mult_points(x[c],y[c],res,x[c],y[c],npt[c]);
}

int draw(int c){
int pi=0;
G_rgb(0,0,0);
G_clear();
G_rgb(0,1,0);
G_fill_rectangle(0,0,800,20);
G_polygon(wx,wy,wsize);
for (int k = 0; k < npg[c]; k++ ){
for (int m = 0; m < psize[c][k]; m++){

polygonx[c][m] = x[c][con[c][k][m]];
polygony[c][m] = y[c][con[c][k][m]];
}

pi=psize[c][k];
if (wsw == 1){pi = window(polygonx[c], polygony[c],psize[c][k]);}

G_rgb(r[c][k],g[c][k],b[c][k]);
G_fill_polygon(polygonx[c], polygony[c],pi);

}
}

int rotate(int c, double degrees){
int temp;
double rotate[3][3], t1[3][3],t2[3][3];
double r = degrees * M_PI/180;

M2d_make_translation(t1, -400,-400);
M2d_make_rotation(rotate, r); 
M2d_make_translation(t2, 400,400);

M2d_mat_mult(t2,t2,rotate);
M2d_mat_mult(t2, t2,t1);
M2d_mat_mult_points(x[c],y[c],t2,x[c],y[c],npt[c]);

}



int stretch(int c, double dx, double dy){
double s[3][3];
M2d_make_scaling(s, dx,dy);
M2d_mat_mult_points(x[c], y[c], s, x[c],y[c],npt[c]);

}

int translate(int c, int dx, int dy){
double t[3][3];
M2d_make_translation(t, dx,dy);
M2d_mat_mult_points(x[c], y[c], t, x[c],y[c],npt[c]);
}


int main(int argc, char **argv) {
wsize = 0; 
wsw=0;
char list[10][100];
int noo,i,c, key, num;
double point[2];
c=0;
i=1;
num= 0;
noo = argc;
key = 'a';

if (argc>10){printf("too many objects.\n"); exit(0);}

while (i < argc){
FILE *w;
w=fopen(argv[i], "r");
if (w==NULL){printf("unable to open file.\n"); exit(0);}
read_file(w,c);
find_center(c);
adjust_points(c);
c++;
i++;
}

G_init_graphics (800,800);


draw(num);

G_wait_click(point);
while (point[1] < 20){G_wait_click(point);}

while (point[1] > 20){
wx[wsize]= point[0];
wy[wsize]= point[1];
wsize++;
G_wait_click(point);
}
wsw= 1;
G_rgb(0,1,0);
G_polygon(wx,wy,wsize);
window_center();

while (key != 'q'){
key = G_wait_key();
if (key == 'r'){rotate(num,2);}
if (key == 's'){stretch(num, 0.9,0.9);}
if (key == 't'){translate(num,5,5);}
if (key >='0' && key < noo + '0'-1){
num = key-'0';}
//window(num);
draw(num);
}


}

