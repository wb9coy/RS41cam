#ifndef _BERLEKAMP_H
#define _BERLEKAMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ecc.h"

#ifndef TEST
#ifndef RSCODE_INTERNAL
#error "There is no user accessable data and functions here"
#endif
#endif

/* polynomial arithmetic */
void copy_poly(struct rscode_driver * driver, int dst[], int src[]);

void add_polys(struct rscode_driver * driver, int dst[], int src[]) ;
void scale_poly(struct rscode_driver * driver, int k, int poly[]);
void mult_polys(struct rscode_driver * driver, int dst[], int p1[], int p2[]);

void zero_poly(struct rscode_driver * driver, int poly[]);

/* Error location routines */
int correct_errors_erasures (struct rscode_driver * driver, unsigned char codeword[], int csize
#ifndef RSCODE_DISABLE_ERASURES_FUNCTIONS
  ,int nerasures, int erasures[]
#endif
  );

#ifdef __cplusplus
}
#endif

#endif /* _BERLEKAMP_H */
