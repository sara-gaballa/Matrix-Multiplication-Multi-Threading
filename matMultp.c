#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
FILE *fp;
struct Array{
    int **arr;
    int row, col;
};
struct Array A, B, C;
struct Array parseArray(char name[]){
    struct Array temp;
    fp = fopen(name,"r");
    int row, col;
    if (fp != NULL){
        char input[1024];
        fgets(input,1024,fp);
        sscanf(input, "row=%d col=%d", &row, &col);
        int **matrix = (int **)malloc(row * sizeof(int *));
        for (int i = 0; i < row; i++) {
            matrix[i] = (int *)malloc(col * sizeof(int));
        }
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                fscanf(fp, "%d", &matrix[i][j]);
            }
        }
        temp.arr = matrix;
        temp.row = row;
        temp.col = col;
        fclose(fp);
        return temp;
    }else{
        printf("Error opening the file");
        exit(1);
    }
}
void ReadFile(char input[]){
    char *Parsed_Input;
    char name[1024];
    Parsed_Input = strtok(input, " ");//A
    if(Parsed_Input != NULL){
        strcpy(name, Parsed_Input);
        strcat(name,".txt");
        A = parseArray(name);
        Parsed_Input = strtok(NULL, " ");//B
        if(Parsed_Input != NULL){
            strcpy(name, Parsed_Input);
            strcat(name,".txt");
            B = parseArray(name);
        }
        Parsed_Input = strtok(NULL, "\n");//C
        if(Parsed_Input != NULL){
            strcpy(name, Parsed_Input);
            strcat(name,".txt");
            fp = fopen(name,"w");
            fclose(fp);
            C.row = A.row;
            C.col = B.col;
        }
    }
    else{
        A = parseArray("a.txt");
        B = parseArray("b.txt");
        fp = fopen("c.txt","w");
        fclose(fp);
        C.row = A.row;
        C.col = B.col;
    }
}
void* A_thread_per_matrix(void * arg){
    fp = fopen("C.txt","a");
    int mult;
    for (int i = 0; i < A.row; ++i) {
        for (int j = 0; j < B.col; ++j) {
            mult = 0;
            for (int k = 0; k < A.col; ++k) {
                mult += A.arr[i][k] * B.arr[k][j];
            }
            fprintf(fp,"%d ",mult);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("done");
    return NULL;
}
int main() {
    char input[1024];
    fgets(input, sizeof(input), stdin);
    ReadFile(input);
//    for (int i = 0; i <  A.row; i++) {
//        for (int j = 0; j < A.col; j++) {
//            printf("%d ", A.arr[i][j]);
//        }
//        printf("\n");
//    }
    pthread_t first;
    pthread_create(&first, NULL, A_thread_per_matrix, NULL);
    pthread_join(first, NULL);
    for (int i = 0; i < A.row; i++) {
        free(A.arr[i]);
    }
    free(A.arr);
    for (int i = 0; i < B.row; i++) {
        free(B.arr[i]);
    }
    free(B.arr);
    return 0;
}
