#include <stdio.h>
#include <omp.h>

float f(float x){
    return x;
}

main(int argc, char *argv[]){

    float a,b,step;
    a = 10;
    b = 100;
    step = 5;
    int i;
    float x,x1;
    float A,sum;

    if(argc == 3){
        a = atoi(argv[1]);
        b = atoi(argv[2]);
    }
    /*FUNCAO DO CALCULO DA AREA DOS TRAPEZIOS;
    At = ((f(xn) + f(xn+1))/2) * (xn+1 - xn)
    */
    sum = 0;
    x=a;
    //Parte sequencial
    while(x <= b){
        x1 = x + step;
        A = ((f(x) + f(x1))/2) * (x1 - x);
        sum += A;
        x = x1;
    }
    printf("Resultado sequencial : %f \n\n",sum);

	i = (int)((b - a)/step);
	int j;
    sum = 0;
	x=a;



    #pragma omp parallel for private(A,x1,x),reduction(+:sum)
    for(j = 0; j <= i; j ++){

        x = a + (j * step);
        x1 = x + step;
        A = ((f(x) + f(x1))/2) * (x1 - x);
        sum+=A;
      //  x = x1;
    }

	#pragma omp parallel
	{
	int nthreads = omp_get_num_threads();
    printf("Nthreads : %d \n", nthreads);
	}
    printf("Resultado paralelo : %f \n\n",sum);
}
