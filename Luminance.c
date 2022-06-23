#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct A_PGMImage {
    char pgmType[3];
    int ** data;
    int width;
    int height;
    int maxValue;
} A_PGMImage;

void ignoreComments(FILE* fp)
{
    int ch;
    char line[100];
 
    // Ignore any blank lines
    while ((ch = fgetc(fp)) != EOF
           && isspace(ch))
        ;
 
    // Recursively ignore comments
    // in a PGM image commented lines
    // start with a '#'
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        ignoreComments(fp);
    }
    else
        fseek(fp, -1, SEEK_CUR);
}
 

bool openA_PGM(A_PGMImage* pgm,const char* filename)
{
    // Open the image file in the
    // 'read binary' mode
    FILE* pgmfile = fopen(filename, "r");
 
    // If file does not exist,
    // then return
    if (pgmfile == NULL) {
        printf("File does not exist\n");
        return false;
    }
 
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%s",pgm->pgmType);
 
    // Check for correct PGM Binary
    // file type
    if (strcmp(pgm->pgmType, "P2")) {
        fprintf(stderr,"Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
 
    ignoreComments(pgmfile);
 
    // Read the image dimensions
    fscanf(pgmfile, "%d %d",&(pgm->width),&(pgm->height));
 
    ignoreComments(pgmfile);
 
    // Read maximum gray value
    fscanf(pgmfile, "%d", &(pgm->maxValue));
    ignoreComments(pgmfile);
 
    // Allocating memory to store
    // img info in defined struct
    pgm->data = malloc(pgm->height* sizeof(int*));
 
    // Storing the pixel info in
    // the struct
    if (pgm->pgmType[1] == '2') {
 
        //fgetc(pgmfile);
 
        for (int i = 0;i < pgm->height; i++) {
            
            pgm->data[i] = malloc(pgm->width* sizeof(int));

            // If memory allocation
            // is failed
            if (pgm->data[i] == NULL) {
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }

            for (int j = 0; j< pgm->width; j++){
                fscanf(pgmfile, "%d", &(pgm->data[i][j]));
            }

            // Read the gray values and
            // write on allocated memory
            //fread(pgm->data[i], sizeof(int), pgm->width, pgmfile);
        }
    }
 
    // Close the file

    fclose(pgmfile);
 
    return true;
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
 
 A_PGMImage* Luminance_Apgm(int valeur, A_PGMImage* img){
     //unsigned int** result;
     int max_val = 255;
     int min_val = 0;
     for (int i =0; i<img->height; i++){
        
         for (int j = 0; j < img->width; j++)
         {
            if(img->data[i][j] + valeur > max_val  ){
                img->data[i][j] = max_val;
            }
            else if((img->data[i][j] + valeur) < min_val){
                img->data[i][j] = min_val;
            }
            else{
                img->data[i][j] = img->data[i][j] + valeur;

            }         
         }


     }

    return img;
 }


 int main(int argc, char const* argv[])
{
    A_PGMImage* pgm = malloc(sizeof(A_PGMImage));
    
    
    A_PGMImage* result;
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
        if (openA_PGM(pgm, ipfile)){
            result = Luminance_Apgm(lum,pgm);
            new_A_PGMimage(name_result,result);
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

        if (openA_PGM(pgm, f)){
            result = Luminance_Apgm(lum,pgm);
            new_A_PGMimage(name,result);
            printf("\nOpération effectuée avec success\n");
        }

    }

    free(pgm);
    
    
    return 0;
}