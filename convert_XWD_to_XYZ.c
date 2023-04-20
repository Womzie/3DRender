#include "xwd_tools_03.c"

int main(){
    char nameA[100];
    int idA;
    int widthA,heightA ;
    FILE *fptr;
    

    int d[2],w,h,x,y,e,q ;
    double rgb[3];

    printf("enter name of xwd file\n") ;
    scanf("%s",nameA) ;
    idA = init_xwd_map_from_file (nameA) ;
    if (idA == -1) { printf("failure\n") ;  exit(0) ; }
    e = get_xwd_map_dimensions(idA, d) ;
    if (e == -1) { printf("failure\n") ;  exit(0) ; }
    widthA = d[0] ; heightA = d[1] ;
    
    char xyzName[100];
    printf("Name of new .XYZ file: ");
    scanf("%s", xyzName);
    fptr = fopen(xyzName, "w");
    if(fptr == NULL){
        printf("ERROR!");
        exit(1);
    }
    int pixels = widthA * heightA;
    fprintf(fptr,"%d %d \n", widthA, heightA);
    
    


    for(x = 0; x < widthA; x++){
        for(y = 0; y < heightA; y++){
            e = get_xwd_map_color(idA, x, y, rgb);
            if (e == -1) { printf("failure\n") ;  exit(0) ; }
            fprintf(fptr, "%lf %lf %lf\n", rgb[0], rgb[1], rgb[2]);
            
        }
    }

    for(x = 0; x < widthA; x++){
        for(y = 0; y < heightA; y++){
            fprintf(fptr,"%d %d\n", x, y);
        }
    
    }


    
    fclose(fptr);

    return 0;
}