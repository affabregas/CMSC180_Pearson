#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <syscall.h>


/*
To-dos
1. spawn threads mechanic
2. thread args holder
3. thread joining
4. multiple run for multiple no. of threads in main, modification
5. check if computation of pearsons r is still correct


*/

// This will be passed as parameters
typedef struct arg{
	char **x;
    double *y;
    int n;
    double *r;
    int start;
    int end;
}arg;


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


void * bounded_pearson_cor(void *args) {
    arg* a = (arg *) args;


    char **x = a->x;
    double *y = a->y;
    int n = a->n;
    double *r = a->r;
    int start = a->start;
    int end = a->end;

    // printf("Thread created. Bounds: [%d, %d]\n", start, end);

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


    pthread_exit(NULL);

}

void pearson_cor(char **x, double *y, int n, double*r, int numberOfThreads, arg* argHolder, pthread_t* tid) {
      

    // Thread creation
    for(int i = 0; i < numberOfThreads; i++) {
        pthread_create(&tid[i], NULL, bounded_pearson_cor, (void *) &argHolder[i]);
    }

    // Thread joining
	for(int i=0; i < numberOfThreads; i++){
		pthread_join(tid[i], NULL);
	}


}


int main(){
    int n_array[] = {25000, 30000, 40000, 50000,100000};
    int threadArray[] = {1,2,4,8,16,32,64};
    int NumberOfInputs;


    printf("\n");

    for(int rt = 0; rt < 5; rt++) {
        double avg = 0;
        int iter = 0;

        NumberOfInputs = n_array[rt];
        printf("\n\nCurrently running on N = %d\n",NumberOfInputs);

        for(int ti = 0; ti < 7; ti++) {
            int NUMBER_OF_THREADS = threadArray[ti];
            printf("\n  -- With %d thread/s --\n", NUMBER_OF_THREADS);

            for(int gh = 0; gh < 3; gh++) {
            if (NumberOfInputs < NUMBER_OF_THREADS) {
                printf("Number of inputs cannot be less than number of threads.\n");
                continue;
            }            

            srand(time(NULL));

            char **xVec = (char**) malloc(sizeof(char*) * NumberOfInputs);
            double *yVec = (double*) malloc(sizeof(double) * NumberOfInputs);
            double *rVec = (double*) malloc(sizeof(double) * NumberOfInputs);

            // Initializing the input matrix
            for(int i = 0; i < NumberOfInputs; i++) {

                xVec[i] = malloc(sizeof(char) * NumberOfInputs);  
                char RandomNum = rand() % 100 + 1; 
                for(int j = 0; j < NumberOfInputs; j++){
                    xVec[i][j] = RandomNum;
                }

                yVec[i] = rand() % 20 + 1;
            }


            // Filling in the arguments for the threads
            int offset = NumberOfInputs / NUMBER_OF_THREADS;
            arg* argHolder = (arg*) malloc(sizeof(arg) * NUMBER_OF_THREADS);
            pthread_t *tid = (pthread_t*) malloc(sizeof(pthread_t) * NUMBER_OF_THREADS);

            for(int i = 0; i < NUMBER_OF_THREADS; i++) {
                int start = offset*i;
                int end = offset*(i+1)-1;

                if (offset*(i+1) >= NumberOfInputs) 
                    end = NumberOfInputs-1;

                argHolder[i].x = xVec;
                argHolder[i].y = yVec;
                argHolder[i].n = NumberOfInputs;
                argHolder[i].r = rVec;
                argHolder[i].start = start;
                argHolder[i].end = end;
            }


            struct timespec before = {0,0}, after = {0,0};

            clock_gettime(CLOCK_MONOTONIC, &before);
            pearson_cor(xVec, yVec, NumberOfInputs, rVec, NUMBER_OF_THREADS, argHolder, tid);
            clock_gettime(CLOCK_MONOTONIC, &after);

            double runtime = (((double) 1000*after.tv_sec + after.tv_nsec/1000000.0) - ((double) 1000*before.tv_sec + before.tv_nsec/1000000.0));
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
            free(argHolder);
            free(tid);

            


        }

        }
        printf("\n");

    }

    return 0;
}


