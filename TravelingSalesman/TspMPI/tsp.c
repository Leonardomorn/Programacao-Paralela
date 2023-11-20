/* WSCAD - 9th Marathon of Parallel Programming 
 * Simple Brute Force Algorithm for the 
 * Traveling-Salesman Problem
 * Author: Emilio Francesquini - francesquini@ic.unicamp.br
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include<unistd.h>


int min_distance;
int nb_towns;
double ini_time, final_time, delta_time;

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

double timestamp(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    return((double)(tp.tv_sec + tp.tv_nsec*1.0e-9));
}

void tsp (int depth, int current_length, int *path, unsigned int reductcounter, int my_rank, int n_procs,int first_leaf ) {
    int i; int local_min_distance;
    reductcounter++;
    if (current_length >= min_distance) return; 
    if (depth == nb_towns) 
    {
        current_length += dist_to_origin[path[nb_towns - 1]];
        //print debug
        /**
         * 
        if(my_rank == 0)
        {
            printf(" processo %d percorreu o caminho: ", my_rank);
            for (int a = 0; a < nb_towns; a++)
            {
                printf("%d ", path[a]);
            }
            printf("\n");       
        } 
        */

        if (current_length < min_distance)
        {
            min_distance = current_length;
            if ( reductcounter > 80000)
            {
                MPI_Allreduce(&min_distance, &local_min_distance, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
                min_distance = local_min_distance; 
            }
                    
        }            
            
    }
    else if (depth == 1)
    {
        int town, me, dist;
        me = path[depth - 1];
        for (i = my_rank; i < nb_towns; i = i + n_procs) 
        {
            town = d_matrix[me][i].to_town;
            if (!present (town, depth, path)) {
                path[depth] = town;
                //print debug
                // printf("processo %d : nova cidade escolhida: path = ", my_rank);
                // for (int a = 0; a < nb_towns; a++)
                // {
                //     printf("%d ", path[a]);
                // }
                

                dist = d_matrix[me][i].dist;
                // printf("Processo %d colocou cidade %d na profundidade 1\n", my_rank, town);
                tsp (depth + 1, current_length + dist, path, reductcounter, my_rank, n_procs, first_leaf);
            }
        }        


    }
    else
    {
        int town, me, dist;
        me = path[depth - 1];

        for (i = 0; i < nb_towns; i++) 
        {
            town = d_matrix[me][i].to_town;
            if (!present (town, depth, path)) {
                path[depth] = town;
                dist = d_matrix[me][i].dist;
                tsp (depth + 1, current_length + dist, path, reductcounter, my_rank, n_procs, first_leaf);
            }
        }
    }
    
}

void greedy_shortest_first_heuristic(int *x, int *y) 
{
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

void init_tsp(int my_rank, int n_procs) {
    int i, st;
    int *x, *y;

    min_distance = INT_MAX;

    if (my_rank == 0)
    {
    st = scanf("%u", &nb_towns);
    if (st != 1) exit(1);
    }
    MPI_Bcast(&nb_towns, 1, MPI_INT, 0, MPI_COMM_WORLD);

 
    
    d_matrix = (d_info**) malloc (sizeof(d_info*) * nb_towns);
    for (i = 0; i < nb_towns; i++)
        d_matrix[i] = (d_info*) malloc (sizeof(d_info) * nb_towns);
    dist_to_origin = (int*) malloc(sizeof(int) * nb_towns);
   
    
    x = (int*) malloc(sizeof(int) * nb_towns);
    y = (int*) malloc(sizeof(int) * nb_towns);
    

    for (i = 0; i < nb_towns; i++) {
        if(my_rank == 0)
        {
        st = scanf("%u %u", x + i, y + i);
        if (st != 2) exit(1);
        }
    }

    MPI_Bcast(x, nb_towns, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(y, nb_towns, MPI_INT, 0, MPI_COMM_WORLD);
    
    greedy_shortest_first_heuristic(x, y);
    
    free(x);
    free(y);
}

int run_tsp(int my_rank, int n_procs) {
    int i, *path, firstleaf = 1;
    unsigned int reductcounter =0;
    init_tsp(my_rank, n_procs);
    
    path = (int*) malloc(sizeof(int) * nb_towns);
    path[0] = 0;
    
    tsp (1, 0, path, reductcounter, my_rank, n_procs, firstleaf);
    

    free(path);
    for (i = 0; i < nb_towns; i++)
        free(d_matrix[i]);
    free(d_matrix);

    int local_min_distance;

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Allreduce(&min_distance, &local_min_distance, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    min_distance = local_min_distance;
    return min_distance;
}

int main (int argc, char **argv) {

    ini_time = timestamp();
    MPI_Init(&argc, &argv);
    int num_instances, st, my_rank, n_procs;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    
    if(my_rank == 0)
    {

        st = scanf("%u", &num_instances);
        if (st != 1) exit(1);    
    }

    MPI_Bcast(&num_instances, 1, MPI_INT, 0, MPI_COMM_WORLD);



    while(num_instances-- > 0)
    {
        int dis_min = run_tsp(my_rank, n_procs);
        if (my_rank == 0)
        {
            printf ("%d ", dis_min);
            final_time = timestamp();
            delta_time = final_time - ini_time;
            printf("%15f\n", delta_time);
        }
    }
        
    MPI_Finalize();

    return 0;
}
