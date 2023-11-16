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


int min_distance;
int nb_towns;
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

void tsp (int depth, int current_length, int *path, int my_rank, int n_proc) {
    MPI_Status status;


    int i;
    if (current_length >= min_distance) return; //corte por otimização
    if (depth == nb_towns) {
        current_length += dist_to_origin[path[nb_towns - 1]];
        if (current_length < min_distance)
        {
            min_distance = current_length;
            MPI_Allreduce(&min_distance, &min_distance, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        }
    }
    if(depth == 1)
    {
        
    }
    else
    {
        int town, me, dist;
        me = path[depth - 1];
        for (i = 0; i < nb_towns; i++) {
            town = d_matrix[me][i].to_town;
            if (!present (town, depth, path)) {
                path[depth] = town;
                dist = d_matrix[me][i].dist;
                tsp (depth + 1, current_length + dist, path, my_rank, n_proc);
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
 
    d_matrix = (d_info**) malloc (sizeof(d_info*) * nb_towns);
    for (i = 0; i < nb_towns; i++)
        d_matrix[i] = (d_info*) malloc (sizeof(d_info) * nb_towns);
    dist_to_origin = (int*) malloc(sizeof(int) * nb_towns);
   
    x = (int*) malloc(sizeof(int) * nb_towns);
    y = (int*) malloc(sizeof(int) * nb_towns);
    

    for (i = 0; i < nb_towns; i++) {
        st = scanf("%u %u", x + i, y + i);
        if (st != 2) exit(1);
    }
    
    greedy_shortest_first_heuristic(x, y);
    
    free(x);
    free(y);
}


void get_input(int my_rank, int n_proc, int *path, MPI_Status status)
{

    MPI_Bcast(&min_distance, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 1; i < n_proc; i++) {
    printf("Processo %d: %s \n", status.MPI_SOURCE, min_distance);
    }
    
    printf("sou o processo %d e a min_distance é: %d \n", my_rank, min_distance);
    MPI_Bcast(&nb_towns, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(dist_to_origin, nb_towns, MPI_INT,0, MPI_COMM_WORLD);
    if(my_rank != 0)
    {
        MPI_Alloc_mem(sizeof(int) * nb_towns, MPI_INFO_NULL, path);
    }
    MPI_Bcast(path, nb_towns, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(d_matrix, 2, MPI_INT, 0, MPI_COMM_WORLD );


}

int run_tsp(int *argc, char ***argv) 
{
    int i, *path;
    MPI_Status status;

    init_tsp();
    
    path = (int*) malloc(sizeof(int) * nb_towns);
    path[0] = 0;
    
    min_distance = 10;
    MPI_Init(argc, argv);
    int my_rank, n_proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);
    get_input(my_rank, n_proc, path, status);
    tsp (1, 0, path, my_rank, n_proc);
    MPI_Finalize();

    free(path);
    for (i = 0; i < nb_towns; i++)
        free(d_matrix[i]);
    free(d_matrix);

    return min_distance;
}

int main (int argc, char **argv) {
    int num_instances, st;
    st = scanf("%u", &num_instances);
    if (st != 1) exit(1);
    while(num_instances-- > 0)
        printf("distancia encontrada %d\n", run_tsp(&argc, &argv));
    return 0;
}