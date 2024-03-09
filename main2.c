#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>


/*
To-dos
1. spawn threads mechanic
2. thread args holder
3. thread joining
4. multiple run for multiple no. of threads in main, modification
5. 


*/

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


void bounded_pearson_cor(double **x, double *y, int n, double *r, int start, int end) {

    for(int i = start; i < end; i++) { // * O(n)

        double sum_x = 0;        // + O(1)
        double sum_y = 0;        // + O(1)
        double sum_x2 = 0;       // + O(1)
        double sum_y2 = 0;       // + O(1)
        double sum_xy = 0;       // + O(1)

        for(int j = 0; j < n; j++) {    // * O(n)
            // create worker thread       
            sum_x += x[j][i];               // + O(1)
            sum_y += (y[j]);                // + O(1)
            sum_x2 += (x[j][i] * x[j][i]);  // + O(1)
            sum_y2 += (y[j] * y[j]);        // + O(1)
            sum_xy += (x[j][i] * y[j]);     // + O(1)

            sum_x++;
        }

        r[i] = ((n * sum_xy) - sum_x * sum_y) / sqrt((n * sum_x2 - (sum_x*sum_x)) * (n * sum_y2 - (sum_y*sum_y)));
                                   
    }

}

void pearson_cor(double **x, double *y, int n, double*r, int numberOfThreads) {

    /*
           j  j+1
    i      1   3   5   3   2   5
    i+1    1   8   7   8   3   5
           2   5   4   9   5   3
           3   5   9   6   9   6
           7   4   9   2   3   7
           3   5   9   3   5   9
    
    */

    // Create thread that runs the function bounded_pearson_cor()
    // IF numberOfThreads = 4, n = 17 then:
    // offset = 17/4 = 4,     c = 0
    // 0 1 2 3     4 5 6 7    8 9 10 11    12 13 14 15    16
    // [offset*i, (offset*(i+1) - 1)]    c+offset

    int offset = n / numberOfThreads;
    for(int i = 0; i < n; i++) {
        int start = offset*i;
        int end = offset*(i+1)-1;
        if (offset*(i+1) >= n) 
            end = n-1;

        // MAKE THREAD RUN HERE 
        bounded_pearson_cor(x, y, n, r, start, end);
    }

    // WAIT FOR JOIN


    // i is the row iterator, j is the col iterator
    /*for(int i = 0; i < n; i++) { // * O(n)
        double sum_x = 0;        // + O(1)
        double sum_y = 0;        // + O(1)
        double sum_x2 = 0;       // + O(1)
        double sum_y2 = 0;       // + O(1)
        double sum_xy = 0;       // + O(1)

        for(int j = 0; j < n; j++) {    // * O(n)
            // create worker thread       
            sum_x += x[j][i];               // + O(1)
            sum_y += (y[j]);                // + O(1)
            sum_x2 += (x[j][i] * x[j][i]);  // + O(1)
            sum_y2 += (y[j] * y[j]);        // + O(1)
            sum_xy += (x[j][i] * y[j]);     // + O(1)

            sum_x++;
        }

        r[i] = ((n * sum_xy) - sum_x * sum_y) / sqrt((n * sum_x2 - (sum_x*sum_x)) * (n * sum_y2 - (sum_y*sum_y)));
                                   
    }
    */

}


void main(){
    int n_array[] = {100,200,300,400,500,600,700,800,900,1000,2000,4000,8000,16000,20000};
    int NumberOfInputs;
    int NUMBER_OF_THREADS = 1;

    printf("\n");

    // Division by row

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
                double RandomNum = rand() % 100 + 1; 
                for(int j = 0; j < NumberOfInputs; j++){
                    xVec[i][j] = RandomNum;
                }

                yVec[i] = rand() % 20 + 1;
            }

            // display_table(xVec, NumberOfInputs, yVec);

            struct timespec before = {0,0}, after = {0,0};

            // create thread here

            clock_gettime(CLOCK_MONOTONIC, &before);
            pearson_cor(xVec, yVec, NumberOfInputs, rVec, NUMBER_OF_THREADS);
            clock_gettime(CLOCK_MONOTONIC, &after);

            double runtime = (((double) 1000*after.tv_sec + after.tv_nsec/1000000.0) - ((double) 1000*before.tv_sec + before.tv_nsec/1000000.0));
            // printf("Total runtime: %lf ms ", runtime);
            printf("%lf\t", runtime);
            
            avg += runtime;
            iter += 1;
            // printf("Current average of %lf at iteration %d\n", avg/iter, iter);


            free(yVec);
            free(rVec);
            for(int j = 0; j < NumberOfInputs; j++){
                free(xVec[j]);
            }
            free(xVec);

            


        }

        printf("\n");

    }
}


