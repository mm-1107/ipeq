#include "util.h"

FILE* readFile(char* fname){
  FILE *fin = fopen(fname, "rt");
  if (!fin) {
    perror("fopen");
    exit(1);
  }else {
    printf("\n%s file opened!\n", fname);
	}
  return fin;
}

int getLength(char* fname){
  int line = 0;
  FILE *fin = readFile(fname);
  int c;
  while ((c = fgetc(fin)) != EOF) {
    if (c == '\n') line++;
  }
  fclose(fin);
  printf("%s: %d line\n", fname, line);
  return line;
}

// Set constant in x[idx]
void setConstant(cfe_vec *x, int idx, int c){
  mpz_t el;
  mpz_set_si(el, c);
  cfe_vec_set(x, el, idx);
  // mpz_clears(el);
}

// Fill all elements with constant
void fillAll(cfe_vec *x, int l, int c){
  for (size_t i = 0; i < l; i++) {
    setConstant(x, i, c);
  }
}

void printVec(char str[], cfe_vec *v){
  size_t l = v->size;
  printf("%s[ ", str);
  for (size_t i = 0; i < l; i++) {
    gmp_printf("%Zd ",v->vec[i]);
  }
  printf("]\n");
}

int getIndex(char **str, char item[], int size){
  for (size_t i = 0; i < size; i++) {
    if (strcmp(*str, item) == 0) return i;
    str++;
  }
  printf("%s: Not found\n", item);
  exit(1);
}

void encodeDecimal(int base, int *encode, int decimal, int num_bits){
  for(int i=0; i < num_bits; i++){
    if (decimal >= 0){
      encode[i] = decimal % base;
      decimal = decimal / base;
    }
    else{
      encode[i] = 0;
    }
  }
}

void outputCtxt(cfe_fhipe_ciphertext ctxt){
  FILE *fpw = fopen("ctxt.dat", "wb");
  if (fpw == NULL) {          // オープンに失敗した場合
   printf("cannot open\n");         // エラーメッセージを出して
   exit(1);                         // 異常終了
  }

  fwrite(&ctxt, sizeof(ctxt), 1, fpw);  // ファイルを書き込み用にオープン(開く)

  fclose(fpw);
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

void minimal_benchmark(int exponent, size_t L){
  printf("Minimal benchmark for IPE. \n");
  cfe_vec x, y;
  mpz_t bound, bound_neg, xy;
  mpz_inits(bound, bound_neg, xy, NULL);
  mpz_set_ui(bound, 2);
  mpz_pow_ui(bound, bound, exponent);
  mpz_neg(bound_neg, bound);
  cfe_vec_inits(L, &x, &y, NULL);
  cfe_uniform_sample_range_vec(&x, bound_neg, bound);
  cfe_uniform_sample_range_vec(&y, bound_neg, bound);


  struct timespec start_time, end_time;
  double d_sec;

  clock_gettime(CLOCK_MONOTONIC, &start_time);
  cfe_fhipe fhipe;
  cfe_error err = cfe_fhipe_init(&fhipe, L, bound, bound);
  cfe_fhipe_sec_key sec_key;
  cfe_fhipe_master_key_init(&sec_key, &fhipe);
  err = cfe_fhipe_generate_master_key(&sec_key, &fhipe);
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  d_sec = getExecutiontime(&start_time, &end_time);
  printf("IPE.Setup() time:%f\n", d_sec);

  // derive a functional key for vector y
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  cfe_fhipe_fe_key FE_key;
  cfe_fhipe_fe_key_init(&FE_key, &fhipe);
  err = cfe_fhipe_derive_fe_key(&FE_key, &y, &sec_key, &fhipe);
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  d_sec = getExecutiontime(&start_time, &end_time);
  printf("IPE.Keygen() time:%f\n", d_sec);

  // encrypt the vector
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  cfe_fhipe_ciphertext cipher;
  cfe_fhipe_ciphertext_init(&cipher, &fhipe);
  err = cfe_fhipe_encrypt(&cipher, &x, &sec_key, &fhipe);
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  d_sec = getExecutiontime(&start_time, &end_time);
  printf("IPE.Encrypt() time:%f\n", d_sec);

  // simulate a decryptor
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  cfe_fhipe decryptor;
  cfe_fhipe_copy(&decryptor, &fhipe);
  // decryptor decrypts the inner-product without knowing
  // vectors x and y
  err = cfe_fhipe_decrypt(xy, &cipher, &FE_key, &decryptor);
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  d_sec = getExecutiontime(&start_time, &end_time);
  printf("IPE.Decrypt() time:%f\n", d_sec);

  // clean up
  mpz_clears(bound, bound_neg, xy, NULL);
  cfe_vec_frees(&x, &y, NULL);
  cfe_fhipe_free(&fhipe);
  cfe_fhipe_free(&decryptor);
  cfe_fhipe_master_key_free(&sec_key);
  cfe_fhipe_fe_key_free(&FE_key);
  cfe_fhipe_ciphertext_free(&cipher);
}
