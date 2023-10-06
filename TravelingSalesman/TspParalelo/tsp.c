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
        
        if(iter < 1 )
        {
            #pragma omp parallel for firstprivate (depth, current_length) private(dist, town, me)
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
                    printf("\nnão são recursões infinitas");
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

/****Imprime a matriz de distâncias para melhor compreensão do que o greedy shortest first faz*/    
/*/
    printf("\nImprimindo a matriz de distâncias \n");
    printf("|tt |   dist   ||tt |   dist   ||tt |   dist   |\n");
    for (int a = 0; a < nb_towns; a++)
    {
        for (int b = 0; b < nb_towns; b++)
        {
            printf(" %d   %.10d  ", d_matrix[a][b].to_town, d_matrix[a][b].dist );

        }
        
        printf("\n");
    }
    
    
    printf("\nImprimindo distancias a origem \n");
    for (int a = 0; a < nb_towns; a++)
    {
        printf(" %d  ", dist_to_origin[a] );

        
    }
        printf("\n");


    printf("\n Distância mínima é: %d\n", min_distance);
*/
    free(x);
    free(y);
}

int run_tsp() {
    int i, *path;

    init_tsp();


    path = (int*) malloc(sizeof(int) * nb_towns);
    path[0] = 0;
 
    
    
    tsp (1, 0, path);

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
        printf("%d\n", run_tsp());
    return 0;
}
