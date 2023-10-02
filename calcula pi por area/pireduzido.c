#include <stdio.h>

static long num_steps = 100000;
double step;

int main(){
    int i; double pi = 0.0, sum = 0.0;
    step = 1.0/(double) num_steps;

    #pragma omp parallel 
    {

    double x;
    #pragma omp for    
    for (i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step; // largura do retangulo
        sum = sum + 4.0 / (1.0 + x*x); // sum
    }
    #pragma omp critical
    pi = step * sum;
    } 
    printf("\n O pi paralelo reduzido é: %f\n", pi);
    

}

