static long num_steps = 100000;
double step;

int main(){
    int i; double x, pi, sum = 0.0;
    step = 1.0/(double) num_steps;
    for (i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step // largura do retangulo
        sum = sum + 4.0 / (1.0 + x*x); // sum
    }
    pi = step * sum;
}