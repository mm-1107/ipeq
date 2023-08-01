/*
 * Copyright (c) 2018 XLAB d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <time.h>
#include "cifer/innerprod/fullysec/fh_multi_ipe.h"
#include "cifer/sample/uniform.h"

double getExecutiontime(struct timespec *start_time, struct timespec *end_time){
  unsigned int sec;
  int nsec;
  double d_sec;
  sec = end_time->tv_sec - start_time->tv_sec;
  nsec = end_time->tv_nsec - start_time->tv_nsec;

  d_sec = (double)sec + (double)nsec / (1000 * 1000 * 1000);
  return d_sec;
}

int main(int argc, char const *argv[]) {
    // choose the parameters for the encryption and build the scheme
    size_t sec_level = 80;
    size_t num_clients = 100;
    size_t vec_len = 2;
    mpz_t bound, bound_neg, xy_check, xy;
    mpz_inits(bound, bound_neg, xy_check, xy, NULL);
    mpz_set_ui(bound, 2);
    mpz_pow_ui(bound, bound, 7);
    mpz_neg(bound_neg, bound);
    struct timespec start_time, end_time;
    double d_sec;

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    cfe_fh_multi_ipe fh_multi_ipe;
    cfe_error err= cfe_fh_multi_ipe_init(&fh_multi_ipe, sec_level, num_clients, vec_len, bound, bound);

    // generate master key
    cfe_fh_multi_ipe_sec_key sec_key;
    cfe_fh_multi_ipe_master_key_init(&sec_key, &fh_multi_ipe);
    FP12_BN254 pub_key;
    err = cfe_fh_multi_ipe_generate_keys(&sec_key, &pub_key, &fh_multi_ipe);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    d_sec = getExecutiontime(&start_time, &end_time);
    printf("time:%f\n", d_sec);

    // sample an inner product matrix
    cfe_mat y;
    cfe_mat_init(&y, num_clients, vec_len);
    cfe_uniform_sample_range_mat(&y, bound_neg, bound);

    // derive a functional key for matrix y
    cfe_mat_G2 FE_key;
    cfe_fh_multi_ipe_fe_key_init(&FE_key, &fh_multi_ipe);
    err = cfe_fh_multi_ipe_derive_fe_key(&FE_key, &y, &sec_key, &fh_multi_ipe);

    // we simulate the clients encrypting vectors; each client is given its part
    // of the secret key (B_hat), it samples a random vector and encrypts it
    cfe_fh_multi_ipe clients[num_clients];
    cfe_vec x[num_clients];
    cfe_mat X_for_check;
    cfe_mat_init(&X_for_check, num_clients, vec_len);

    cfe_vec_G1 ciphers[num_clients];

    for (size_t i = 0; i < num_clients; i++) {
        cfe_fh_multi_ipe_copy(&clients[i], &fh_multi_ipe);

        cfe_vec_init(&(x[i]), vec_len);
        cfe_uniform_sample_vec(&x[i], bound);

        cfe_mat_set_vec(&X_for_check, &(x[i]), i);

        cfe_fh_multi_ipe_ciphertext_init(&ciphers[i], &clients[i]);
        err = cfe_fh_multi_ipe_encrypt(&ciphers[i], &x[i], &sec_key.B_hat[i], &clients[i]);
    }


    // simulate a decryptor
    cfe_fh_multi_ipe decryptor;

    // decryptor collects the ciphertexts and decrypts th value of Σ_i <x_i, y_i>
    // (sum of inner products) where x_i is the i-th encrypted vector and y_i the
    // i-th inner product vector (i-th row of y); note that decryptor decrypts using
    // the FE key without knowing vectors x_i or an inner product matrix y
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (size_t i = 0; i < 1; i++) {
      printf("i=%d\n", i);
      cfe_fh_multi_ipe_copy(&decryptor, &fh_multi_ipe);
      err = cfe_fh_multi_ipe_decrypt(xy, ciphers, &FE_key, &pub_key, &decryptor);
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    d_sec = getExecutiontime(&start_time, &end_time);
    printf("time:%f\n", d_sec);
    // check correctness
    cfe_mat_dot(xy_check, &X_for_check, &y);

    // clean up
    mpz_clears(bound, bound_neg, xy_check, xy, NULL);
    for (size_t i = 0; i < num_clients; i++) {
        cfe_vec_free(&x[i]);
        cfe_vec_G1_free(&ciphers[i]);
        cfe_fh_multi_ipe_free(&clients[i]);
    }
    cfe_mat_frees(&X_for_check, &y, NULL);
    cfe_mat_G2_free(&FE_key);
    cfe_fh_multi_ipe_free(&fh_multi_ipe);
    cfe_fh_multi_ipe_free(&decryptor);
    cfe_fh_multi_ipe_master_key_free(&sec_key);

    return 0;
}
