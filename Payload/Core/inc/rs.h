#ifndef _RS_H
#define _RS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ecc.h"

#ifndef TEST
#ifndef RSCODE_INTERNAL
#error "There is no user accessable data and functions here"
#endif
#endif

/* Reed Solomon encode/decode routines */
void initialize_ecc (struct rscode_driver * driver);
int check_syndrome (struct rscode_driver * driver);
void decode_data (struct rscode_driver * driver, unsigned char *data, int nbytes);
void encode_data (struct rscode_driver * driver, unsigned char *msg, int nbytes, unsigned char *dst);
void zero_fill_from (unsigned char * buf, int from, int to);

#ifdef __cplusplus
}
#endif

#endif /* _RS_H */
