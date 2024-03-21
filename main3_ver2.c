#define _GNU_SOURCE

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


// mutex as global variable
pthread_mutex_t *summation_mutex;


typedef struct arg{
	char **x;
    double *y;
    int n;
    double *r;
    int start;
    int end;

    double *sum_x;
    double *sum_y;
    double *sum_x2;
    double *sum_y2;
    double *sum_xy;

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

int set_affinity(int core_id) {
   int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
//    if (core_id < 0 || core_id >= num_cores)
//       return EINVAL;

   cpu_set_t cpuset;
   CPU_ZERO(&cpuset);
   CPU_SET(core_id, &cpuset);

   pthread_t current_thread = pthread_self();    
   return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}


void * bounded_pearson_cor(void *args) {
    arg* a = (arg *) args;


    char **x = a->x;
    double *y = a->y;
    int n = a->n;
    double *r = a->r;
    int start = a->start;
    int end = a->end;

    int num_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
    int core_id = ((end+1) / (end-start+1)) % (num_of_cores-1);
    // set_affinity(core_id);


    // printf("Thread created. Bounds: [%d, %d]\n", start, end);

    for(int i = 0; i < n; i++) { // * O(n)
    //  printf("\nIterating\n");

        double sum_x = 0;        // + O(1)
        double sum_y = 0;        // + O(1)
        double sum_x2 = 0;       // + O(1)
        double sum_y2 = 0;       // + O(1)
        double sum_xy = 0;       // + O(1)

        for(int j = start; j < end; j++) {    // * O(n)
            // create worker thread       
            sum_x += x[j][i];               // + O(1)
            sum_y += (y[j]);                // + O(1)
            sum_x2 += (x[j][i] * x[j][i]);  // + O(1)
            sum_y2 += (y[j] * y[j]);        // + O(1)
            sum_xy += (x[j][i] * y[j]);     // + O(1)
        }

        // CRITICAL SECTION
        //printf("Trying to access: %p\n", &summation_mutex[i]);
        pthread_mutex_lock(&summation_mutex[i]);
        //printf("\nLocked: %p\n", &summation_mutex[i]);

        a->sum_x[i] += sum_x;
        a->sum_y[i] += sum_y;
        a->sum_x2[i] += sum_x2;
        a->sum_y2[i] += sum_y2;
        a->sum_xy[i] += sum_xy;

        pthread_mutex_unlock(&summation_mutex[i]);
        //printf("\nUnlocked: %p\n", &summation_mutex[i]);
        // END OF CRITICAL SECTION
                 
                                   
    }


    pthread_exit(NULL);

}

void pearson_cor(char **x, double *y, int n, double*r, int numberOfThreads, arg* argHolder, pthread_t* tid) {

    for(int i = 0; i < numberOfThreads; i++) {
        pthread_create(&tid[i], NULL, bounded_pearson_cor, (void *) &argHolder[i]);
    }


	for(int i=0; i < numberOfThreads; i++){
		pthread_join(tid[i], NULL);
	}

    for(int i=0; i < n; i++){
        // The formula for Pearson's r in code form
		r[i] = ((n * argHolder->sum_xy[i]) - argHolder->sum_x[i] * argHolder->sum_y[i]) / sqrt((n * argHolder->sum_x2[i] - (argHolder->sum_x[i]*argHolder->sum_x[i])) * (n * argHolder->sum_y2[i] - (argHolder->sum_y[i]*argHolder->sum_y[i])));
        
    }  


}


int main(){
    int n_array[] = {25000};
    int threadArray[] = {1,2,4,8,16,32,64,128,256};
    int NumberOfInputs;


    printf("\n");

    // Division by row

    for(int rt = 0; rt < 1; rt++) {
        double avg = 0;
        int iter = 0;

        NumberOfInputs = n_array[rt];
        printf("\n\nCurrently running on N = %d\n",NumberOfInputs);

        for(int ti = 0; ti < 9; ti++) {
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

            double *sum_x =  (double*) calloc(NumberOfInputs, sizeof(double));
            double *sum_y =  (double*) calloc(NumberOfInputs, sizeof(double));
            double *sum_x2 = (double*) calloc(NumberOfInputs, sizeof(double));
            double *sum_y2 = (double*) calloc(NumberOfInputs, sizeof(double));
            double *sum_xy = (double*) calloc(NumberOfInputs, sizeof(double));

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

                argHolder[i].sum_x = sum_x;
                argHolder[i].sum_y = sum_y;
                argHolder[i].sum_x2 = sum_x2;
                argHolder[i].sum_y2 = sum_y2;
                argHolder[i].sum_xy = sum_xy;
            }

            // mutex init

            summation_mutex = malloc(sizeof(pthread_mutex_t) * NumberOfInputs);
            for (int m = 0; m < NumberOfInputs; m++) {
                pthread_mutex_init(&summation_mutex[m], NULL);
            }

        


            struct timespec before = {0,0}, after = {0,0};

            clock_gettime(CLOCK_MONOTONIC, &before);
            pearson_cor(xVec, yVec, NumberOfInputs, rVec, NUMBER_OF_THREADS, argHolder, tid);
            clock_gettime(CLOCK_MONOTONIC, &after);

            double runtime = (((double) 1000*after.tv_sec + after.tv_nsec/1000000.0) - ((double) 1000*before.tv_sec + before.tv_nsec/1000000.0));
            printf("%lf\t", runtime);
            
            avg += runtime;
            iter += 1;


            // mutex destroy
            for (int m = 0; m < NUMBER_OF_THREADS; m++) {
                pthread_mutex_destroy(&summation_mutex[m]);
            }

            

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


