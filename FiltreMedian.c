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
 
int* TriSelection(int* vec, int size){
    int temp;
    for (int i = 0; i<size; i++){
        for(int j = i+1; j < size; j++){
            if(vec[i] > vec[j]){
                temp = vec[i];
                vec[i] = vec[j];
                vec[j] = temp;
            }
        }
    }
    //printf(" *** %d %d %d\n",vec[0],vec[1], vec[8] );

    return vec;
} 

 A_PGMImage* FiltreMedian(A_PGMImage* img,int size ){

    float temp = 0.;
    A_PGMImage* result = malloc(sizeof(A_PGMImage)) ;
    result->data = calloc(img->height , sizeof(int*));
   
   
    for (int t = 0; t < img->height ; t++){
        result->data[t] = calloc(img->width,sizeof(int));
    } 
    strcpy(result->pgmType, img->pgmType);
    result->height = img->height;
    result->width = img->width;
    result->maxValue = img->maxValue;
    // initialisation de la plus grande valeur de l'image resultat
    
    int len_vec = size * size;
    int* v = malloc(len_vec * sizeof(int));
    int indice = 0;

    for (int  i = size/2; i < (img->height - size/2) ; i++)
    {
        for (int j = size/2; j < (img->width - size/2); j++)
        {
            for (int k = (i-size/2); k < (i + size/2 )+1; k++)
            {   
                for (int p = (j- size/2); p < (j + size/2)+1; p++)
                {
                   v[indice] = img->data[k][p];
                   indice+=1;
                }
                
            }
            indice = 0;

            // Tri selection
            v = TriSelection(v, len_vec);

            // valeur médianne
            result->data[i][j] = v[len_vec/2];

          
           
            
            
        }
        
    }
    return result;
    
}




 int main(int argc, char const* argv[])
{
    A_PGMImage* pgm = malloc(sizeof(A_PGMImage));
    
    
    A_PGMImage* result;
    const char* ipfile;
    const char* name_result;
    const char* const_size;
    
    

    //int s_min, s_max;

    char n_result[50];
    char file[50];

   
    int size = 3;

    

    if (argc == 4){
        
        ipfile = argv[1];
        name_result = argv[2];
        const_size = argv[3];
        
    
       
        //printf("ipfile: %s, name: %s, lumimance : %s, l : %d", ipfile, name_result, luminance, l);

        if (openA_PGM(pgm, ipfile)){
            printf("****FiltreMedian *****\n");
            result = FiltreMedian(pgm,atoi( const_size ));
            new_A_PGMimage(name_result,result);
           printf("\n*******Opération effectuée avec success*******\n");
        }
    }
    else{
        printf("****FiltreMedian *****\n");
        printf("Entrer le nom du fichier (moins de 50 caracteres):\t");
        scanf("%s",file);
        printf("\n Entrer un nom pour le nouveau fichier (moins de 50 caracteres):\t ");
        scanf("%s",n_result);
        printf("\n Entrer la taille du filtre median (Ex: 3 pour (3x3)):\t ");
        scanf("%d",&size);

        const char* name = n_result;
        const char* f = file;


        if (openA_PGM(pgm, f)){
            result = FiltreMedian(pgm,size);
            new_A_PGMimage(name,result);
            printf("\nOpération effectuée avec success\n");
        }

    }

    free(pgm);
    
    
    return 0;
}