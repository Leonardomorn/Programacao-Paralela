#include <stdio.h>

float calcula_eficiencia(int serial, int paralelo, int procesadores)
{
    float eficiencia = serial / (paralelo * procesadores);
    return eficiencia;
}

void main ()
{
    printf("\nE = %f", calcula_eficiencia(10, 4, 4));
}