#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

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

void pearson_cor(double **x, double *y, int n, double*r) {  // T(n) = n(5 + (n + 5) + 12)
                                                            // T(n) = n(n + 21)
                                                            // T(n) = n^2 + 21n
                                                            // O(n) = n^2

    // i is the row iterator, j is the col iterator
    for(int i = 0; i < n; i++) { // * O(n)
        double sum_x = 0;        // + O(1)
        double sum_y = 0;        // + O(1)
        double sum_x2 = 0;       // + O(1)
        double sum_y2 = 0;       // + O(1)
        double sum_xy = 0;       // + O(1)

        for(int j = 0; j < n; j++) {    // * O(n)        
            sum_x += x[j][i];               // + O(1)
            sum_y += (y[j]);                // + O(1)
            sum_x2 += (x[j][i] * x[j][i]);  // + O(1)
            sum_y2 += (y[j] * y[j]);        // + O(1)
            sum_xy += (x[j][i] * y[j]);     // + O(1)
        }

        r[i] = ((n * sum_xy) - sum_x * sum_y) / sqrt((n * sum_x2 - (sum_x*sum_x)) * (n * sum_y2 - (sum_y*sum_y)));
                                   
    }

}


void main(){
    int n_array[] = {100,200,300,400,500,600,700,800,900,1000,2000,4000,8000,16000,20000};
    // int NumberOfInputs = 1000;
    int NumberOfInputs;

    printf("[1]: Walkthrough Mode (100 to 20000)\n");
    printf("[2]: Custom N Mode\n");
    printf("[3]: Small data set integrity check (n=5)\n");
    printf("[0]: Exit\n");
    printf("Enter mode: ");
    int choice = 0;
    scanf("%d",&choice);

    printf("\n");


    // checking if formula is accurate
    if (choice == 3) {
        double avg = 0;
        int NumberOfInputs = 5;


        for(int gh = 0; gh < 3; gh++) {               
            srand(time(NULL));

            double **xVec = (double**) malloc(sizeof(double*) * NumberOfInputs);
            double *yVec = (double*) malloc(sizeof(double) * NumberOfInputs);
            double *rVec = (double*) malloc(sizeof(double) * NumberOfInputs);

/
            // Initializing the input matrix
            for(int i = 0; i < NumberOfInputs; i++) {

                xVec[i] = malloc(sizeof(double) * NumberOfInputs);  
                double RandomNum = rand() % 4 + 1; 
                for(int j = 0; j < NumberOfInputs; j++){
                    xVec[i][j] = RandomNum;
                }

                yVec[i] = rand() % 4 + 1;
            }

            display_table(xVec,5,yVec);

            // Function call, and timing

            struct timespec before = {0,0}, after = {0,0};
            clock_gettime(CLOCK_MONOTONIC, &before);
            pearson_cor(xVec, yVec, NumberOfInputs, rVec);
            clock_gettime(CLOCK_MONOTONIC, &after);

            // Runtime calculation
            double runtime = (((double) 1000*after.tv_sec + after.tv_nsec/1000000.0) - ((double) 1000*before.tv_sec + before.tv_nsec/1000000.0));
            printf("Runtime: %lf\n", runtime);

            printf("Pearson's r: ");
            for(int i = 0; i < 5; i++) {
                printf("%lf ",rVec[i]);
            }
            printf("\n\n\n");
            
            avg += runtime;

        }

        printf("\n");

        return;
    } else if (choice == 2) {
        double avg = 0;
        int iter = 0;
        int NumberOfInputs;

        printf("Enter n: ");
        scanf("%d",&NumberOfInputs);
        printf("\n\nCurrently running on N = %d\n",NumberOfInputs);

        for(int gh = 0; gh < 3; gh++) {               
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

            // Function call, and timing

            struct timespec before = {0,0}, after = {0,0};
            clock_gettime(CLOCK_MONOTONIC, &before);
            pearson_cor(xVec, yVec, NumberOfInputs, rVec);
            clock_gettime(CLOCK_MONOTONIC, &after);

            // Runtime calculation
            double runtime = (((double) 1000*after.tv_sec + after.tv_nsec/1000000.0) - ((double) 1000*before.tv_sec + before.tv_nsec/1000000.0));
            printf("%lf\t", runtime);
            
            avg += runtime;

            free(yVec);
            free(rVec);
            for(int j = 0; j < NumberOfInputs; j++){
                free(xVec[j]);
            }
            free(xVec);
        }

        printf("\n");

        return;
    } else if (choice == 1) {
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

                clock_gettime(CLOCK_MONOTONIC, &before);
                pearson_cor(xVec, yVec, NumberOfInputs, rVec);
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
}


