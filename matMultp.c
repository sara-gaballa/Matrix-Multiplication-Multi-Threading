#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
FILE *fp;//public pointer to the files
struct timeval stop, start;//struct for saving time
struct Array{
    int **arr;
    int row, col;
};//struct for saving 2D array and number of row and columns
struct Arguments{
    int row, col;
};//saving the arguments send to thread functions
struct Array A, B;//global assignment
char* outputFile;//public pointer to the output file name
struct Array parseArray(char name[]){//parsing the arrays from txt file
    struct Array temp;//temp array to be returned from the function
    fp = fopen(name,"r");
    int row, col;
    if (fp != NULL){
        char input[1024];
        fgets(input,1024,fp);
        sscanf(input, "row=%d col=%d", &row, &col);//extracting row and columns numbers from the file
        int **matrix = (int **)malloc(row * sizeof(int *));//allocating memory for the 2D array A or B
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
    }else{//print error if the file can not be accessed
        printf("Error opening the file");
        exit(1);
    }
}
void ReadInput(char  *input[]){//reading the input entered by the user
    char *Parsed_Input;
    char name[1024];
    if(input[1] != NULL) {//check if the input entered is not an empty line
        char name[1024];
        strcpy(name, input[1]);
        strcat(name, ".txt");
        A = parseArray(name);
        strcpy(name, input[2]);
        strcat(name, ".txt");
        B = parseArray(name);
        outputFile = input[3];
    }
    else{//the default case if the user do not enter names of the files
        A = parseArray("a.txt");
        B = parseArray("b.txt");
        outputFile = "c";
    }
}
void* ThreadPerMatrix(void * arg){//calculating the matrix multiplication thread by matrix
    char temp[1024];
    strcpy(temp, outputFile);
    strcat(temp,"_per_matrix.txt");//creating the output file of this method according to the public name of the output
    fp = fopen(temp,"a");
    fprintf(fp,"Method: A thread per matrix\n");
    fprintf(fp,"row=%d col=%d\n", A.row, B.col);
    int mult;
    for (int i = 0; i < A.row; ++i) {//doing the multiplication operation
        for (int j = 0; j < B.col; ++j) {
            mult = 0;
            for (int k = 0; k < A.col; ++k) {
                mult += A.arr[i][k] * B.arr[k][j];
            }
            fprintf(fp,"%d ",mult);//print each element in the file
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return NULL;
}
void* ThreadPerRow(void * i){//calculating the matrix multiplication thread by row
    int ROW = *(int*)i;//extracting the row number from the function argument
    char temp[1024];
    strcpy(temp, outputFile);
    strcat(temp,"_per_row.txt");//creating the output file of this method according to the public name of the output
    fp = fopen(temp,"a");
    if(ROW == 0) {
        fprintf(fp, "Method: A thread per row\n");
        fprintf(fp,"row=%d col=%d\n", A.row, B.col);
    }
    int mult ;
    for (int i = 0; i < B.col; ++i) {
        mult = 0;
        for (int j = 0; j < B.row; ++j) {
            mult += A.arr[ROW][j] * B.arr[j][i];
        }
        fprintf(fp,"%d ",mult);
    }
    fprintf(fp,"\n");
    fclose(fp);
    return NULL;
}
void *ThreadPerElement(void *args){//calculating the matrix multiplication thread by element
    struct Arguments *arg = (struct Arguments*)args;
    char temp[1024];
    strcpy(temp, outputFile);
    strcat(temp,"_per_element.txt");//creating the output file of this method according to the public name of the output
    fp = fopen(temp,"a");
    if(arg->row == 0 && arg->col ==0 ){
        fprintf(fp,"Method: A thread per element\n");
        fprintf(fp,"row=%d col=%d\n", A.row, B.col);
    }
    int mult = 0;
    for (int i = 0; i < A.col; ++i) {
        mult += A.arr[arg->row][i] * B.arr[i][arg->col];
    }
    fprintf(fp,"%d ",mult);
    if(arg->col == B.col -1)
        fprintf(fp,"\n");
    fclose(fp);
    return NULL;
}
void freeAllocation(){
    for (int i = 0; i < A.row; i++) {
        free(A.arr[i]);
    }
    free(A.arr);
    for (int i = 0; i < B.row; i++) {
        free(B.arr[i]);
    }
    free(B.arr);
}
int main(int argc, char *argv[]) {

    ReadInput(argv);//reading the input from the terminal
    pthread_t ByMatrix;
    gettimeofday(&start, NULL); //start checking time
    pthread_create(&ByMatrix, NULL, ThreadPerMatrix, NULL);
    pthread_join(ByMatrix, NULL);
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of threads: %d\n", 1);
    printf("Seconds taken from the ByMatrix method %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for the ByMatrix: %lu\n\n", stop.tv_usec - start.tv_usec);

    pthread_t ByRow[A.row];
    gettimeofday(&start, NULL); //start checking time
    for (int i = 0; i < A.row ; ++i) {
        pthread_create(&ByRow[i], NULL, ThreadPerRow, &i);
        pthread_join(ByRow[i], NULL);
    }
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of threads: %d\n", A.row);
    printf("Seconds taken from the ByRow method %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for the ByRow method: %lu\n\n", stop.tv_usec - start.tv_usec);

    pthread_t ByElement[A.row * B.col];
    struct Arguments args;
    gettimeofday(&start, NULL); //start checking time
    for (int i = 0; i < A.row ; ++i) {
        for (int j = 0; j < B.col; ++j) {
            args.row = i;
            args.col = j;
            pthread_create(&ByElement[i], NULL, ThreadPerElement, &args);
            pthread_join(ByRow[i], NULL);
        }
    }
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of threads: %d\n", A.row * B.col);
    printf("Seconds taken from the ByElement method %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for the ByElement method: %lu\n\n", stop.tv_usec - start.tv_usec);

    freeAllocation();
    
    return 0;
}
