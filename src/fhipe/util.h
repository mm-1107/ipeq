#ifndef _util_h
#define _util_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gmp.h>
#include <time.h>
#include "cifer/data/vec.h"
#include "cifer/innerprod/fullysec/fhipe.h"
#include "cifer/sample/uniform.h"

FILE* readFile(char* fname);
void setConstant(cfe_vec *x, int l, int c);
void fillAll(cfe_vec *x, int l, int c);
int getLength(char* fname);
void printVec(char str[], cfe_vec *v);
int getIndex(char **str, char *item, int size);
void encodeDecimal(int base, int *encode, int decimal, int num_bits);
void outputCtxt(cfe_fhipe_ciphertext ctxt);
double getExecutiontime(struct timespec *start_time, struct timespec *end_time);
void minimal_benchmark(int exponent, size_t L);
#endif
