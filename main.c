#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void display_table(double **x, int n, double *y) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            printf("%lf ",x[i][j]);
        }

        printf("\n");
    }
    printf("-------\n");
    for(int i = 0; i < n; i++) {
        printf("%lf ",y[i]);
    }
    printf("\n-------\n");
}

void pearson_cor(double **x, double *y, int n, double*r) {

    // i is the row iterator, j is the col iterator
    for(int i = 0; i < n; i++) {
        double sum_x = 0;
        double sum_y = 0;
        double sum_x2 = 0;
        double sum_y2 = 0;
        double sum_xy = 0;

        for(int j = 0; j < n; j++) {
            // create worker thread

            sum_x += x[j][i];
            sum_y += (y[j]);
            sum_x2 += (x[j][i] * x[j][i]);
            sum_y2 += (y[j] * y[j]);
            sum_xy += (x[j][i] * y[j]);

        }


        r[i] = ((n * sum_xy) - sum_x * sum_y) / sqrt((n * sum_x2 - (sum_x*sum_x)) * (n * sum_y2 - (sum_y*sum_y)));
    
    }

}


void main(){
    int n_array[] = {100,200,300,400,500,600,700,800,900,1000,2000,4000,8000,16000,20000};
    // int NumberOfInputs = 1000;
    int NumberOfInputs;

    for(int rt = 0; rt < 15; rt++) {
        double avg = 0;
        int iter = 0;

        NumberOfInputs = n_array[rt];
        printf("\n\nCurrently running on N = %d\n",NumberOfInputs);
        for(int gh = 0; gh < 3; gh++) {
            
            char* buffer;

            
            //printf("Input N: ");
            //scanf("%d",&NumberOfInputs);

            srand(time(NULL));

            double **xVec = (double**) malloc(sizeof(double*) * NumberOfInputs);
            double *yVec = (double*) malloc(sizeof(double*) * NumberOfInputs);
            double *rVec = (double*) malloc(sizeof(double*) * NumberOfInputs);

            for(int i = 0; i < NumberOfInputs; i++) {

                xVec[i] = malloc(sizeof(double) * NumberOfInputs);  
                double RandomNum = rand() % 100000 + 1; 
                for(int j = 0; j < NumberOfInputs; j++){
                    xVec[i][j] = RandomNum;
                }

                yVec[i] = rand() % 100000 + 1;
            }

            // display_table(xVec, NumberOfInputs, yVec);

            clock_t before = clock();
            pearson_cor(xVec, yVec, NumberOfInputs, rVec);
            clock_t after = clock();

            double runtime = 1000 * difftime(after,before)/CLOCKS_PER_SEC;
            printf("Total runtime: %lf ms\n", runtime);
            avg += runtime;
            iter += 1;
            printf("Current average of %lf at iteration %d\n", avg/iter, iter);


            free(yVec);
            free(rVec);
            for(int j = 0; j < NumberOfInputs; j++){
                free(xVec[j]);
            }
            free(xVec);


        }
    }
}


