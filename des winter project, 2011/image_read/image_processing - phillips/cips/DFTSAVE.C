

       /******************************************************
       *
       *       file d:\cips\dftsave.c
       *
       *       Functions: This file contains
       *          main
       *          dft
       *          invdft
       *          dft_2d
       *          invdft_2d
       *          odd_dft
       *          odd_invdft
       *          print_real_im
       *          print_2d_real_im
       *
       *       Purpose:
       *          These functions perform forward and 
       *          inverse discrete Fourier transforms
       *          in 1 and 2 dimensions.  The basic algorithms
       *          are from "An Introduction to Digital
       *          Signal Processing," John H. Karl, Academic
       *          Press, 1989.  The basic routines work for even
       *          dimensioned arrays.  For odd dimensioned arrays
       *          you must use the odd routines.
       *
       *       External Calls:
       *          none
       *
       *       Modifications:
       *          05 February 1991 - created
       *
       *****************************************************/


#include "d:\cips\cips.h"


#define N 5
#define M 5
#define pie 3.1425927

float box1[M][N] = {{0., 0.,     0.,    0.,     0.}, 
                    {0.,-1.0   ,-1.0  ,-1.0   , 0.},
                    {0.,-1.0   , 9.0  ,-1.0  ,  0.},
                    {0.,-1.0   ,-1.0  ,-1.0   , 0.},
                    {0., 0.,     0.,    0.,     0.}};

float box2[M][N] = {{0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.}};

float box1_out[M][N] = {{3.5, 3.0, 2.5, 3.0, 3.5},
                    {3.0, 2.5,  1.7, 2.5, 3.0},
                    {2.8, 1.7,  1.0, 1.7, 2.8},
                    {3.0, 2.5 , 1.7, 2.5, 3.0},
                    {3.5, 3.0, 2.5,  3.0, 3.5}};

float box2_out[M][N] = {{0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.},
                    {0., 0., 0., 0., 0.}};


float x[N] = {1., 1., 1.};

float y[N] = {0., 0., 0.};

float r[N], i[N];




main()
{


   /*** 2-D inverse DFT ***/
   printf("\nDFT> This is the 2D input in freq domain\n");
   print_2d_real_im(box1_out, box2_out);
   printf("\nDFT> Now take the inverse DFT and look at time domain\n");
   odd_invdft_2d(box1_out, box2_out, box1, box2);
   print_2d_real_im(box1, box2);





   /****   1-D forward then inverse DFT
   printf("\nDFT> This is the input\n");
   print_real_im(x, y);
   dft(x, y, r, i);
   printf("\nDFT> This is the output\n");
   print_real_im(r, i);

   invdft(r, i, x, y);
   printf("\nDFT> Now reverse the process and look at the data\n");
   print_real_im(x, y);
   ****/
   /*** 2-D forward and inverse DFT
   printf("\nDFT> This is the 2D input\n");
   print_2d_real_im(box1, box2);
   dft_2d(box1, box2, box1_out, box2_out);
   printf("\nDFT> This is the 2D output\n");
   print_2d_real_im(box1_out, box2_out);

   printf("\nDFT> Now reverse the process and look at the data\n");
   invdft_2d(box1_out, box2_out, box1, box2);
   print_2d_real_im(box1, box2);
   ****/

   /*** 2-D inverse DFT
   printf("\nDFT> This is the 2D input in freq domain\n");
   print_2d_real_im(box1_out, box2_out);
   printf("\nDFT> Now take the inverse DFT and look at time domain\n");
   invdft_2d(box1_out, box2_out, box1, box2);
   print_2d_real_im(box1, box2);
   *****/


}  /* ends main */



/*
      This is a simple print routine to 
      look at the 2D real and imaginary
      for small M N 
*/
print_2d_real_im(a, b)
   float a[M][N], b[M][N];
{
   int i, k;
   printf("\nDFT> ");
   for(i=0; i<M; i++){
      printf("\nDFT> ");
      for(k=0; k<N; k++){
         printf(" %2.4f+j%2.4f", a[i][k], b[i][k]);
      }
   }

}  /* ends print_2d_real_im */




/*
      This is a simple print routine to 
      look at the 1D real and imaginary
      for small N 
*/

print_real_im(a, b)
   float a[], b[];
{
   int i;
   printf("\nDFT> ");
   for(i=0; i<N; i++)
      printf("\nDFT>  %f + j%f", a[i], b[i]);

}  /* ends print_real_im */






/*   
   This is the 1D forward DFT.
   This is the centered format.
   This runs from -N/2 to N/2 + 1.
   This works for N even.
*/


dft(x, y, r, i)
   float x[], y[], r[], i[];
{
   int   k, j;
   float c, s, p, w, x0, y0;

   for(k=-N/2; k<=N/2 -1; k++){

      w  = 2. * pie * k/N;
      x0 = 0;
      y0 = 0;

      for(j=-N/2; j<=N/2 - 1; j++){
         /*p  = w * j;*/
         p  = 2. * pie * k * j/N;
         c  = cos(p);
         s  = sin(p);
         x0 = x0 + c*x[j+N/2] + s*y[j+N/2];
         y0 = y0 + c*y[j+N/2] - s*x[j+N/2];
      }  /* ends loop over j */

      r[k+N/2] = x0;
      i[k+N/2] = y0;

   }  /* ends loop over k */

}  /* ends dft */






/*   
   This is the 1D reverse DFT.
   This is the centered format.
   This runs from -N/2 to N/2 + 1.
   This works for N even.
*/


invdft(x, y, r, i)
   float x[], y[], r[], i[];
{
   int   k, j;
   float c, s, p, w, x0, y0;

   for(k=-N/2; k<=N/2 -1; k++){

      w  = -1. * 2. * pie * k/N;
      x0 = 0;
      y0 = 0;

      for(j=-N/2; j<=N/2 - 1; j++){
         p  = w * j;
         c  = cos(p);
         s  = sin(p);
         x0 = x0 + c*x[j+N/2] + s*y[j+N/2];
         y0 = y0 + c*y[j+N/2] - s*x[j+N/2];
      }  /* ends loop over j */

      r[k+N/2] = x0/N;
      i[k+N/2] = y0/N;
   }  /* ends loop over k */

}  /* ends invdft */





/*   
   This is the 1D forward DFT.
   This is the centered format.
   This runs from -N/2 to N/2.
   This works for N odd.
*/


odd_dft(x, y, r, i)
   float x[], y[], r[], i[];
{
   int   k, j;
   float c, s, p, w, x0, y0;

   for(k=-N/2; k<=N/2; k++){

      w  = 2. * pie * k/N;
      x0 = 0;
      y0 = 0;

      for(j=-N/2; j<=N/2; j++){
         /*p  = w * j;*/
         p  = 2. * pie * k * j/N;
         c  = cos(p);
         s  = sin(p);
         x0 = x0 + c*x[j+N/2] + s*y[j+N/2];
         y0 = y0 + c*y[j+N/2] - s*x[j+N/2];
      }  /* ends loop over j */

      r[k+N/2] = x0;
      i[k+N/2] = y0;

   }  /* ends loop over k */

}  /* ends odd_dft */



/*   
   This is the 1D reverse DFT.
   This is the centered format.
   This runs from -N/2 to N/2.
   This works for N odd.
*/


odd_invdft(x, y, r, i)
   float x[], y[], r[], i[];
{
   int   k, j;
   float c, s, p, w, x0, y0;

   for(k=-N/2; k<=N/2; k++){

      w  = -1. * 2. * pie * k/N;
      x0 = 0;
      y0 = 0;

      for(j=-N/2; j<=N/2; j++){
         p  = w * j;
         c  = cos(p);
         s  = sin(p);
         x0 = x0 + c*x[j+N/2] + s*y[j+N/2];
         y0 = y0 + c*y[j+N/2] - s*x[j+N/2];
      }  /* ends loop over j */

      r[k+N/2] = x0/N;
      i[k+N/2] = y0/N;
   }  /* ends loop over k */

}  /* ends odd_invdft */







/*   
   This is the forward 2D DFT.
   This is the centered format.
   This runs from -N/2 to N/2 + 1
   This runs from -M/2 to M/2 + 1
   This works for MxN matrices
   where M and N are even.

   x is the input array real part.
   y is the input array imaginary part.
   r is the output array real part.
   i is the output array imaginary part.

   The time domain h has subscripts
   m n.  The freq domain H has
   subscripts u v.  These are both
   varied over M N.

   The angle p = -2jpienu/N  - 2jpiemv/M
             p = 2jpie[ (-nuM) - (mvN)]/MN
*/


dft_2d(x, y, r, i)
   float x[M][N], y[M][N], r[M][N], i[M][N];
{
   int   n, m, u, v;
   float c, s, p, w, x0, y0;

   for(v=-M/2; v<=M/2 -1; v++){
      for(u=-N/2; u<=N/2 -1; u++){

         x0 = 0;
         y0 = 0;

         for(m=-M/2; m<=M/2 - 1; m++){
            for(n=-N/2; n<=N/2 - 1; n++){
                     /* you can probably separate the following
                        to increase speed */
               p  = 2. * pie * (n*u*M + m*v*N) / (N*M);
               c  = cos(p);
               s  = sin(p);
               x0 = x0 + c*x[m+M/2][n+N/2] + s*y[m+M/2][n+N/2];
               y0 = y0 + c*y[m+M/2][n+N/2] - s*x[m+M/2][n+N/2];
            }  /* ends loop over n */
         }  /* ends loop over m */

         r[v+M/2][u+N/2] = x0;
         i[v+M/2][u+N/2] = y0;
      }  /* ends loop over u */
   }  /* ends loop over v */

}  /* ends dft_2d */





/*   
   This is the reverse 2D DFT.
   This is the centered format.
   This runs from -N/2 to N/2 + 1
   This runs from -M/2 to M/2 + 1
   This works for MxN matrices
   where M and N are even.

   x is the input array real part.
   y is the input array imaginary part.
   r is the output array real part.
   i is the output array imaginary part.

   The time domain h has subscripts
   m n.  The freq domain H has
   subscripts u v.  These are both
   varied over M N.

   The angle p = -2jpienu/N  - 2jpiemv/M
             p = 2jpie[ (-nuM) - (mvN)]/MN
*/



invdft_2d(x, y, r, i)
   float x[M][N], y[M][N], r[M][N], i[M][N];
{
   int   n, m, u, v;
   float c, s, p, w, x0, y0;

   for(v=-M/2; v<=M/2 -1; v++){
      for(u=-N/2; u<=N/2 -1; u++){

         x0 = 0;
         y0 = 0;

         for(m=-M/2; m<=M/2 - 1; m++){
            for(n=-N/2; n<=N/2 - 1; n++){
                     /* you can probably separate the following
                        to increase speed */
               p  = 2. * pie * (-1*n*u*M - m*v*N) / (N*M);
               c  = cos(p);
               s  = sin(p);
               x0 = x0 + c*x[m+M/2][n+N/2] + s*y[m+M/2][n+N/2];
               y0 = y0 + c*y[m+M/2][n+N/2] - s*x[m+M/2][n+N/2];
            }  /* ends loop over n */
         }  /* ends loop over m */

         r[v+M/2][u+N/2] = x0/(M*N);
         i[v+M/2][u+N/2] = y0/(M*N);
      }  /* ends loop over u */
   }  /* ends loop over v */

}  /* ends invdft_2d */






/*   
   This is the forward 2D DFT.
   This is the centered format.
   This runs from -N/2 to N/2 
   This runs from -M/2 to M/2
   This works for MxN matrices
   where M and N are odd.

   x is the input array real part.
   y is the input array imaginary part.
   r is the output array real part.
   i is the output array imaginary part.

   The time domain h has subscripts
   m n.  The freq domain H has
   subscripts u v.  These are both
   varied over M N.

   The angle p = -2jpienu/N  - 2jpiemv/M
             p = 2jpie[ (-nuM) - (mvN)]/MN
*/


odd_dft_2d(x, y, r, i)
   float x[M][N], y[M][N], r[M][N], i[M][N];
{
   int   n, m, u, v;
   float c, s, p, w, x0, y0;

   for(v=-M/2; v<=M/2; v++){
      for(u=-N/2; u<=N/2; u++){

         x0 = 0;
         y0 = 0;

         for(m=-M/2; m<=M/2; m++){
            for(n=-N/2; n<=N/2; n++){
                     /* you can probably separate the following
                        to increase speed */
               p  = 2. * pie * (n*u*M + m*v*N) / (N*M);
               c  = cos(p);
               s  = sin(p);
               x0 = x0 + c*x[m+M/2][n+N/2] + s*y[m+M/2][n+N/2];
               y0 = y0 + c*y[m+M/2][n+N/2] - s*x[m+M/2][n+N/2];
            }  /* ends loop over n */
         }  /* ends loop over m */

         r[v+M/2][u+N/2] = x0;
         i[v+M/2][u+N/2] = y0;
      }  /* ends loop over u */
   }  /* ends loop over v */

}  /* ends odd_dft_2d */





/*   
   This is the reverse 2D DFT.
   This is the centered format.
   This runs from -N/2 to N/2
   This runs from -M/2 to M/2
   This works for MxN matrices
   where M and N are odd.

   x is the input array real part.
   y is the input array imaginary part.
   r is the output array real part.
   i is the output array imaginary part.

   The time domain h has subscripts
   m n.  The freq domain H has
   subscripts u v.  These are both
   varied over M N.

   The angle p = -2jpienu/N  - 2jpiemv/M
             p = 2jpie[ (-nuM) - (mvN)]/MN
*/



odd_invdft_2d(x, y, r, i)
   float x[M][N], y[M][N], r[M][N], i[M][N];
{
   int   n, m, u, v;
   float c, s, p, w, x0, y0;

   for(v=-M/2; v<=M/2; v++){
      for(u=-N/2; u<=N/2; u++){

         x0 = 0;
         y0 = 0;

         for(m=-M/2; m<=M/2; m++){
            for(n=-N/2; n<=N/2; n++){
                     /* you can probably separate the following
                        to increase speed */
               p  = 2. * pie * (-1*n*u*M - m*v*N) / (N*M);
               c  = cos(p);
               s  = sin(p);
               x0 = x0 + c*x[m+M/2][n+N/2] + s*y[m+M/2][n+N/2];
               y0 = y0 + c*y[m+M/2][n+N/2] - s*x[m+M/2][n+N/2];
            }  /* ends loop over n */
         }  /* ends loop over m */

         r[v+M/2][u+N/2] = x0/(M*N);
         i[v+M/2][u+N/2] = y0/(M*N);
      }  /* ends loop over u */
   }  /* ends loop over v */

}  /* ends odd_invdft_2d */

