#include <stdio.h>
#include <gmp.h>
#include "tpch.h"
#include "util.h"
#include "cifer/internal/big.h"
#include "cifer/data/vec.h"
#include "cifer/innerprod/fullysec/fhipe.h"
#include "cifer/sample/uniform.h"
// https://eprint.iacr.org/2015/608.pdf

int main(int argc, char *argv[]) {
    // argument: scale, query, encode
    int encode;
    size_t L;
    if (argc < 4){
      encode = 1;
      L = 11;
    } else if (strcmp(argv[3], "encode=false") == 0) {
      encode = 0;
      L = 5;
    }
    /*
    int length_list[4] = {2, 4, 8, 16};
    int exponent_list[4] = {6, 8, 10, 12};
    for (size_t i = 0; i < 4; i++) {
      for (size_t j = 0; j < 4; j++) {
        printf("bound = 2^%d, L = %d\n", exponent_list[i], length_list[j]);
        minimal_benchmark(exponent_list[i], length_list[j]);
      }
    }*/

    DBlen length;
    char path[50];
    sprintf(path, "%s%s%s", "../../../dataset/TPCH/scale_", argv[1], "/customer.csv");
    length.customer = getLength(path);
    // length.customer = 100;
    Customer *c_db = malloc(sizeof(Customer)*length.customer);
    readCustomer(c_db, path, length.customer);

    sprintf(path, "%s%s%s", "../../../dataset/TPCH/scale_", argv[1], "/orders.csv");
    length.orders = getLength(path);
    // length.orders = 200;
    Orders *o_db = malloc(sizeof(Orders)*length.orders);
    readOrders(o_db, path, length.orders);

    printf("choose the parameters for the encryption and build the scheme\n");
    mpz_t bound;
    mpz_inits(bound, NULL);
    mpz_set_ui(bound, 2);
    mpz_pow_ui(bound, bound, 8);

    mpz_t check, order;
    mpz_inits(check, order, NULL);
    mpz_from_BIG_256_56(order, (int64_t *) CURVE_Order_BN254);

    mpz_mul(check, bound, bound);
    mpz_mul_ui(check, check, L);

    if (mpz_cmp(check, order) >= 0) {
        printf("[err] mpz_cmp(check, order)>=0\n");
    }
    else{
      printf("mpz_cmp(check, order)<0\n");
    }
    cfe_fhipe fhipe;
    cfe_error err = cfe_fhipe_init(&fhipe, L, bound, bound);
    // munit_assert(err == 0);

    printf("generate master key\n");
    cfe_fhipe_sec_key sec_key;
    cfe_fhipe_master_key_init(&sec_key, &fhipe);
    err = cfe_fhipe_generate_master_key(&sec_key, &fhipe);
    //
    printf("Start enctryption of customer...");
    cfe_fhipe_ciphertext *encC = (cfe_fhipe_ciphertext*)malloc(sizeof(cfe_fhipe_ciphertext)*length.customer);
    if (encode) {
      encCustomer(encC, c_db, length.customer, &sec_key, &fhipe);
    } else {
      encCustomerDecimal(encC, c_db, length.customer, &sec_key, &fhipe);
    }
    printf("OK !\n");
    free(c_db);

    printf("Start enctryption of orders...");
    cfe_fhipe_ciphertext *encO = (cfe_fhipe_ciphertext*)malloc(sizeof(cfe_fhipe_ciphertext)*length.orders);
    if (encode) {
      encOrders(encO, o_db, length.orders, &sec_key, &fhipe);
    } else {
      encOrdersDecimal(encO, o_db, length.orders, &sec_key, &fhipe);
    }
    printf("OK !\n");
    free(o_db);
    outputCtxt(encO[0]);

    printf("%d\n", encode);
    if (strcmp(argv[2], "q0") == 0) {
      Q0(encO, &sec_key, &fhipe, length, &encode);
    } else if (strcmp(argv[2], "q1") == 0) {
      Q1(encO, &sec_key, &fhipe, length, &encode);
    } else if (strcmp(argv[2], "q2") == 0) {
      Q2(encO, &sec_key, &fhipe, length, &encode, 1);
    } else if (strcmp(argv[2], "q3") == 0) {
      Q3(encC, encO, 1, &sec_key, &fhipe, length, &encode);
    } else {
      printf("%s is not supported !\n", argv[2]);
    }
    mpz_clears(bound, NULL);
    cfe_fhipe_free(&fhipe);
    cfe_fhipe_master_key_free(&sec_key);
    cfe_fhipe_ciphertext_free(encC);
    cfe_fhipe_ciphertext_free(encO);
    return 0;
}
