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

typedef struct A_PGMImage {
    char pgmType[3];
    int ** data;
    int width;
    int height;
    int maxValue;
} A_PGMImage;


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


A_PGMImage* Convolution(A_PPMImage* img,float kernel[3][3], int size, int seuil  ){
    float temp1 = 0.;
    float temp2 = 0.;
    float temp3 = 0.;
    float temp = 0.;
    A_PGMImage* result = malloc(sizeof(A_PGMImage)) ;
    result->data = calloc(img->height , sizeof(int*));
   
   
    for (int t = 0; t < img->height ; t++){
        result->data[t] = calloc(img->width,sizeof(int));
    } 
    strcpy(result->pgmType, "P2");
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
                    temp = temp1+temp2+temp3;
                    b++;
                }
                a++;
                
            }
            // Activation RELU
            if (temp< seuil)
            {
                temp= 0;
            }

            
            
            
            // convertir la pr??-activation en entier 
            result->data[i][j] = (int) temp;
            temp1 = 0.;
            temp2 = 0.;
            temp3 = 0.;
            temp = 0.;
            // Mettre ?? jour la valeur max
            if (result->data[i][j]> maxvalue)
            {
                maxvalue = result->data[i][j];
            }

            
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

void new_A_PGMimage( const char* name, A_PGMImage* img){

    FILE* fic = fopen(name, "w+");
    fprintf(fic,"%s\n", img->pgmType);

    //fputs(comment, fic);
    fprintf(fic,"%d %d\n", img->width, img->height);
    fprintf(fic,"%d\n", img->maxValue);

    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j< img->width; j++){
            fprintf(fic, "%d\n", img->data[i][j]);
        }
       
        //fwrite(img->data[i], sizeof(int), img->width, fic);
    }
    
    fclose(fic);
}
 
int main(int argc, char const* argv[])
{
    A_PPMImage* pgm = malloc(sizeof(A_PPMImage));
    
    
    A_PGMImage* result;
    const char* ipfile;
    const char* name_result;
    
    

    //int s_min, s_max;

    char n_result[50];
    char file[50];

    float m[3][3] = {
            {-1,-1,-1},
            {0,0,0},
            {2,2,2},
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
            new_A_PGMimage(name_result,result);
           printf("\n*******Op??ration effectu??e avec success*******\n");
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
            new_A_PGMimage(name,result);
            printf("\nOp??ration effectu??e avec success\n");
        }

    }

    free(pgm);
    
    
    return 0;
}