#include <stdio.h>
#include <pthread.h>
FILE *fp;
int main() {
    fp = fopen("A.txt","r");
    if (fp != NULL){
        char input[1024];
        while (!feof(fp)){
            fgets(input,1024,fp);
            puts(input);
        }
        fclose(fp);
    }else{
        printf("error opening the file");
    }
    return 0;
}
