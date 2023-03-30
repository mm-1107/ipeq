#ifndef _tpch_h
#define _tpch_h
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gmp.h>
#include <time.h>
#include <math.h>
#include "cifer/data/vec.h"
#include "cifer/innerprod/fullysec/fhipe.h"
#include "cifer/sample/uniform.h"
#include "util.h"

struct customer {
    int custkey; // base q (2bit)
    char name[20]; // not use
    char address[30]; // not use
    int nationkey; // (5bit)
    char phone[20]; // not use
    float acctbal; // not use
    char mktsegment[20]; // (3bit)
    char comment[100]; // not use
};
typedef struct customer Customer;

struct nation {
    int nationkey;
    char name[20];
    int regionkey;
    char comment[150];
};
typedef struct nation Nation;

struct orders {
    int orderkey; // base q (2 bit)
    int custkey; // base q (2 bit)
    char orderstatus[3]; // (2 bit)
    float totalprice; // not use
    char orderdate[20]; // not use
    char orderpriority[20]; // (3 bit)
    char clerk[20]; // not use
    int shippriority; // not use
    char comment[100]; // not use
};
typedef struct orders Orders;

struct lineitem {
    int orderkey;
    int partkey;
    int suppkey;
    int linenumber;
    int quantity;
    float extendedprice;
    float discount;
    float tax;
    char returnflg[3];
    char linestatus[3];
    char shipdate[20];
    char commitdate[20];
    char receiptdate[20];
    char shipinstruct[20];
    char shipmode[10];
    char comment[50];
};
typedef struct lineitem Lineitem;

struct supplier {
    int suppkey;
    char name[20];
    char address[20];
    int nationkey;
    char phone[20];
    float acctbal;
    char comment[50];
};
typedef struct supplier Supplier;

struct dblen {
  int customer;
  int nation;
  int orders;
  int lineitem;
  int supplier;
};
typedef struct dblen DBlen;

void readCustomer(Customer db[], char* fname, size_t row_num);
void readNation(Nation db[], char* fname, size_t row_num);
void readOrders(Orders db[], char* fname, size_t row_num);
void readLineitem(Lineitem db[], char* fname, size_t row_num);
void readSupplier(Supplier db[], char* fname, size_t row_num);
void encNation(cfe_fhipe_ciphertext *cipher, Nation *n_db, int len, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe);
void encCustomer(cfe_fhipe_ciphertext *cipher, Customer *c_db, int len, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe);
void encCustomerDecimal(cfe_fhipe_ciphertext *cipher, Customer *c_db, int len, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe);
void encOrders(cfe_fhipe_ciphertext *cipher, Orders *o_db, int len, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe);
void encOrdersDecimal(cfe_fhipe_ciphertext *cipher, Orders *o_db, int len, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe);
int getIntersection(cfe_vec *innerprod, int length, mpz_t v);
void Q0(cfe_fhipe_ciphertext *orders, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe, DBlen length, int *encode);
void Q1(cfe_fhipe_ciphertext *orders, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe, DBlen length, int *encode);
void Q2(cfe_fhipe_ciphertext *orders, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe, DBlen length, int *encode, int table);
void Q3(cfe_fhipe_ciphertext *customer, cfe_fhipe_ciphertext *orders, int begin_idx, cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe, DBlen length, int *encode);
#endif
