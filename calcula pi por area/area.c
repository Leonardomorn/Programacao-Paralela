#include <stdio.h>
#include <omp.h>

static long num_steps = 100000;
double step;

double main()
{
    double sum_array[4], pi;
	#pragma omp parallel
	{
	int ID = omp_get_thread_num();
    long i;
    step = 1.0 / (double) num_steps;
    long t0_step_start = 0;
    long t1_step_start = 1 *num_steps/4;
    long t2_step_start = 2 *num_steps/4;
    long t3_step_start = 3 *num_steps/4;
    switch (ID)
    {
    case 0:
    {
        double x, sum = 0.0;
        for (i = t0_step_start; i < t1_step_start; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x*x);
        }
        sum_array[ID] = sum;
    }
        break;
    case 1:
    {
         double x, sum = 0.0;
        for (i = t1_step_start; i < t2_step_start; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x*x);
        }
        sum_array[ID] = sum;       
    }
        break;
    case 2:
    {
        double x, sum = 0.0;
        for (i = t2_step_start; i < t3_step_start; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x*x);
        }
        sum_array[ID] = sum;       
    }
        break;
    case 3:
    {
         double x, sum = 0.0;
        for (i = t3_step_start; i < num_steps; i++)
        {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x*x);
        }
        sum_array[ID] = sum;       
    }
        break;
    default:
        break;
    }

	}

    pi = step * (sum_array[0] + sum_array[1] + sum_array[2]+ sum_array[3] ); 
    return pi;

		
}

