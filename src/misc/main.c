#include <stdio.h>
#include <math.h>

#include "cifer/innerprod/fullysec/fh_multi_ipe.h"
#include "cifer/sample/uniform.h"

#include "logistic.h"

int main(int argc, char const *argv[]) {
    // choose the parameters for the encryption and build the scheme
    size_t sec_level = 2;
    size_t num_clients = 10;
    size_t vec_len = 2;
    mpz_t bound, bound_neg, xy_check, xy;
    mpz_inits(bound, bound_neg, xy_check, xy, NULL);
    mpz_set_ui(bound, 2);
    mpz_pow_ui(bound, bound, 10);
    mpz_neg(bound_neg, bound);
    mpz_t el;
    mpz_init(el);

    cfe_fh_multi_ipe fh_multi_ipe;
    cfe_error err= cfe_fh_multi_ipe_init(&fh_multi_ipe, sec_level, num_clients, vec_len, bound, bound);

    // generate master key
    cfe_fh_multi_ipe_sec_key sec_key;
    cfe_fh_multi_ipe_master_key_init(&sec_key, &fh_multi_ipe);
    FP12_BN254 pub_key;
    err = cfe_fh_multi_ipe_generate_keys(&sec_key, &pub_key, &fh_multi_ipe);
    printf("# Generate Key\n");

    // sample an inner product matrix
    cfe_mat y;
    cfe_mat_init(&y, num_clients, vec_len);
    int quantified_mean = round(512 * (1.0 / num_clients));
    printf("quantified_mean: %d\n", quantified_mean);
    mpz_set_ui(el, quantified_mean);
    for (size_t i = 0; i < num_clients; i++) {
      // y = [1, 0]
      cfe_mat_set(&y, el, i, 0);
    }
    // cfe_uniform_sample_range_mat(&y, bound_neg, bound);

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
        printf("# Client %ld\n", i);
        cfe_fh_multi_ipe_copy(&clients[i], &fh_multi_ipe);

        cfe_vec_init(&(x[i]), vec_len);
        // cfe_uniform_sample_vec(&x[i], bound);
        mpz_set_ui(el, 1);
        cfe_vec_set(&x[i], el, 0);

        cfe_mat_set_vec(&X_for_check, &(x[i]), i);

        cfe_fh_multi_ipe_ciphertext_init(&ciphers[i], &clients[i]);
        err = cfe_fh_multi_ipe_encrypt(&ciphers[i], &x[i], &sec_key.B_hat[i], &clients[i]);
    }
    mpz_clears(el, NULL);

    // simulate a decryptor
    cfe_fh_multi_ipe decryptor;
    cfe_fh_multi_ipe_copy(&decryptor, &fh_multi_ipe);

    // decryptor collects the ciphertexts and decrypts th value of Σ_i <x_i, y_i>
    // (sum of inner products) where x_i is the i-th encrypted vector and y_i the
    // i-th inner product vector (i-th row of y); note that decryptor decrypts using
    // the FE key without knowing vectors x_i or an inner product matrix y
    err = cfe_fh_multi_ipe_decrypt(xy, ciphers, &FE_key, &pub_key, &decryptor);
    printf("# Decrypt done.\n");
    // check correctness
    cfe_mat_dot(xy_check, &X_for_check, &y);
    gmp_printf("xy_check = %Zd\n", xy_check);
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
