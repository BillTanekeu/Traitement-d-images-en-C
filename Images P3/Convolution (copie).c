#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pixel
{
    int pix_red;
    int pix_green;
    int pix_blue;
} Pixel;


typedef struct A_PPMImage {
    char ppmType[3];
    Pixel** data;
   int width;
   int height;
   int maxValue;
} A_PPMImage;


void ignoreComments(FILE* fp)
{
    int ch;
    char line[100];
 
    // Ignore any blank lines
    while ((ch = fgetc(fp)) != EOF
           && isspace(ch))
        ;
 
    // Recursively ignore comments
    // in a ppm image commented lines
    // start with a '#'
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        ignoreComments(fp);
    }
    else
        fseek(fp, -1, SEEK_CUR);
}

bool openAppm(A_PPMImage* ppm,const char* filename)
{
    // Open the image file in the
    // 'read binary' mode
    FILE* ppmfile = fopen(filename, "r");
 
    // If file does not exist,
    // then return
    if (ppmfile == NULL) {
        printf("File does not exist\n");
        return false;
    }
 
    ignoreComments(ppmfile);
    fscanf(ppmfile, "%s",ppm->ppmType);
 
    // Check for correct ppm Binary
    // file type
    if (strcmp(ppm->ppmType, "P3")) {
        fprintf(stderr,"Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
 
    ignoreComments(ppmfile);
 
    // Read the image dimensions
    fscanf(ppmfile, "%d %d",&(ppm->width),&(ppm->height));
 
    ignoreComments(ppmfile);
 
    // Read maximum gray value
    fscanf(ppmfile, "%d", &(ppm->maxValue));
    ignoreComments(ppmfile);
 
    // Allocating memory to store
    // img info in defined struct
    ppm->data = malloc(ppm->height* sizeof(Pixel*));
    
 
    // verifie si l'image est bien en binaire
    if (ppm->ppmType[1] == '3') {
 
        //fgetc(ppmfile);
 
        for (int i = 0;i < ppm->height; i++) {
            
            ppm->data[i] = malloc(ppm->width* sizeof(Pixel));
            
 
            // If memory allocation
            // is failed
            if (ppm->data[i] == NULL  ) {
                fprintf(stderr,
                        "malloc failed\n");
                exit(1);
            }

            for (int j = 0; j < ppm->width; j++){
                fscanf(ppmfile, "%d", &ppm->data[i][j].pix_red);
                fscanf(ppmfile, "%d", &ppm->data[i][j].pix_green);
                fscanf(ppmfile, "%d", &ppm->data[i][j].pix_blue);
            }
            // lire les pixels(3 composantes) et stocker dans un tableau
            //fread(ppm->data[i], sizeof(Pixel ), ppm->width, ppmfile);
            }
        }
    

// Close the file

    fclose(ppmfile);
 
    return true;
}


A_PPMImage* Convolution(A_PPMImage* img,float kernel[3][3], int size, int seuil  ){
    float temp1 = 0.;
    float temp2 = 0.;
    float temp3 = 0.;
    A_PPMImage* result = malloc(sizeof(A_PPMImage)) ;
    result->data = calloc(img->height , sizeof(Pixel*));
   
   
    for (int t = 0; t < img->height ; t++){
        result->data[t] = calloc(img->width,sizeof(Pixel));
    } 
    strcpy(result->ppmType, img->ppmType);
    result->height = img->height;
    result->width = img->width;
    // initialisation de la plus grande valeur de l'image resultat
    int maxvalue = 0;
    
    // a et b permettent de parcourir le kernel
    int a,b;
    
    for (int  i = size/2; i < (img->height - size/2) ; i++)
    {
        for (int j = size/2; j < (img->width - size/2); j++)
        {
            a = 0;
            for (int k = (i-size/2); k < (i + size/2 )+1; k++)
            {   
                b = 0;
                for (int p = (j- size/2); p < (j + size/2)+1; p++)
                {
                    temp1 += img->data[k][p].pix_red  * kernel[a][b];
                    temp2 += img->data[k][p].pix_green  * kernel[a][b];
                    temp3 += img->data[k][p].pix_blue  * kernel[a][b];
                    b++;
                }
                a++;
                
            }
            // Activation RELU
            if (temp1< seuil)
            {
                temp1= 0;
            }

            if(temp2< seuil)
                temp2 =0;
            
            if (temp3< seuil)
            {
                temp3 = 0;
            }
            
            
            // convertir la pré-activation en entier 
            result->data[i][j].pix_red = (int) temp1;
            result->data[i][j].pix_green = (int) temp2;
            result->data[i][j].pix_green = (int) temp3;

            temp1 = 0.;
            temp2 = 0.;
            temp3 = 0.;
            // Mettre à jour la valeur max
            if (result->data[i][j].pix_red > maxvalue)
            {
                maxvalue = result->data[i][j].pix_red;
            }

            if (result->data[i][j].pix_green > maxvalue)
                maxvalue = result->data[i][j].pix_green;
        
            if (result->data[i][j].pix_blue > maxvalue)
                maxvalue = result->data[i][j].pix_blue;
            
            
        }
        
    }
    result->maxValue = maxvalue;
    return result;
    
}



void new_AppmImage(const char* name, A_PPMImage* img){

    FILE* fic = fopen(name, "wb+");
    fprintf(fic,"%s\n", img->ppmType);
    fprintf(fic, "# created by Tanekeu Bill Jordan\n");
    fprintf(fic,"%d %d\n", img->width, img->height);
    fprintf(fic,"%d\n", img->maxValue);

    for (int i = 0; i < img->height; i++)
    {
        
        for (int j = 0; j< img->width; j++){
            fprintf(fic, "%d\n", img->data[i][j].pix_red );
            fprintf(fic, "%d\n", img->data[i][j].pix_green );
            fprintf(fic, "%d\n", img->data[i][j].pix_blue );
        }
       
        
    }
    

    fclose(fic);

}
 
int main(int argc, char const* argv[])
{
    A_PPMImage* pgm = malloc(sizeof(A_PPMImage));
    
    
    A_PPMImage* result;
    const char* ipfile;
    const char* name_result;
    
    

    //int s_min, s_max;

    char n_result[50];
    char file[50];

    float m[3][3] = {
            {-1,-1,-1},
            {0,0,0},
            {1,1,1},
        }  ; 
    
    int seuil = 0;
    int size = 3;
    if (argc == 3){
        
        ipfile = argv[1];
        name_result = argv[2];
        
    
       
        //printf("ipfile: %s, name: %s, lumimance : %s, l : %d", ipfile, name_result, luminance, l);

        if (openAppm(pgm, ipfile)){
            printf("****Convolution (3,3)*****\n");
            result = Convolution(pgm,m, size, seuil );
            new_AppmImage(name_result,result);
           printf("\n*******Opération effectuée avec success*******\n");
        }
    }
    else{
        printf("****Convolution (3,3)*****\n");
        printf("Entrer le nom du fichier (moins de 50 caracteres):\t");
        scanf("%s",file);
        printf("\n Entrer un nom pour le nouveau fichier (moins de 50 caracteres):\t ");
        scanf("%s",n_result);

        const char* name = n_result;
        const char* f = file;


        if (openAppm(pgm, f)){
            result = Convolution(pgm, m,size, seuil);
            new_AppmImage(name,result);
            printf("\nOpération effectuée avec success\n");
        }

    }

    free(pgm);
    
    
    return 0;
}