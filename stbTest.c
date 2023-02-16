#include <stdio.h>
 
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <math.h>
#include <string.h>


#define ARRAY_SIZE 20;

const size_t NUM_PIXELS_TO_PRINT = 25U;
int writeJPG(unsigned char *data, int w, int h){
    int c = 3;
    int x,y;
    FILE *fptr;
    char xyzName[100];
    printf("Name of new .XYZ file: ");
        scanf("%s", xyzName);
        fptr = fopen(xyzName, "w");
        if(fptr == NULL){
            printf("ERROR!");
            exit(1);
        }
    fprintf(fptr,"%d %d \n", w, h);
    for (size_t i = 0; i < w * h * c; i+=c) {
            
        for(size_t j = 0; j < c; j++){
            double hex = data[i + j];
            hex /= 255;
            
            fprintf(fptr, "%lf %s", hex, ((j + 1) % c) ? "" : "\n");  
        }
        }
        
        for(x = 0; x < w; x++){
        for(y = 0; y < h; y++){
            fprintf(fptr,"%d %d\n", x, y);
            }
        }   

    fclose(fptr);

    return 0;
}

int writePNG(unsigned char *data, int w, int h){
    int c = 4;
    int x,y;
    FILE *fptr;
    char xyzName[100];
    printf("Name of new .XYZ file: ");
        scanf("%s", xyzName);
        fptr = fopen(xyzName, "w");
        if(fptr == NULL){
            printf("ERROR!");
            exit(1);
        }
    fprintf(fptr,"%d %d \n", w, h);
    for (size_t i = 0; i < w * h * c; i+=c) {
            
        if(data[i + c -1] != 0){
            for(size_t j = 0; j < c; j++){
                if(j != c-1){
                    double hex = data[i + j];
                    hex /= 255;
                    fprintf(fptr, "%lf %s", hex, ((j + 1) % (c-1)) ? "" : "\n"); 
                }
                 
            }
        }
    }
        
    for(x = 0; x < w; x++){
    for(y = 0; y < h; y++){
        fprintf(fptr,"%d %d\n", x, y);
        }
    }   

    fclose(fptr);

    
    return 0;
}

int main(void) {
    int width, height, comp, x, y;
    double rgb[3];
    
    char name[100];
    
    printf("enter name of jpg file\n") ;
    scanf("%s",name) ;


    unsigned char *data = stbi_load(name, &width, &height, &comp, 0);
    if (data) {
        
        printf("width = %d, height = %d, comp = %d (channels)\n", width, height, comp);
        
        if(comp == 3){
            writeJPG(data, width, height);
        }
        else if(comp == 4){
            writePNG(data, width, height);
        }

    }
    return 0;
}