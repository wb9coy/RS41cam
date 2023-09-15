#ifndef _GALOIS_H
#define _GALOIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ecc.h"

#ifndef TEST
#ifndef RSCODE_INTERNAL
#error "There is no user accessable data and functions here"
#endif
#endif

int gmult(struct rscode_driver * driver, int x, int y);
int ginv(struct rscode_driver * driver, int elt);
int gexp(struct rscode_driver * driver, int z);

#ifdef __cplusplus
}
#endif

#endif /* _GALOIS_H */
