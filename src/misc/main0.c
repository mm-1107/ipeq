#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "cifer/innerprod/fullysec/fh_multi_ipe.h"
#include "cifer/sample/uniform.h"

#include "logistic.h"
#include "adult.h"
#define Q 128

struct encparam {
    cfe_vec_G1 w[dim][batch_size];
    cfe_vec_G1 b[batch_size];
};
typedef struct encparam Encparam;

void update(
  mpz_t *sum,
  double *param,
  cfe_mat_G2 *FE_key,
  cfe_mat *part_sec_key,
  FP12_BN254 *pub_key,
  cfe_fh_multi_ipe *fh_multi_ipe);

void outputCtxt(Encparam *enc_grads);
void readNoise(Logistic *noise_vec);

int main(int argc, char const *argv[]) {
    // choose the parameters for the encryption and build the scheme
    size_t sec_level = 10;
    printf("Security = %d bits\n", sec_level);
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
    // gen_random_train(N, data, label);
    readAdult(data, label, "../../../dataset/adult/adult.data", N);
    Logistic param = init_param();
    double eta = 0.1;
    int epoch = 1;
    int iteration = N/batch_size;

    Logistic agg_grad[batch_size];
    double sub_x[batch_size][dim];
    int sub_label[batch_size];

    Logistic noise;
    cfe_mat y;
    cfe_mat_init(&y, batch_size, vec_len);
    cfe_mat_G2 FE_key;
    mpz_t el;
    mpz_init(el);

    printf("## Loss = %f\n", loss(param, data, label, N));
    mpz_t sum;
    mpz_init(sum);
    Logistic tmp_param;
    double tmp_grad[batch_size];
    for (size_t i = 0; i < epoch; i++) {
      for (size_t j = 0; j < iteration; j++) {
        printf("# Epoch %ld, Iteration %ld\n", i, j);
        split_mat(sub_x, data, j*batch_size, batch_size);
        split_array(sub_label, label, j*batch_size, batch_size);
        grad_by_client(agg_grad, param, sub_x, sub_label, batch_size);
        // Generate ddp noise
        if (system("python3 ../ddp/main.py 10") == -1) {
          printf("Failed to execute the command.\n");
        }
        readNoise(&noise);
        for (size_t j = 0; j < dim; j++) {
          printf("-> Param w[%ld]\n", j);
          printf("--> Set noisy vector...");
          mpz_set_si(el, 1);
          for (size_t i = 0; i < batch_size; i++) {
            // y[i] = [1, 0]
            cfe_mat_set(&y, el, i, 0);
          }
          mpz_set_si(el, noise.w[j]);
          // y[0] = [1, noise]
          cfe_mat_set(&y, el, 0, 1);
          cfe_fh_multi_ipe_fe_key_init(&FE_key, &fh_multi_ipe);
          cfe_fh_multi_ipe_derive_fe_key(&FE_key, &y, &sec_key, &fh_multi_ipe);
          printf("OK.\n");
          for (size_t k = 0; k < batch_size; k++) {
            tmp_grad[k] = agg_grad[k].w[j];
          }
          update(
            &sum,
            tmp_grad,
            &FE_key,
            sec_key.B_hat,
            &pub_key,
            &fh_multi_ipe);
          tmp_param.w[j] += mpz_get_si(sum);
        }
        printf("-> Param b\n");
        printf("--> Noisy vector...");
        mpz_set_si(el, 1);
        for (size_t i = 0; i < batch_size; i++) {
          // y[i] = [1, 0]
          cfe_mat_set(&y, el, i, 0);
        }
        mpz_set_si(el, noise.b);
        // y[0] = [1, noise]
        cfe_mat_set(&y, el, 0, 1);
        cfe_mat_G2 FE_key;
        cfe_fh_multi_ipe_fe_key_init(&FE_key, &fh_multi_ipe);
        cfe_fh_multi_ipe_derive_fe_key(&FE_key, &y, &sec_key, &fh_multi_ipe);
        printf("OK.\n");
        update(
          &sum,
          &agg_grad->b,
          &FE_key,
          sec_key.B_hat,
          &pub_key,
          &fh_multi_ipe);
        tmp_param.b += mpz_get_si(sum);

        // update_param.w = update_param.w / batch_size;
        divide_vector(tmp_param.w, tmp_param.w, Q*batch_size);
        tmp_param.b = tmp_param.b / (Q*batch_size);

        // update_param.w = param.w - eta * update_param.w;
        mult_vector(tmp_param.w, tmp_param.w, (-1)*eta);
        sum_vector(param.w, param.w, tmp_param.w);

        param.b = param.b - eta * tmp_param.b;

        // param = update_param_server(param, agg_grad, eta, batch_size);
        printf("-> Loss = %f\n", loss(param, data, label, N));
      }
    }

    // clean up
    mpz_clears(bound, bound_neg, xy, el, NULL);
    cfe_mat_frees(&y, NULL);
    cfe_mat_G2_free(&FE_key);
    cfe_fh_multi_ipe_free(&fh_multi_ipe);
    cfe_fh_multi_ipe_master_key_free(&sec_key);

    return 0;
}


void readNoise(Logistic *noise_vec){
  FILE *fin = fopen("./noise.txt", "rt");
  if (!fin) {
    perror("fopen");
    exit(1);
  }
  int ret;
  for (size_t i = 0; i < dim; i++) {
    ret = fscanf(fin, "%le\n", &noise_vec->w[i]);
  }
  ret = fscanf(fin, "%le\n", &noise_vec->b);
  // 使い終わったらファイルを閉じる
  fclose(fin);
}


void outputCtxt(Encparam *enc_grads){
  FILE *fpw = fopen("ctxt.dat", "wb");
  if (fpw == NULL) {          // オープンに失敗した場合
   printf("cannot open\n");         // エラーメッセージを出して
   exit(1);                         // 異常終了
  }

  fwrite(&enc_grads, sizeof(enc_grads), 1, fpw);  // ファイルを書き込み用にオープン(開く)

  fclose(fpw);
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
  // gmp_printf("%Zd ",xy);
  mpz_clears(xy, NULL);
}

double getExecutiontime(struct timespec *start_time, struct timespec *end_time){
  unsigned int sec;
  int nsec;
  double d_sec;
  sec = end_time->tv_sec - start_time->tv_sec;
  nsec = end_time->tv_nsec - start_time->tv_nsec;

  d_sec = (double)sec + (double)nsec / (1000 * 1000 * 1000);
  return d_sec;
}

void update(
  mpz_t *sum,
  double *param,
  cfe_mat_G2 *FE_key,
  cfe_mat *part_sec_key,
  FP12_BN254 *pub_key,
  cfe_fh_multi_ipe *fh_multi_ipe)
{
  struct timespec start_time, end_time;
  double d_sec;
  printf("--> Encryption ");
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  cfe_vec x;
  cfe_vec_init(&x, 2);
  mpz_t el;
  mpz_init(el);
  cfe_vec_G1 enc_grads[batch_size];

  cfe_fh_multi_ipe client;
  cfe_fh_multi_ipe_copy(&client, fh_multi_ipe);
  // Encrypt x = [grad[i], Q]
  int i = 0;
  for (i = 0; i < batch_size; i++) {
    mpz_set_si(el, round(Q*param[i]));
    cfe_vec_set(&x, el, 0);
    mpz_set_si(el, Q);
    cfe_vec_set(&x, el, 1);
    cfe_fh_multi_ipe_ciphertext_init(&enc_grads[i], &client);
    cfe_fh_multi_ipe_encrypt(&enc_grads[i], &x, &part_sec_key[i], &client);
  }
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  printf("done.\n");
  d_sec = getExecutiontime(&start_time, &end_time);
  printf("---> Encryption time:%f\n", d_sec);

  // Decrypt
  printf("--> Decryption ");
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  cfe_fh_multi_ipe decryptor;
  cfe_fh_multi_ipe_copy(&decryptor, fh_multi_ipe);
  cfe_fh_multi_ipe_decrypt(*sum, enc_grads, FE_key, pub_key, &decryptor);
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  printf("done.\n");
  d_sec = getExecutiontime(&start_time, &end_time);
  printf("---> Decryption time:%f\n", d_sec);

  // clean up
  cfe_vec_free(&x);
  mpz_clears(el, NULL);
  cfe_fh_multi_ipe_free(&decryptor);
  cfe_fh_multi_ipe_free(&client);
  for (i = 0; i < batch_size; i++) {
    cfe_vec_G1_free(&enc_grads[i]);
  }
}
