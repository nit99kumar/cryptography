
   /********************************************
   *
   *   File d:\cips\mainfit.c
   *
   *   This is a main routine that calls the
   *   fit routine as a test.  It tests the
   *   ability of fit to fit data to a
   *   straight line y=mx+b.
   *
   *   21 August 1993
   *
   ********************************************/

#include <stdio.h>
#include <math.h>
#define N 7

main()
{
   float x[N+1], y[N+1], sig[N+1], a, b, siga, sigb, chi2, q;
   int   ndata, mwt;

   int i;

   for(i=1; i<=N; i++){
      sig[i] = 1.0;
   }

   x[1] = 0.0;        /* ln(1)        */
   x[2] = 0.34657359; /* ln(sqrt(2))  */
   x[3] = 0.69314718; /* ln(2)        */
   x[4] = 0.80471896; /* ln(sqrt(5))  */
   x[5] = 1.03972077; /* ln(sqrt(8))  */
   x[6] = 1.09861229; /* ln(3)        */
   x[7] = 1.15129255; /* ln(sqrt(10)) */

    /********
   y[1] = log(15.);
   y[2] = log(18.);
   y[3] = log(21.);
   y[4] = log(26.);
   y[5] = log(30.);
   y[6] = log(32.);
   y[7] = log(35.);

   y[1] = log(45.);
   y[2] = log(59.);
   y[3] = log(98.);
   y[4] = log(117.);
   y[5] = log(140.);
   y[6] = log(152.);
   y[7] = log(159.);

   y[1] = log(5.);
   y[2] = log(5.);
   y[3] = log(10.);
   y[4] = log(10.);
   y[5] = log(10.);
   y[6] = log(15.);
   y[7] = log(20.);

   y[1] = log(15.);
   y[2] = log(20.);
   y[3] = log(20.);
   y[4] = log(40.);
   y[5] = log(45.);
   y[6] = log(45.);
   y[7] = log(50.);
   ********/

   y[1] = log(5.);
   y[2] = log(15.);
   y[3] = log(20.);
   y[4] = log(30.);
   y[5] = log(45.);
   y[6] = log(50.);
   y[7] = log(60.);

   ndata = N;
   mwt   = 1;
   fit(x, y, ndata, sig, mwt, &a, &b, &siga, &sigb, &chi2, &q);

   printf("\nThe answers");
   printf("\n\t   a=%f    b=%f", a, b);
   printf("\n\tsiga=%f sigb=%f", siga, sigb);
   printf("\n\tchi2=%f    q=%f", chi2, q);

}
