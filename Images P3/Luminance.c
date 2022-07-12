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



A_PPMImage* LuminanceAppm(int valeur, A_PPMImage* img)
{
     //unsigned int** result;
     int max_val = 255;
     int min_val = 0;
     for (int i =0; i< img->height; i++){
        
         for (int j = 0; j < img->width; j++)
         {
            if(img->data[i][j].pix_red + valeur > max_val  ){
                img->data[i][j].pix_red = max_val;
            }
            else if((img->data[i][j].pix_red + valeur) < min_val){
                img->data[i][j].pix_red = min_val;
            }
            else{
                img->data[i][j].pix_red = img->data[i][j].pix_red + valeur;

            }

            if(img->data[i][j].pix_green + valeur > max_val  ){
                img->data[i][j].pix_green = max_val;
            }
            else if((img->data[i][j].pix_green + valeur) < min_val){
                img->data[i][j].pix_green = min_val;
            }
            else{
                img->data[i][j].pix_green = img->data[i][j].pix_green + valeur;

            }

            if(img->data[i][j].pix_blue + valeur > max_val  ){
                img->data[i][j].pix_blue = max_val;
            }
            else if((img->data[i][j].pix_blue + valeur) < min_val){
                img->data[i][j].pix_blue = min_val;
            }
            else{
                img->data[i][j].pix_blue = img->data[i][j].pix_blue + valeur;

            }        


         }


     }

    return img;
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
    A_PPMImage* ppm = malloc(sizeof(A_PPMImage));
    
    
    A_PPMImage* result;
    const char* ipfile;
    
    const char* name_result;
    const char* luminance;
    

    int lum;

    char n_result[50];
    char file[50];
    if (argc == 4){
        
        ipfile = argv[1];
        name_result = argv[2];
        luminance = argv[3];
    
        lum = atoi(luminance);
        //printf("ipfile: %s, name: %s, lumimance : %s, l : %d", ipfile, name_result, luminance, l);

        printf("\nip file : %s\n", ipfile);
        if (openAppm(ppm, ipfile)){
            result = LuminanceAppm(lum,ppm);
            new_AppmImage(name_result,result);
           printf("\nOpération effectuée avec success\n");
        }
    }
    else{
        
        printf("Entrer le nom du fichier (moins de 50 caracteres):\t");
        scanf("%s",file);
        printf("\n Entrer un nom pour le nouveau fichier (moins de 50 caracteres):\t ");
        scanf("%s",n_result);
        printf("\nEntrer le coefficient de luminance:\t");
        scanf("%d",&lum);

        const char* name = n_result;
        const char* f = file;

        printf("\nip file : %s\n", f);

        if (openAppm(ppm, f)){
            result = LuminanceAppm(lum,ppm);
            new_AppmImage(name,result);
            printf("\nOpération effectuée avec success\n");
        }

    }

    free(ppm);
    
    
    return 0;
}