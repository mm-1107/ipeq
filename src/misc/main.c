#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "cifer/innerprod/fullysec/fh_multi_ipe.h"
#include "cifer/sample/uniform.h"

#include "logistic.h"
#define Q 256

struct encparam {
    cfe_vec_G1 w[dim][batch_size];
    cfe_vec_G1 b[batch_size];
};
typedef struct encparam Encparam;

void encrypt_gradient_client(
  Encparam *enc_grads,
  Logistic *grads,
  cfe_fh_multi_ipe *fh_multi_ipe,
  cfe_mat *part_sec_key
);

Logistic decrypt_param_server(
  Logistic *param,
  Encparam *grads,
  cfe_mat_G2 *FE_key,
  FP12_BN254 *pub_key,
  cfe_fh_multi_ipe *decryptor,
  float eta);


int main(int argc, char const *argv[]) {
    // choose the parameters for the encryption and build the scheme
    size_t sec_level = 2;
    size_t vec_len = 2;
    mpz_t bound, bound_neg, xy;
    mpz_inits(bound, bound_neg, xy, NULL);
    mpz_set_ui(bound, 2);
    mpz_pow_ui(bound, bound, 8);
    mpz_neg(bound_neg, bound);

    cfe_fh_multi_ipe fh_multi_ipe;
    cfe_fh_multi_ipe_init(&fh_multi_ipe, sec_level, batch_size, vec_len, bound, bound);

    // generate master key
    cfe_fh_multi_ipe_sec_key sec_key;
    cfe_fh_multi_ipe_master_key_init(&sec_key, &fh_multi_ipe);
    FP12_BN254 pub_key;
    cfe_fh_multi_ipe_generate_keys(&sec_key, &pub_key, &fh_multi_ipe);
    printf("# Generate Key\n");


    int N = 1000;
    double data[N][dim];
    int label[N];
    gen_random_train(N, data, label);
    Logistic param = init_param();
    double eta = 0.1;
    int epoch = 1;
    int iteration = N/batch_size;

    Logistic agg_grad[batch_size];
    double sub_x[batch_size][dim];
    int sub_label[batch_size];

    cfe_mat y;
    cfe_mat_init(&y, batch_size, vec_len);
    mpz_t el;
    mpz_init(el);
    mpz_set_si(el, 1);
    for (size_t i = 0; i < batch_size; i++) {
      // y[i] = [1, 0]
      cfe_mat_set(&y, el, i, 0);
    }
    cfe_mat_G2 FE_key;
    cfe_fh_multi_ipe_fe_key_init(&FE_key, &fh_multi_ipe);
    cfe_fh_multi_ipe_derive_fe_key(&FE_key, &y, &sec_key, &fh_multi_ipe);
    cfe_fh_multi_ipe decryptor;
    cfe_fh_multi_ipe_copy(&decryptor, &fh_multi_ipe);


    Encparam enc_grads;
    printf("## Loss = %f\n", loss(param, data, label, N));
    for (size_t i = 0; i < epoch; i++) {
      for (size_t j = 0; j < iteration; j++) {
        printf("# Epoch %ld, Iteration %ld\n", i, j);
        split_mat(sub_x, data, j*batch_size, batch_size);
        split_array(sub_label, label, j*batch_size, batch_size);
        grad_by_client(agg_grad, param, sub_x, sub_label, batch_size);
        // encrypt gradient
        encrypt_gradient_client(
          &enc_grads,
          agg_grad,
          &fh_multi_ipe,
          sec_key.B_hat);
        // aggregate and decrypt
        param = decrypt_param_server(
          &param,
          &enc_grads,
          &FE_key,
          &pub_key,
          &fh_multi_ipe,
          eta);
        // param = update_param_server(param, agg_grad, eta, batch_size);
        printf("## Loss = %f\n", loss(param, data, label, N));
      }
    }

    // clean up
    mpz_clears(bound, bound_neg, xy, el, NULL);
    cfe_mat_frees(&y, NULL);
    cfe_mat_G2_free(&FE_key);
    cfe_fh_multi_ipe_free(&fh_multi_ipe);
    cfe_fh_multi_ipe_free(&decryptor);
    cfe_fh_multi_ipe_master_key_free(&sec_key);

    return 0;
}


void encrypt_gradient_client(
  Encparam *enc_grads,
  Logistic *grads,
  cfe_fh_multi_ipe *fh_multi_ipe,
  cfe_mat *part_sec_key
)
{
  cfe_vec x;
  mpz_t el;
  mpz_init(el);
  cfe_fh_multi_ipe client;
  for (size_t i = 0; i < dim; i++) {
    printf("# Param %ld\n", i);
    for (size_t j = 0; j < batch_size; j++) {
      cfe_fh_multi_ipe_copy(&client, fh_multi_ipe);
      cfe_vec_init(&x, 2);
      mpz_set_si(el, round(Q*grads[j].w[i]));
      cfe_vec_set(&x, el, 0);
      mpz_set_si(el, 1);
      cfe_vec_set(&x, el, 1);
      cfe_fh_multi_ipe_ciphertext_init(&(enc_grads->w[i][j]), &client);
      cfe_fh_multi_ipe_encrypt(&(enc_grads->w[i][j]), &x, &part_sec_key[j], &client);
    }
  }
  printf("# Param b\n");
  for (size_t j = 0; j < batch_size; j++) {
    cfe_fh_multi_ipe_copy(&client, fh_multi_ipe);
    cfe_vec_init(&x, 2);
    mpz_set_ui(el, round(Q*grads[j].b));
    cfe_vec_set(&x, el, 0);
    mpz_set_si(el, 1);
    cfe_vec_set(&x, el, 1);
    cfe_fh_multi_ipe_ciphertext_init(&(enc_grads->b[j]), &client);
    cfe_fh_multi_ipe_encrypt(&(enc_grads->b[j]), &x, &part_sec_key[j], &client);
  }
  printf("## Encryption done.\n");
  mpz_clears(el, NULL);
  cfe_vec_free(&x);
  cfe_fh_multi_ipe_free(&client);
}


struct pack {
    cfe_vec_G1 *grad;
  	cfe_mat_G2 *fe_key;
    FP12_BN254 *pub_key;
    cfe_fh_multi_ipe *decryptor;
    double *update_param;
};
typedef struct pack Pack;

void *parallel(void *arg_) {
  Pack *arg;
  arg = arg_;
  mpz_t xy;
  mpz_inits(xy, NULL);
  cfe_fh_multi_ipe_decrypt(xy, arg->grad, arg->fe_key, arg->pub_key, arg->decryptor);
  arg->update_param += mpz_get_si(xy);
  gmp_printf("%Zd ",xy);
  mpz_clears(xy, NULL);
}


Logistic decrypt_param_server(
  Logistic *param,
  Encparam *grads,
  cfe_mat_G2 *FE_key,
  FP12_BN254 *pub_key,
  cfe_fh_multi_ipe *fh_multi_ipe,
  float eta)
{
  Logistic update_param;
  mpz_t xy;
  mpz_inits(xy, NULL);
  cfe_fh_multi_ipe decryptor;

  Pack arg;
  pthread_t th[dim];

  for (size_t i = 0; i < dim; i++) {
    /*
    cfe_fh_multi_ipe_copy(&decryptor, fh_multi_ipe);
    cfe_fh_multi_ipe_decrypt(xy, grads->w[i], FE_key, pub_key, &decryptor);
    update_param.w[i] += mpz_get_si(xy);
    gmp_printf("%Zd ",xy);
    */
    printf("## [%d] OK\n", i);
    cfe_fh_multi_ipe_copy(&decryptor, fh_multi_ipe);
  	arg.grad = grads->w[i];
  	arg.fe_key = FE_key;
    arg.pub_key = pub_key;
    arg.decryptor = &decryptor;
    arg.update_param = &update_param.w[i];
    int ret = pthread_create(&th[i], NULL, parallel, (void*)&arg);
  }

  for (size_t i = 0; i < dim; i++) {
    pthread_join(th[i], NULL);
  }

  cfe_fh_multi_ipe_copy(&decryptor, fh_multi_ipe);
  cfe_fh_multi_ipe_decrypt(xy, grads->b, FE_key, pub_key, &decryptor);
  update_param.b += mpz_get_si(xy);
  gmp_printf("%Zd ",xy);
  // update_param.w = update_param.w / batch_size;
  divide_vector(update_param.w, update_param.w, Q*batch_size);
  update_param.b = update_param.b / (Q*batch_size);

  // update_param.w = param.w - eta * update_param.w;
  mult_vector(update_param.w, update_param.w, (-1)*eta);
  sum_vector(update_param.w, param->w, update_param.w);

  update_param.b = param->b - eta * update_param.b;

  mpz_clears(xy, NULL);
  cfe_fh_multi_ipe_free(&decryptor);
  return update_param;
}
