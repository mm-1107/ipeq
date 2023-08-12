#ifndef _adult_h
#define _adult_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

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

void readAdult(double x[][13], int *label, char* fname, size_t row_num);
int writeWorkclass(char x[]);
int writeEducation(char x[]);
int writeMarital_status(char x[]);
int writeOccupation(char x[]);
int writeRelationship(char x[]);
int writeRace(char x[]);
int writeSex(char x[]);
int writeNative_country(char x[]);
int writeIncome_flg(char x[]);
#endif
