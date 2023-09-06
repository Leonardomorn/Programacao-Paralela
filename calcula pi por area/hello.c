#include <stdio.h>
#include <omp.h>
#include <unistd.h>

int main()
{
	int ID = 0;
	
	#pragma omp parallel
	{
	sleep(1);
	int ID = omp_get_thread_num();
	printf(" hello(%d) ", ID);
	printf(" world(%d) \n", ID);
	}
		
}

