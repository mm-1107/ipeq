#ifndef _adult_h
#define _adult_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include "cifer/data/vec.h"
#include "cifer/innerprod/fullysec/fhipe.h"
#include "cifer/sample/uniform.h"

struct adult {
    int age;  // continuous
    int workclass;
    int fnlwgt; // continuous
    int education;
    int education_num;  // continuous
    int marital_status;
    int occupation;
    int relationship;
    int race;
    int sex;
    int capital_gain; // continuous
    int capital_loss; // continuous
    int hours_per_week; // continuous
    int native_country;
    int income_flg;
};
typedef struct adult Adult;

void readAdult(Adult db[], char* fname, size_t row_num);
int writeWorkclass(char x[]);
int writeEducation(char x[]);
int writeMarital_status(char x[]);
int writeOccupation(char x[]);
int writeRelationship(char x[]);
int writeRace(char x[]);
int writeSex(char x[]);
int writeNative_country(char x[]);
int writeIncome_flg(char x[]);
void encAdult(cfe_fhipe_ciphertext *cipher, Adult *db, int len,
  cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe);
void runAdult1(cfe_fhipe_ciphertext *db, int length, cfe_fhipe_sec_key *sec_key,
  cfe_fhipe *fhipe);
#endif
