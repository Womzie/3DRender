#include <stdio.h>
 
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <math.h>
#include <string.h>


#define ARRAY_SIZE 20;

const size_t NUM_PIXELS_TO_PRINT = 25U;
int writeJPG(FILE *fptr, unsigned char *data, int w, int h){
    int c = 3;
    int x,y;

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

int writePNG(FILE *fptr,unsigned char *data, int w, int h){
    int c = 4;
    int x,y;
    
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
    char xyzName[100];
    FILE *fptr;
    
    char name[100];
    
    printf("enter name of .jpg or .png file\n") ;
    scanf("%s",name) ;


    unsigned char *data = stbi_load(name, &width, &height, &comp, 0);
    if (data) {
        
        printf("Name of new .XYZ file: ");
        scanf("%s", xyzName);
        fptr = fopen(xyzName, "w");
        if(fptr == NULL){
            printf("ERROR!");
            exit(1);
        }
        printf("width = %d, height = %d, comp = %d (channels)\n", width, height, comp);
        
        if(comp == 3){
            writeJPG(fptr, data, width, height);
        }
        else if(comp == 4){
            writePNG(fptr, data, width, height);
        }

    }
    return 0;
}