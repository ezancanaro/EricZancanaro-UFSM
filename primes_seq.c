#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int prime_v1(int n);
int prime_v2(int n);
int prime_v3(int n);

int main(int argc, char *argv[])
{
    int n;
    int primes;
    double time1;
    double time2;
    double time3;
    double time4;

    n = 524288;
    int a,b,c;

    printf("2 THREADS\n");
    printf("                                V1            V2            V3            VSequencial\n");
    printf("         N     Pi(N)          Time          Time          Time          Time\n");
    printf("\n");


    omp_set_num_threads(2);

    printf("%d",omp_get_num_threads());
    time1 = omp_get_wtime();
    primes = prime_v1(n);
    time1 = omp_get_wtime() - time1;

    a=primes;
    time2 = omp_get_wtime();
    primes = prime_v2(n);
    time2 = omp_get_wtime() - time2;

    b=primes;
    time3 = omp_get_wtime();
    primes = prime_v3(n);
    time3 = omp_get_wtime() - time3;

    time4 = omp_get_wtime();
    primes = prime_v4(n);
    time4 = omp_get_wtime() - time4;

    c=primes;
    printf ("  %8d  %8d  %12f  %12f  %12f  %12f\n", n, primes, time1, time2, time3, time4);

    printf ("%d  %d   %d \n",a,b,c);


//// 4 THREADS
    printf("4 THREADS\n");
    printf("                                V1            V2            V3            VSequencial\n");
    printf("         N     Pi(N)          Time          Time          Time          Time\n");
    printf("\n");

    omp_set_num_threads(4);

    printf("%d",omp_get_num_threads());
    time1 = omp_get_wtime();
    primes = prime_v1(n);
    time1 = omp_get_wtime() - time1;

    a=primes;
    time2 = omp_get_wtime();
    primes = prime_v2(n);
    time2 = omp_get_wtime() - time2;

    b=primes;
    time3 = omp_get_wtime();
    primes = prime_v3(n);
    time3 = omp_get_wtime() - time3;

    time4 = omp_get_wtime();
    primes = prime_v4(n);
    time4 = omp_get_wtime() - time4;

    c=primes;
    printf ("  %8d  %8d  %12f  %12f  %12f  %12f\n", n, primes, time1, time2, time3, time4);

    printf ("%d  %d   %d \n",a,b,c);


    // THREADS 1
    printf("1 THREADS\n");
  printf("                                V1            V2            V3            VSequencial\n");
  printf("         N     Pi(N)          Time          Time          Time          Time\n");
  printf("\n");


  omp_set_num_threads(1);

    printf("%d",omp_get_num_threads());
  time1 = omp_get_wtime();
  primes = prime_v1(n);
  time1 = omp_get_wtime() - time1;

    a=primes;
  time2 = omp_get_wtime();
  primes = prime_v2(n);
  time2 = omp_get_wtime() - time2;

    b=primes;
  time3 = omp_get_wtime();
  primes = prime_v3(n);
  time3 = omp_get_wtime() - time3;

    time4 = omp_get_wtime();
  primes = prime_v4(n);
  time4 = omp_get_wtime() - time4;

    c=primes;
  printf ("  %8d  %8d  %12f  %12f  %12f  %12f\n", n, primes, time1, time2, time3, time4);

    printf ("%d  %d   %d \n",a,b,c);
    return 0;
}

/******************************************************************************/
int prime_v1(int n)
/******************************************************************************/
{
    int i;
    int j;
    int prime;
    int total = 0;

    #pragma omp parallel for private(i, j, prime) shared(total)
    for (i = 2; i <= n; i++)
    {
        prime = 1;
        for (j = 2; j < i; j++)
        {
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        }
        #pragma omp critical
        total = total + prime;
    }
    return total;
}

/******************************************************************************/
int prime_v2(int n)
/******************************************************************************/
{
    int i;
    int j;
    int prime;
    int total = 0;

    #pragma omp parallel for private(i, j, prime) schedule(dynamic) reduction(+:total)
    for (i = 2; i <= n; i++)
    {
        prime = 1;
        for (j = 2; j < i; j++)
        {
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        }

        total = total + prime;
    }
    return total;
}

/******************************************************************************/
int prime_v3(int n)
/******************************************************************************/
{
    int i;
    int j;
    int prime;
    int total = 0;

    #pragma omp parallel for private(i, j, prime) schedule(runtime)
    for (i = 2; i <= n; i++)
    {
        prime = 1;
        for (j = 2; j < i; j++)
        {
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        }
        #pragma omp atomic
        total = total + prime;
    }
    return total;
}

/******************************************************************************/
int prime_v4(int n)
/******************************************************************************/
{
    int i;
    int j;
    int prime;
    int total = 0;

    for (i = 2; i <= n; i++)
    {
        prime = 1;
        for (j = 2; j < i; j++)
        {
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        }
        total = total + prime;
    }
    return total;
}
