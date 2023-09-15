/***********************************************************************
 * Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
 *
 * This software library is licensed under terms of the GNU GENERAL
 * PUBLIC LICENSE
 * 
 *
 * RSCODE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSCODE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Rscode.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial licensing is available under a separate license, please
 * contact author for details.
 *
 * Source code is available at http://rscode.sourceforge.net
 * Berlekamp-Peterson and Berlekamp-Massey Algorithms for error-location
 *
 * From Cain, Clark, "Error-Correction Coding For Digital Communications", pp. 205.
 *
 * This finds the coefficients of the error locator polynomial.
 *
 * The roots are then found by looking for the values of a^n
 * where evaluating the polynomial yields zero.
 *
 * Error correction is done using the error-evaluator equation  on pp 207.
 *
 */

#define RSCODE_INTERNAL
#include "ecc.h"
#ifdef DEBUG
#include <stdio.h>
#endif
#include "rs.h"
#include "galois.h"
#include "berlekamp.h"

/* local ANSI declarations */
static int compute_discrepancy(struct rscode_driver * driver, int lambda[], int S[], int L, int n);
static void init_gamma(struct rscode_driver * driver, int gamma[]);
static void compute_modified_omega (struct rscode_driver * driver);

/********** polynomial arithmetic *******************/

void add_polys (__attribute__((unused)) struct rscode_driver * driver, int dst[], int src[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) dst[i] ^= src[i];
}

void copy_poly (__attribute__((unused)) struct rscode_driver * driver, int dst[], int src[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) dst[i] = src[i];
}

void scale_poly (struct rscode_driver * driver, int k, int poly[]) 
{ 
  int i;
  for (i = 0; i < MAXDEG; i++) poly[i] = gmult(driver, k, poly[i]);
}


void zero_poly (__attribute__((unused)) struct rscode_driver * driver, int poly[]) 
{
  int i;
  for (i = 0; i < MAXDEG; i++) poly[i] = 0;
}


/* multiply by z, i.e., shift right by 1 */
static void mul_z_poly (__attribute__((unused)) struct rscode_driver * driver, int src[])
{
  int i;
  for (i = MAXDEG-1; i > 0; i--) src[i] = src[i-1];
  src[0] = 0;
}

/* polynomial multiplication */
void
mult_polys (struct rscode_driver * driver, int dst[], int p1[], int p2[])
{
  int i, j;
  int tmp1[MAXDEG*2];
  
  for (i=0; i < (MAXDEG*2); i++) dst[i] = 0;
  
  for (i = 0; i < MAXDEG; i++) {
    for(j=MAXDEG; j<(MAXDEG*2); j++) tmp1[j]=0;
    
    /* scale tmp1 by p1[i] */
    for(j=0; j<MAXDEG; j++) tmp1[j]=gmult(driver, p2[j], p1[i]);
    /* and mult (shift) tmp1 right by i */
    for (j = (MAXDEG*2)-1; j >= i; j--) tmp1[j] = tmp1[j-i];
    for (j = 0; j < i; j++) tmp1[j] = 0;
    
    /* add into partial product */
    for(j=0; j < (MAXDEG*2); j++) dst[j] ^= tmp1[j];
  }
}

/* From  Cain, Clark, "Error-Correction Coding For Digital Communications", pp. 216. */
void
Modified_Berlekamp_Massey (struct rscode_driver * driver)
{	
  int n, L, L2, k, d, i;
  int psi[MAXDEG], psi2[MAXDEG], D[MAXDEG];
  int gamma[MAXDEG];
	
  /* initialize Gamma, the erasure locator polynomial */
  init_gamma(driver, gamma);

  /* initialize to z */
  copy_poly(driver, D, gamma);
  mul_z_poly(driver, D);
	
  copy_poly(driver, psi, gamma);	
  k = -1;

#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS
  L = driver->NErasures;
#else
  L = 0;
#endif

  for (n = L/*driver->NErasures*/; n < NPAR; n++) {
	
    d = compute_discrepancy(driver, psi, driver->synBytes, L, n);
		
    if (d != 0) {
		
      /* psi2 = psi - d*D */
      for (i = 0; i < MAXDEG; i++) psi2[i] = psi[i] ^ gmult(driver, d, D[i]);
		
		
      if (L < (n-k)) {
	L2 = n-k;
	k = n-L;
	/* D = scale_poly(ginv(d), psi); */
	for (i = 0; i < MAXDEG; i++) D[i] = gmult(driver, psi[i], ginv(driver, d));
	L = L2;
      }
			
      /* psi = psi2 */
      for (i = 0; i < MAXDEG; i++) psi[i] = psi2[i];
    }
		
    mul_z_poly(driver, D);
  }
	
  for(i = 0; i < MAXDEG; i++) driver->Lambda[i] = psi[i];
  compute_modified_omega(driver);

	
}

/* given Psi (called driver->Lambda in Modified_Berlekamp_Massey) and driver->synBytes,
   compute the combined erasure/error evaluator polynomial as 
   Psi*S mod z^4
  */
void
compute_modified_omega (struct rscode_driver * driver)
{
  int i;
  int product[MAXDEG*2];
	
  mult_polys(driver, product, driver->Lambda, driver->synBytes);	
  zero_poly(driver, driver->Omega);
  for(i = 0; i < NPAR; i++) driver->Omega[i] = product[i];

}

/* gamma = product (1-z*a^Ij) for erasure locs Ij */
void
init_gamma (struct rscode_driver * driver, int gamma[])
{
#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS 
  int e;
#endif
  int tmp[MAXDEG];
	
  zero_poly(driver, gamma);
  zero_poly(driver, tmp);
  gamma[0] = 1;

#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS	
  for (e = 0; e < driver->NErasures; e++) {
    copy_poly(driver, tmp, gamma);
    scale_poly(driver, gexp(driver,driver->ErasureLocs[e]), tmp);
    mul_z_poly(driver, tmp);
    add_polys(driver, gamma, tmp);
  }
#endif
}


#if 0
void 
compute_next_omega (struct rscode_driver * driver, int d, int A[], int dst[], int src[])
{
  int i;
  for ( i = 0; i < MAXDEG;  i++) {
    dst[i] = src[i] ^ gmult(driver, d, A[i]);
  }
}
#endif


int
compute_discrepancy (struct rscode_driver * driver, int lambda[], int S[], int L, int n)
{
  int i, sum=0;
	
  for (i = 0; i <= L; i++) 
    sum ^= gmult(driver, lambda[i], S[n-i]);
  return (sum);
}

/* Finds all the roots of an error-locator polynomial with coefficients
 * driver->Lambda[j] by evaluating driver->Lambda at successive values of alpha. 
 * 
 * This can be tested with the decoder's equations case.
 */


void 
Find_Roots (struct rscode_driver * driver)
{
  int sum, r, k;	
  driver->NErrors = 0;
  
  for (r = 1; r < 256; r++) {
    sum = 0;
    /* evaluate lambda at r */
    for (k = 0; k < NPAR+1; k++) {
      sum ^= gmult(driver, gexp(driver,(k*r)%255), driver->Lambda[k]);
    }
    if (sum == 0) 
      { 
	driver->ErrorLocs[driver->NErrors] = (255-r); driver->NErrors++; 
#ifdef DEBUG
	fprintf(stderr, "Root found at r = %d, (255-r) = %d\n", r, (255-r));
#endif
      }
  }
}

/* Combined Erasure And Error Magnitude Computation 
 * 
 * Pass in the codeword, its size in bytes, as well as
 * an array of any known erasure locations, along the number
 * of these erasures.
 * 
 * Evaluate driver->Omega(actually Psi)/driver->Lambda' at the roots
 * alpha^(-i) for error locs i. 
 *
 * Returns 1 if everything ok, or 0 if an out-of-bounds error is found
 *
 */

int
correct_errors_erasures (struct rscode_driver * driver, unsigned char codeword[], 
			 int csize
#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS
       ,int nerasures,
			 int erasures[]
#endif
       )
{
  int r, i, j, err;

  /* If you want to take advantage of erasure correction, be sure to
     set driver->NErasures and driver->ErasureLocs[] with the locations of erasures. 
     */
#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS
  driver->NErasures = nerasures;
  for (i = 0; i < driver->NErasures; i++) driver->ErasureLocs[i] = erasures[i];
#endif

  Modified_Berlekamp_Massey(driver);
  Find_Roots(driver);
  

  if (((driver->NErrors <= NPAR) && (driver->NErrors > 0))
#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS
  && (driver->NErasures<=(2*NPAR))
#endif
  ) { 

    /* first check for illegal error locs */
    for (r = 0; r < driver->NErrors; r++) {
      if (driver->ErrorLocs[r] >= csize) {
#ifdef DEBUG
	fprintf(stderr, "Error loc i=%d outside of codeword length %d\n", i, csize);
#endif
	return(0);
      }
    }

    for (r = 0; r < driver->NErrors; r++) {
      int num, denom;
      i = driver->ErrorLocs[r];
      /* evaluate driver->Omega at alpha^(-i) */

      num = 0;
      for (j = 0; j < MAXDEG; j++) 
	num ^= gmult(driver, driver->Omega[j], gexp(driver,((255-i)*j)%255));
      
      /* evaluate driver->Lambda' (derivative) at alpha^(-i) ; all odd powers disappear */
      denom = 0;
      for (j = 1; j < MAXDEG; j += 2) {
	denom ^= gmult(driver, driver->Lambda[j], gexp(driver,((255-i)*(j-1)) % 255));
      }
      
      err = gmult(driver, num, ginv(driver, denom));
#ifdef DEBUG
      fprintf(stderr, "Error magnitude %#x at loc %d\n", err, csize-i);
#endif
      
      codeword[csize-i-1] ^= err;
    }
    return(1);
  }
  else {
  #ifdef DEBUG
    if (driver->NErrors) fprintf(stderr, "Uncorrectable codeword\n");
  #endif
    return(0);
  }
}

