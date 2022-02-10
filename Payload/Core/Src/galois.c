/*****************************
 * Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
 *
 * This software library is licensed under terms of the GNU GENERAL
 * PUBLIC LICENSE
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

 * Commercial licensing is available under a separate license, please
 * contact author for details.
 *
 * Source code is available at http://rscode.sourceforge.net
 * 
 *
 * Multiplication and Arithmetic on Galois Field GF(256)
 *
 * From Mee, Daniel, "Magnetic Recording, Volume III", Ch. 5 by Patel.
 * 
 * 
 * Part of software was licensed by GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * Galois.c
 * James S. Plank
 * April, 2007
 * Galois.tar - Fast Galois Field Arithmetic Library in C/C+
 * Copright (C) 2007 James S. Plank
 * 
 * James S. Plank
 * Department of Computer Science
 * University of Tennessee
 * Knoxville, TN 37996
 * plank@cs.utk.edu
 * 
 * http://web.eecs.utk.edu/~plank/plank/papers/CS-07-593/
 *
 ******************************/
 
#define RSCODE_INTERNAL
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include "ecc.h"
#include "galois.h"

/* This is one of 14 irreducible polynomials
 * of degree 8 and cycle length 255. (Ch 5, pp. 275, Magnetic Recording)
 * The high order 1 bit is implicit */
/* x^8 + x^4 + x^3 + x^2 + 1 */
#define PPOLY 0x1D 

#define PRIM_POLY 0435
#define NWM1 ((1 << 8)-1)

int gexp(__attribute__((unused)) struct rscode_driver * driver, int z)
{
  int i;
  int pinit,p1,p2,p3,p4,p5,p6,p7,p8;
  int ret = 1;

  pinit = p2 = p3 = p4 = p5 = p6 = p7 = p8 = 0;
  p1 = 1;
  
  if(z!=0) {
    for (i = 1; i <= z; i++) {
      pinit = p8;
      p8 = p7;
      p7 = p6;
      p6 = p5;
      p5 = p4 ^ pinit;
      p4 = p3 ^ pinit;
      p3 = p2 ^ pinit;
      p2 = p1;
      p1 = pinit;
    }
    ret = p1 + (p2<<1) + (p3<<2) + (p4<<3) + (p5<<4) + (p6<<5) + (p7<<6) + (p8<<7);
  }
  return ret;
}

int gmult(__attribute__((unused)) struct rscode_driver * driver, int x, int y)
{
  int prod;
  int i, j, ind;
  int k;
  int scratch[8];
  int w;
  if (x==0 || y == 0) return (0);
  w = 8;

  prod = 0;
  for (i = 0; i < w; i++) {
    scratch[i] = y;
    if (y & (1 << (w-1))) {
      y = y << 1;
      y = (y ^ PRIM_POLY) & NWM1;
    } else {
      y = y << 1;
    }
  }
  for (i = 0; i < w; i++) {
    ind = (1 << i);
    if (ind & x) {
      j = 1;
      for (k = 0; k < w; k++) {
        prod = prod ^ (j & scratch[i]);
        j = (j << 1);
      }
    }
  }
  return prod;
}	

/* This will destroy mat, by the way */

void galois_invert_binary_matrix(int *mat, int *inv)
{
  int rows = 8;
  int cols, i, j;
  int tmp;
 
  cols = rows;

  for (i = 0; i < rows; i++) inv[i] = (1 << i);

  /* First -- convert into upper triangular */

  for (i = 0; i < cols; i++) {

    /* Swap rows if we ave a zero i,i element.  If we can't swap, then the 
       matrix was not invertible */

    if ((mat[i] & (1 << i)) == 0) { 
      for (j = i+1; j < rows && (mat[j] & (1 << i)) == 0; j++) ;
      tmp = mat[i]; mat[i] = mat[j]; mat[j] = tmp;
      tmp = inv[i]; inv[i] = inv[j]; inv[j] = tmp;
    }
 
    /* Now for each j>i, add A_ji*Ai to Aj */
    for (j = i+1; j != rows; j++) {
      if ((mat[j] & (1 << i)) != 0) {
        mat[j] ^= mat[i]; 
        inv[j] ^= inv[i];
      }
    }
  }

  /* Now the matrix is upper triangular.  Start at the top and multiply down */

  for (i = rows-1; i >= 0; i--) {
    for (j = 0; j < i; j++) {
      if (mat[j] & (1 << i)) {
/*        mat[j] ^= mat[i]; */
        inv[j] ^= inv[i];
      }
    }
  } 
}

int galois_shift_inverse(int y)
{
  int w = 8;
  int mat2[8];
  int inv2[8];
  int i;
 
  for (i = 0; i < w; i++) {
    mat2[i] = y;

    if (y & ((1 << 7))/*nw[w-1]*/) {
      y = y << 1;
      y = (y ^ PRIM_POLY) & NWM1/*nwm1[w]*/;
    } else {
      y = y << 1;
    }
  }

  galois_invert_binary_matrix(mat2, inv2);

  return inv2[0]; 
}

int ginv (__attribute__((unused)) struct rscode_driver * driver, int elt) 
{
  return galois_shift_inverse(elt);
}

