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

void ignoreComments(FILE* fp);
 
bool openA_PGM(A_PGMImage* pgm,const char* filename);

int* pgm_histogramme(A_PGMImage* img){
    
    static int hist[256];

    for (int i = 0; i < img->height ; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            hist[img->data[i][j]] = hist[img->data[i][j]] + 1;
        }
           
    }
     
     return hist;
    
}


float Moyenne(int* hist, int T, int class){
    float temp=0;
    float result = 0;

    if(class == 0){
        for(int i = 0; i<T; i++){
            temp += hist[i];
        }
        result = temp/T;
    }
    else{
        for (int i = T; i < 256; i++){
            temp += hist[i];
        }
        result = temp / (256-T);
    }

    return result;
}


float Variance(int* hist, float mean, int T, int class ){
    float temp = 0;
    float result = 0;
    if(class == 0){
        for(int i=0; i<T; i++){
            temp += (hist[i] - mean)*(hist[i] - mean);
        }
        result = temp / T;
    }
    else{
        for(int i= T; i<256; i++){
            temp += (hist[i] - mean)*(hist[i] - mean);
        }
        result = temp / (256 - T);
    }

    return result;

}


float Proportion(int l,int L, int* hist, int T, int class){
    float temp = 0;
    float result = 0;

    if(class == 0){
        for(int i =0; i<T; i++){
            temp += hist[i];
        }
        result = temp / (l*L);
    }
    else{
        for(int i =T; i<256; i++){
            temp += hist[i];
        }
        result = temp / (l*L);
    }

    return result;
}


float Variance_intra(float p1, float v1, float p2, float v2){
    float result = (p1*v1 + p2*v2);
    return result;
}



int Otsu(A_PGMImage* img, int* seuils, int nb_seuil){
    float intra[nb_seuil];
    int* hist = pgm_histogramme(img);

    float m1,m2,v1,v2,p1,p2,v_intra;

    for (int i = 0; i < nb_seuil; i++){
        m1 = Moyenne(hist, seuils[i], 0);
        m2 = Moyenne(hist, seuils[i], 1);

        v1 = Variance(hist, m1, seuils[i], 0);
        v2 = Variance(hist, m2, seuils[i], 1);

        p1 = Proportion(img->width, img->height, hist, seuils[i], 0);
        p2 = Proportion(img->width, img->height, hist, seuils[i], 1);

        v_intra = Variance_intra(p1, v1, p2, v2);

        intra[i] = v_intra;

    }

    // tri des seuils candidats en fonction de leurs variances intra

    int temp =0;
    for(int i= 0; i < nb_seuil; i++){
        for(int j = i+1; j< nb_seuil; j++){
            if (intra[i]> intra[j])
            {
                temp = seuils[i];
                seuils[i] = seuils[j];
                seuils[j] = temp;
            }
            
        }
    }

    
    return seuils[0];


}




int main(int argc, char const* argv[])
{
    A_PGMImage* pgm = malloc(sizeof(A_PGMImage));
    A_PGMImage* result;
    const char* ipfile;

    char file[50];

    int nb_seuil = 254;


    int* sueils;

    sueils = malloc(254*sizeof(int));
    for (int i = 0 ; i < nb_seuil; i++){
        sueils[i] = i+1;
    }


    if (argc == 2){
        ipfile = argv[1];

        if (openA_PGM(pgm, ipfile)){
            printf("****Algorithme d'Otsu *****\n");
            printf("Meilleur seuil : %d", Otsu(pgm, sueils, nb_seuil));
            
           printf("\n*******Opération effectuée avec success*******\n");
        }
    }
    else{
        printf("****Algorithme d'Otsu *****\n");
        printf("Entrer le nom du fichier (moins de 50 caracteres):\t");
        scanf("%s",file);
    
        const char* f = file;


        if (openA_PGM(pgm, f)){
            printf("Meilleur seuil : %d", Otsu(pgm, sueils, nb_seuil));

            printf("\nOpération effectuée avec success\n");
        }

    }

    free(pgm);
    
    
    return 0;
}



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
