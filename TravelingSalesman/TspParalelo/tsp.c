/* WSCAD - 9th Marathon of Parallel Programming 
 * Simple Brute Force Algorithm for the 
 * Traveling-Salesman Problem
 * Author: Emilio Francesquini - francesquini@ic.unicamp.br
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <time.h>


/***********************
 * Função auxiliar para medir o tempo de execução
 ***********************/
double timestamp(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    return((double)(tp.tv_sec + tp.tv_nsec*1.0e-9));
}

//tempo das partes puramente sequenciais, paralelas e totais
double ini_seq_time, final_seq_time, delta_seq_time = 0.0;
double ini_par_time, final_par_time, delta_par_time = 0.0;
double ini_tot_time, final_tot_time, delta_tot_time = 0.0;

    
int min_distance;
int nb_towns;
int iter = 0;

typedef struct {
    int to_town;
    int dist;
} d_info;

d_info **d_matrix;
int *dist_to_origin;

int present (int town, int depth, int *path) {
    int i;
    for (i = 0; i < depth; i++)
        if (path[i] == town) return 1;
    return 0;
}

void tsp (int depth, int current_length, int *path) {
    if (current_length >= min_distance) return;
    if (depth == nb_towns)
    {
        {
        current_length += dist_to_origin[path[nb_towns - 1]];
        #pragma omp critical
        if (current_length < min_distance)
            min_distance = current_length;
        }
    } 

    else 
    {
        int town, me, dist;
        // printf("a thread é %d\n", omp_get_thread_num());
        
        if(depth < 2 )
        {
            #pragma omp parallel for num_threads(1) firstprivate (depth, current_length) private(dist, town, me)
            for (int i = 0; i < nb_towns; i++)
            {
                int *path_aux = (int*) malloc(sizeof(int) * nb_towns);
                memcpy(path_aux, path, sizeof(int)*nb_towns);
                {
                me = path_aux[depth - 1];

                town = d_matrix[me][i].to_town;
                if (!present (town, depth, path_aux)) {
                    path_aux[depth] = town;
                    dist = d_matrix[me][i].dist;
                    tsp (depth + 1, current_length + dist, path_aux);
                    
                }   
                }
          
                    free(path_aux);
            }   
            iter++;
        }
        else
        {
            for (int i = 0; i < nb_towns; i++) 
            {
                me = path[depth - 1];
                town = d_matrix[me][i].to_town;
                if (!present (town, depth, path)) {
                    path[depth] = town;
                    dist = d_matrix[me][i].dist;
                    tsp (depth + 1, current_length + dist, path);
                }            
            }
        }

        
    }
}

void greedy_shortest_first_heuristic(int *x, int *y) {
    int i, j, k, dist;
    int *tempdist;

    tempdist = (int*) malloc(sizeof(int) * nb_towns);
    //Could be faster, albeit not as didactic.
    //Anyway, for tractable sizes of the problem it
    //runs almost instantaneously.
    for (i = 0; i < nb_towns; i++) {
        for (j = 0; j < nb_towns; j++) {
            int dx = x[i] - x[j];
            int dy = y[i] - y[j];
            tempdist [j] = dx * dx + dy * dy;
        }
        for (j = 0; j < nb_towns; j++) {
            int tmp = INT_MAX;
            int town = 0;
            for (k = 0; k < nb_towns; k++) {
                if (tempdist [k] < tmp) {
                    tmp = tempdist [k];
                    town = k;
                }
            }
            tempdist [town] = INT_MAX;
            d_matrix[i][j].to_town = town;
            dist = (int) sqrt (tmp);
            d_matrix[i][j].dist = dist;
            if (i == 0)
                dist_to_origin[town] = dist;
        }
    }

    free(tempdist);
}

void init_tsp() {
    int i, st;
    int *x, *y;

    min_distance = INT_MAX;

    st = scanf("%u", &nb_towns);
    if (st != 1) exit(1);
 
    //alocate a matrix of information * number of towns
    d_matrix = (d_info**) malloc (sizeof(d_info*) * nb_towns);
    //for each number of towns, alocate the line of informations
    for (i = 0; i < nb_towns; i++)
        d_matrix[i] = (d_info*) malloc (sizeof(d_info) * nb_towns);
    //distance to origin is an array of ints with the size of number of towns
    dist_to_origin = (int*) malloc(sizeof(int) * nb_towns);
   
    //x and y are arrays of ints with the size of number of town, they represent the coordenates of each town

    x = (int*) malloc(sizeof(int) * nb_towns);
    y = (int*) malloc(sizeof(int) * nb_towns);
    

    //st = 2 is the number os arguments read by scanf is 2
    //assign x and y arrays with the inputs read 
    for (i = 0; i < nb_towns; i++) {
        st = scanf("%u %u", x + i, y + i);
        if (st != 2) exit(1);
    }

    
    greedy_shortest_first_heuristic(x, y);

    free(x);
    free(y);
}

int run_tsp() {
    int i, *path;

    init_tsp();


    path = (int*) malloc(sizeof(int) * nb_towns);
    path[0] = 0;
 
    final_seq_time = timestamp();
    delta_seq_time = final_seq_time - ini_seq_time;
    ini_par_time = timestamp();
    tsp (1, 0, path);
    final_par_time = timestamp();
    ini_seq_time = timestamp();
    free(path);
    for (i = 0; i < nb_towns; i++)
        free(d_matrix[i]);
    free(d_matrix);

    return min_distance;
}

int main (int argc, char **argv) {
    ini_tot_time = timestamp();
    ini_seq_time = ini_tot_time;
    int num_instances, st;
    st = scanf("%u", &num_instances);
    if (st != 1) exit(1);
    while(num_instances-- > 0)
        printf("%d ", run_tsp());
    final_seq_time = timestamp();
    final_tot_time = timestamp();
    delta_seq_time = delta_seq_time + (final_seq_time-ini_seq_time);
    delta_tot_time = final_tot_time - ini_tot_time;
    delta_par_time = final_par_time - ini_par_time;

    printf("%15f %15f %15f\n", delta_seq_time, delta_par_time, delta_tot_time);

    return 0;
}
