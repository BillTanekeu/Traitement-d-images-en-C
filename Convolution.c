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
    fprintf(fic, "# Tanekeu Bill Jordan\n");
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
 
 
 A_PGMImage* Convolution(A_PGMImage* img, float** kernel,int size,  int seuil ){
    float temp = 0.;
    A_PGMImage* result = malloc(sizeof(A_PGMImage)) ;
    result->data = calloc(img->height , sizeof(int*));
   
   
    for (int t = 0; t < img->height ; t++){
        result->data[t] = calloc(img->width,sizeof(int));
    } 
    strcpy(result->pgmType, img->pgmType);
    result->height = img->height;
    result->width = img->width;
    // initialisation de la plus grande valeur de l'image resultat
    int maxvalue = 255;
    
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
                    temp += img->data[k][p] * kernel[a][b];
                    b++;
                }
                a++;
                
            }
            // Activation RELU
            if (temp < seuil)
            {
                temp= 0;
            }
            
            // convertir la pré-activation en entier 
            result->data[i][j] = (int) temp;

            temp = 0.;
            // Mettre à jour la valeur max
            if (result->data[i][j] > maxvalue)
            {
                maxvalue = result->data[i][j];
            }
            
            
        }
        
    }
    result->maxValue = maxvalue;
    return result;
    
}




 int main(int argc, char const* argv[])
{
    A_PGMImage* pgm = malloc(sizeof(A_PGMImage));
    
    
    A_PGMImage* result;
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

    int p =-1;
    float** mat = malloc(size*sizeof(float*));
    for (int i =0; i<size; i++){
        mat[i] = malloc(size*sizeof(float));
        for (int j=0; j<size; j++){
            mat[i][j] = p;
        }
        p+=1;
    }

    if (argc == 3){
        
        ipfile = argv[1];
        name_result = argv[2];
        
    
       
        //printf("ipfile: %s, name: %s, lumimance : %s, l : %d", ipfile, name_result, luminance, l);

        if (openA_PGM(pgm, ipfile)){
            printf("****Convolution (3,3)*****\n");
            result = Convolution(pgm,mat, size, seuil );
            new_A_PGMimage(name_result,result);
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


        if (openA_PGM(pgm, f)){
            result = Convolution(pgm, mat,size, seuil);
            new_A_PGMimage(name,result);
            printf("\nOpération effectuée avec success\n");
        }

    }

    free(pgm);
    
    
    return 0;
}