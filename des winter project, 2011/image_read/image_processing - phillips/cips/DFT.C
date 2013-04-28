

       /******************************************************
       *
       *       file d:\cips\dft.c
       *
       *       Functions: This file contains
       *          dft
       *          invdft
       *          dft_2d
	   *          invdft_2d
	   *          perform_fourier_transform
       *          print_real_im
       *          print_2d_real_im
       *
       *       Purpose:
       *          These functions perform forward and 
       *          inverse discrete Fourier transforms
       *          in 1 and 2 dimensions.  The basic algorithms
       *          are from "An Introduction to Digital
       *          Signal Processing," John H. Karl, Academic
       *          Press, 1989.
       *
       *       External Calls:
       *          none
       *
       *       Modifications:
       *          05 February 1991 - created
       *
       *****************************************************/


#include "d:\cips\cips.h"

#define M 10
#define N 10

short hi_pass[M][N] = {{10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
                       {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
                       {10, 10, 10, 10,  5,  5, 10, 10, 10, 10},
                       {10, 10, 10,  5,  5,  5,  5, 10, 10, 10},
                       {10, 10,  5,  5,  5,  5,  5,  5, 10, 10},
                       {10, 10,  5,  5,  5,  5,  5,  5, 10, 10},
                       {10, 10, 10,  5,  5,  5,  5, 10, 10, 10},
                       {10, 10, 10, 10,  5,  5, 10, 10, 10, 10},
                       {10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
                       {10, 10, 10, 10, 10, 10, 10, 10, 10, 10}};


#define pie 3.1425927


perform_fourier_transform(in_name, out_name, image1, 
         image2, image3, image4, il, ie, ll, le)
   char   in_name[], out_name[];
   int	  il, ie, ll, le;
   short  image1[ROWS][COLS], image2[ROWS][COLS],
          image3[ROWS][COLS], image4[ROWS][COLS];
{

   int i, j, k, nn[2];

   struct   tiff_header_struct image_header;


   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
	     image1[i][j] = 0;
	     image2[i][j] = 0;
	     image3[i][j] = 0;
	     image4[i][j] = 0;
      }
   }


   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
	     image3[i][j] = hi_pass[i][j];

   print_2d_real(image3);
   invdft_2d(image3, image4, image1, image2);
   printf("\nDFT> real part of transform");
   print_2d_real(image1);
   printf("\nDFT> im part of transform");
   print_2d_real(image2);
   calculate_magnitude(image1, image2);
   printf("\nDFT> After combining real and im parts");
   print_2d_real(image1);


   /*
   read_tiff_image(in_name, image1, il, ie, ll, le);

   dft_2d(image1, image2, image3, image4);

   write_array_into_tiff_image(out_name, image3,
			       il, ie, ll, le);
   */


}  /* ends perform_fourier_transform */







/*
      This is a simple print routine to 
      look at the 2D real and imaginary
      for small M N 
*/
print_2d_real_im(a, b)
   short a[M][N], b[M][N];
{
   int i, k;
   printf("\nDFT> ");
   for(i=0; i<M; i++){
      printf("\nDFT> ");
      for(k=0; k<N; k++){
         printf(" %2d+j%2d", a[i][k], b[i][k]);
      }
   }

}  /* ends print_2d_real_im */




print_2d_real(a)
   short a[M][N];
{
   int i, k;
   printf("\nDFT> ");
   for(i=0; i<M; i++){
      printf("\nDFT> ");
      for(k=0; k<N; k++){
         printf(" %2d", a[i][k]);
      }
   }

}  /* ends print_2d_real */




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
   This runs from -COLS/2 to COLS/2 + 1
*/


dft(x, y, r, i)
   float x[], y[], r[], i[];
{
   int   k, j;
   float c, s, p, w, x0, y0;

   for(k=-COLS/2; k<=COLS/2 -1; k++){

      w  = 2. * pie * k/COLS;
      x0 = 0;
      y0 = 0;

      for(j=-COLS/2; j<=COLS/2 - 1; j++){
         /*p  = w * j;*/
         p  = 2. * pie * k * j/COLS;
         c  = cos(p);
         s  = sin(p);
         x0 = x0 + c*x[j+COLS/2] + s*y[j+COLS/2];
         y0 = y0 + c*y[j+COLS/2] - s*x[j+COLS/2];
      }  /* ends loop over j */

      r[k+COLS/2] = x0;
      i[k+COLS/2] = y0;

   }  /* ends loop over k */

}  /* ends dft */






/*   
   This is the 1D reverse DFT.
   This is the centered format.
   This runs from -COLS/2 to COLS/2 + 1
*/


invdft(x, y, r, i)
   float x[], y[], r[], i[];
{
   int   k, j;
   float c, s, p, w, x0, y0;

   for(k=-COLS/2; k<=COLS/2 -1; k++){

      w  = -1. * 2. * pie * k/COLS;
      x0 = 0;
      y0 = 0;

      for(j=-COLS/2; j<=COLS/2 - 1; j++){
         p  = w * j;
         c  = cos(p);
         s  = sin(p);
         x0 = x0 + c*x[j+COLS/2] + s*y[j+COLS/2];
         y0 = y0 + c*y[j+COLS/2] - s*x[j+COLS/2];
      }  /* ends loop over j */

      r[k+COLS/2] = x0/COLS;
      i[k+COLS/2] = y0/COLS;
   }  /* ends loop over k */

}  /* ends invdft */








/*   
   This is the forward 2D DFT.
   This is the centered format.
   This runs from -N/2 to N/2 + 1
   This runs from -M/2 to M/2 + 1
   This works for MxN matrices

   The time domain h has subscripts
   m n.  The freq domain H has
   subscripts u v.  These are both
   varied over M N.

   The angle p = -2jpienu/N  - 2jpiemv/M
             p = 2jpie[ (-nuM) - (mvN)]/MN

   Making substitutions to speed up processing

*/


dft_2d(x, y, r, i)
   short x[ROWS][COLS], y[ROWS][COLS], r[ROWS][COLS], i[ROWS][COLS];
{
   int   n, m, u, v, um, vn, mvn, M_2, N_2;
   float c, s, p, w, x0, y0, twopie_d;


   M_2      = M/2;
   N_2      = N/2;
   twopie_d = (2. * pie)/(M*N);


   for(v=-M_2; v<=M_2-1; v++){
      for(u=-N_2; u<=N_2 -1; u++){
printf("\n      v=%3d u%3d--", v, u);


         um = u*M;
		 vn = v*N;


         x0 = 0;
         y0 = 0;

         for(m=-M_2; m<=M_2 - 1; m++){

		 mvn = m*vn;


printf(" m%2d", m);
            for(n=-N_2; n<=N_2 - 1; n++){
                     /* you can probably separate the following
                        to increase speed */
               /**p  = 2. * pie * (n*u*M + m*v*N) / (N*M);**/
			   p  = twopie_d * (n*um + mvn); 
               c  = cos(p);
               s  = sin(p);
			      /* the y array is all zero is remove it
				     from the calculations
				  */
				  /*****
               x0 = x0 + c*x[m+M_2][n+N_2] + s*y[m+M_2][n+N_2];
               y0 = y0 + c*y[m+M_2][n+N_2] - s*x[m+M_2][n+N_2];
			      *****/
               x0 = x0 + c*x[m+M_2][n+N_2];
               y0 = y0 - s*x[m+M_2][n+N_2];
            }  /* ends loop over n */
         }  /* ends loop over m */

         r[v+M_2][u+N_2] = x0;
         i[v+M_2][u+N_2] = y0;
      }  /* ends loop over u */
   }  /* ends loop over v */

}  /* ends dft_2d */






/*   
   This is the reverse 2D DFT.
   This is the centered format.
   This runs from -N/2 to N/2 + 1
   This runs from -M/2 to M/2 + 1
   This works for MxN matrices

   The time domain h has subscripts
   m n.  The freq domain H has
   subscripts u v.  These are both
   varied over M N.

   The angle p = -2jpienu/N  - 2jpiemv/M
             p = 2jpie[ (-nuM) - (mvN)]/MN
*/



invdft_2d(x, y, r, i)
   short x[M][N], y[M][N], r[M][N], i[M][N];
{
   int   n, m, u, v;
   float c, s, p, w, x0, y0;

   for(v=-M/2; v<=M/2 -1; v++){
      for(u=-N/2; u<=N/2 -1; u++){
printf("\n      v=%3d u%3d--", v, u);

         x0 = 0;
         y0 = 0;

         for(m=-M/2; m<=M/2 - 1; m++){
printf(" m%2d", m);
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
   This function takes in two arrays (real, im)
   and returns the magnitude = sqrt(a*a + b*b)
   in the first array.
*/

calculate_magnitude(a, b)
   short a[ROWS][COLS], b[ROWS][COLS];
{
   double aa, bb, x, y;
   int i, j;

   printf("\nCALC MAG> ");
   for(i=0; i<ROWS; i++){
      if( (i%10) == 0) printf(" %3d", i);
      for(j=0; j<COLS; j++){
	     aa = a[i][j];
		 bb = b[i][j];
	     x  = aa*aa + bb*bb;
		 y = sqrt(x);
		 a[i][j] = (short)(y);
	  }
   }

}  /* ends calculate_magnitude */




/*
   Replaces data by its ndim-dimensional discrete Fourier transform,
   if isign is input as 1.  nn[1..ndim] is an integer array containing 
   the lengths of each dimension (number of complex values), which
   MUST all be powers of 2.  data is a real array of length twive
   the product of these lengths, in which the data are stored as
   in a multidimensional complex array:  real and imaginary parts
   of each element are in consecutive locations, and the
   rightmost index of the array increases most rapidly as one
   proceedsx along data.  For a two-dimensional array, this is
   equivalent to storing the array by rows.  If isign is input as -1,
   data is replaced by its inverse transform times the product of the
   lengths of all dimensions.
*/

#define SWAP(a, b) tempr = (a); (a) = (b); (b) = tempr;


fourn(data, nn, ndim, isign)
   short data[];
   int nn[], ndim, isign;
{
   int     i1, i2, i3, i2rev, i3rev, 
           ip1, ip2, ip3, ifp1, ifp2;
   int     ibit, idim, k1, k2, n, nprev, nrem, ntot;
   float   tempi, tempr;
   double  theta, wi, wpi, wpr, wr, wtemp;


   ntot = 1;
   for(idim=1; idim<=ndim; idim--)
      ntot *= nn[idim];

printf("\nFOURN> %d dimensions", ndim);

   nprev = 1;
   for(idim=ndim; idim>=1; idim--){  /* main loop */
      n = nn[idim];
	  nrem = ntot/(n*nprev);
	  ip1  = nprev << 1;
printf("\nFOURN> ip1 = %d ", ip1);
	  ip2  = ip1*n;
	  ip3  = ip2*nrem;
	  i2rev = 1;

	  for(i2=1; i2<=ip2; i2+=ip1){
         if(i2 < i2rev){
            for(i1=i2; i1<=i2+ip1-2; i1+=2){
               for(i3=i1; i3<=ip3; i3+=ip2){
                  i3rev = i2rev + i3 - i2;
				  SWAP(data[i3], data[i3rev]);
				  SWAP(data[i3+1], data[i3rev+1]);
			   } /*  ends loop over i3 */
			}  /* ends loop over i1 */
		 }  /* ends if i2 < i2rev */

         ibit = ip2 >> 1;

		 while(ibit >= ip1 && i2rev > ibit){
		    i2rev -= ibit;
			ibit >>=1;
		 }  /* ends while ibit */
         i2rev += ibit;
	  }  /* ends loop over i2 */

printf("\nFOURN> ip1 = %d ", ip1);
	  ifp1 = ip1;
printf("\nFOURN> ifp1=%d ", ifp1);
	  while(ifp1 < i2){
printf("\nFOURN> ifp1=%d ", ifp1);
         ifp2 = ifp1 <<1;
		 theta = isign * 6.28318530717959/(ifp2/ip1);
		 wtemp = sin(0.5*theta);
		 wpr = -2.0*wtemp*wtemp;
		 wpi = sin(theta);
		 wr = 1.0;
		 wi = 0.0;

		 for(i3=1; i3<=ifp1; i3+=ip1){
printf(" i3=%d", i3);
            for(i1=i3; i1<=i3+ip1-2; i1+=2){
               for(i2=i1; i2<=ip3; i2+=ifp2){
                  k1 = i2;
				  k2 = k1 + ifp1;
				  tempr = wr*data[k2] - wi*data[k2+1];
				  tempi = wr*data[k2+1] + wi*data[k2];
				  data[k2] = data[k1] - tempr;
				  data[k2+1] = data[k1+1] - tempi;
				  data[k1] += tempr;
				  data[k1+1] += tempi;
			   }  /* ends loop over i2 */
			}  /* ends loop over i1 */

			wr = (wtemp=wr)*wpr - wi*wpi + wr; /* trig recurrence */
			wi = wi*wpr + wtemp*wpi + wi;

		 }  /* ends loop over i3 */

		 ifp1 = ifp2;
	  }  /* ends while ifp1 */

      nprev *= n;
   }  /* ends main loop over idim */

}  /* ends fourn */
