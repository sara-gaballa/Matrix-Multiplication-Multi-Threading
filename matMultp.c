#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
FILE *fp;//public pointer to the files
struct timeval stop, start;//struct for saving time
struct Array{//global struct for saving arrays data
    int **arr;
    int row, col;
};//struct for saving 2D array and number of row and columns
struct Arguments{//saving the arguments send to thread functions
    int row, col;
};
struct Array A, B, C;//global assignment
char* outputFile;//public pointer to the output file name
void parseArray(char name[], struct Array *x) {//function to parse the input taken from files and save it in arrays
    fp = fopen(name, "r");//reading from a file
    if (fp != NULL) {
        char input[1024];
        fgets(input, 1024, fp);
        sscanf(input, "row=%d col=%d", &x->row, &x->col);
        x->arr = (int **) malloc(x->row * sizeof(int *));
        for (int i = 0; i < x->row; i++) {
            x->arr[i] = (int *) malloc(x->col * sizeof(int));
        }
        for (int i = 0; i < x->row; i++) {
            for (int j = 0; j < x->col; j++) {
                fscanf(fp, "%d", &x->arr[i][j]);//inserting input to the array
            }
        }
        fclose(fp);
    } else {
        printf("Error opening the file");
        exit(1);
    }
}
void ReadInput(char  *input[]){//reading the input entered by the user
    char name[1024];
    if(input[1] != NULL) {//check if the input entered is not an empty line
        char name[1024];
        strcpy(name, input[1]);
        strcat(name, ".txt");
        parseArray(name, &A);//parsing data to A
        strcpy(name, input[2]);
        strcat(name, ".txt");
        parseArray(name, &B);//parsing data to B
        if(A.col != B.row){
            printf("Error matrices cant be multiplied");
            exit(0);
        }
        outputFile = input[3];//name of the output file
    }
    else{//the default case if the user do not enter names of the files
        parseArray("a.txt", &A);//parsing data to A
        parseArray("b.txt", &B);//parsing data to B
        outputFile = "c";
    }
}
void* ThreadPerMatrix(void * arg){//calculating the matrix multiplication thread by matrix
    int mult;
    for (int i = 0; i < A.row; ++i) {//doing the multiplication operation
        for (int j = 0; j < B.col; ++j) {
            mult = 0;
            for (int k = 0; k < A.col; ++k) {
                mult += A.arr[i][k] * B.arr[k][j];
            }
           C.arr[i][j] = mult;//saving the result in result array
        }
    }
    return NULL;
}
void* ThreadPerRow(void * arg){//calculating the matrix multiplication thread by row
    int ROW = (int)arg;//extracting the row number from the function argument
    int mult ;
    for (int i = 0; i < B.col; ++i) {
        mult = 0;
        for (int j = 0; j < B.row; ++j) {
            mult += A.arr[ROW][j] * B.arr[j][i];
        }
        C.arr[ROW][i] = mult;//saving the result in result array
    }
    return NULL;
}
void *ThreadPerElement(void *args){//calculating the matrix multiplication thread by element
    struct Arguments *arg = (struct Arguments*)args;
    int mult = 0;
    for (int i = 0; i < A.col; ++i) {
        mult += A.arr[arg->row][i] * B.arr[i][arg->col];
    }
    C.arr[arg->row][arg->col] = mult;//saving the result in result array
    free(arg);//freeing the allocation of the struct
    return NULL;
}
void freeAllocation(){//function frees the memory allocated for the input matrices.
    for (int i = 0; i < A.row; i++) {
        free(A.arr[i]);
    }
    free(A.arr);
    for (int i = 0; i < B.row; i++) {
        free(B.arr[i]);
    }
    free(B.arr);
    for (int i = 0; i < C.row; i++) {
        free(C.arr[i]);
    }
    free(C.arr);
}
void writeFile(char name[],char FunctionName[]){//function to write output to file
    fp = fopen(name,"w");
    fprintf(fp,FunctionName);//printing function name
    fprintf(fp,"row=%d col=%d\n", A.row, B.col);//printing rows and columns
    for (int i = 0; i < C.row; ++i) {
        for (int j = 0; j < C.col; ++j) {
            fprintf(fp,"%d ",C.arr[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}
void clearRes(){//clearing the result array for the nest method
    for (int i = 0; i < C.row; ++i) {
        for (int j = 0; j < C.col; ++j) {
           C.arr[i][j] = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    ReadInput(argv);//reading the input from the terminal

    C.row = A.row;
    C.col = B.col;
    C.arr = (int **) malloc(C.row * sizeof(int *));
    for (int i = 0; i < C.row; i++) {//allocating memory for the result array
        C.arr[i] = (int *) malloc(C.col * sizeof(int));
    }

    gettimeofday(&start, NULL); //start checking time
    pthread_t ByMatrix;//A thread for the matrix
    pthread_create(&ByMatrix, NULL, ThreadPerMatrix, NULL);//creation of the thread
    pthread_join(ByMatrix, NULL);//waiting for the thread to end
    gettimeofday(&stop, NULL); //end checking time
    char temp[1024];
    strcpy(temp, outputFile);
    strcat(temp,"_per_matrix.txt");//creating the output file of this method according to the public name of the output
    writeFile(temp,"Method: A thread per matrix\n");
    clearRes();//clearing the result matrix
    printf("Number of threads for the first method: %d\n", 1);//printing number of threads taken by the first method
    printf("Seconds taken from the ByMatrix method %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for the ByMatrix: %lu\n\n", stop.tv_usec - start.tv_usec);//time taken to compute the thread

    gettimeofday(&start, NULL); //start checking time
    pthread_t ByRow[A.row];//creating array of threads for the second method
    for (int i = 0; i < A.row ; ++i) {
        pthread_create(&ByRow[i], NULL, ThreadPerRow, (void*)i);//creation of the thread
    }

    for (int i = 0; i < A.row ; ++i) {
        pthread_join(ByRow[i], NULL);//waiting for the thread to end
    }
    gettimeofday(&stop, NULL); //end checking time
    strcpy(temp, outputFile);
    strcat(temp,"_per_row.txt");//creating the output file of this method according to the public name of the output
    writeFile(temp,"Method: A thread per row\n");
    clearRes();//clearing the result matrix
    printf("Number of threads for the second method: %d\n", A.row);//printing number of threads taken by the second method
    printf("Seconds taken from the ByRow method %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for the ByRow method: %lu\n\n", stop.tv_usec - start.tv_usec);//time taken to compute all the threads

    gettimeofday(&start, NULL); //start checking time
    pthread_t ByElement[A.row ][B.col];//2D Array of threads for the third method
    for (int i = 0; i < A.row ; ++i) {
        for (int j = 0; j < B.col; ++j) {
            struct Arguments *args = malloc(sizeof(struct Arguments));
            args->row = i;
            args->col = j;
            pthread_create(&ByElement[i][j], NULL, ThreadPerElement, (void *)args);//creation of the thread
        }
    }
    for(int i = 0; i < A.row; i++) {// Wait for all threads to complete
        for(int j = 0; j < B.col; j++) {
            pthread_join(ByElement[i][j], NULL);//waiting for the thread to end
        }
    }
    gettimeofday(&stop, NULL); //end checking time
    strcpy(temp, outputFile);
    strcat(temp,"_per_element.txt");//creating the output file of this method according to the public name of the output
    writeFile(temp,"Method: A thread per element\n");
    printf("Number of threads for the third method: %d\n", A.row * B.col);//printing number of threads taken by the third method
    printf("Seconds taken from the ByElement method %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken for the ByElement method: %lu\n\n", stop.tv_usec - start.tv_usec);//time taken to compute all the threads

    freeAllocation();//freeing the allocation of the arrays
    return 0;
}