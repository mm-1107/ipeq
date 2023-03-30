#include "tpch.h"
// #define DEBUG_ON
const int bit_id = 2;
int uq_nationkey = 25;
const int bit_nationkey = 5;
char *uq_mktsegment[5] = {"BUILDING", "AUTOMOBILE", "MACHINERY", "HOUSEHOLD", "FURNITURE"};
const int bit_mktsegment = 3;
char *uq_orderstatus[3] = {"O", "F", "P"};
const int bit_orderstatus = 2;
char *uq_orderpriority[5] = {"5-LOW", "1-URGENT", "4-NOT SPECIFIED", "2-HIGH", "3-MEDIUM"};
const int bit_orderpriority = 3;

void printCustomer(Customer r){
  #ifdef DEBUG_ON
  printf("---------------------\n");
  printf("custkey[%d] name[%s] address[%s] nationkey[%d] phone[%s] acctbal[%f] \
    mktsegment[%s] comment[%s]\n",
    r.custkey, r.name, r.address, r.nationkey, r.phone, r.acctbal,
    r.mktsegment, r.comment);
  #endif
}

void printNation(Nation r){
  #ifdef DEBUG_ON
  printf("---------------------\n");
  printf("nationkey[%d] name[%s] regionkey[%d] nationkey[%d] comment[%s]\n",
  r.nationkey, r.name, r.regionkey, r.nationkey, r.comment);
  #endif
}

void printOrders(Orders r){
  #ifdef DEBUG_ON
  printf("---------------------\n");
  printf("orderkey[%d] custkey[%d] orderstatus[%s] totalprice[%f] \
    orderdate[%s] orderpriority[%s] clerk[%s] shippriority[%d] comment[%s]\n",
  r.orderkey, r.custkey, r.orderstatus, r.totalprice, r.orderdate,
  r.orderpriority, r.clerk, r.shippriority, r.comment);
  #endif
}

void printLineitem(Lineitem r){
  #ifdef DEBUG_ON
  printf("---------------------\n");
  printf("orderkey[%d] partkey[%d] suppkey[%d] linenumber[%d] quantity[%d] \
    extendedprice[%f] discount[%f] tax[%f] returnflg[%s] linestatus[%s] \
    shipdate[%s] commitdate[%s] receiptdate[%s] shipinstruct[%s] shipmode[%s] \
    comment[%s]\n",
  r.orderkey, r.partkey, r.suppkey, r.linenumber, r.quantity, r.extendedprice,
  r.discount, r.tax, r.returnflg, r.linestatus, r.shipdate, r.commitdate,
  r.receiptdate, r.shipinstruct, r.shipmode, r.comment);
  #endif
}

void printSupplier(Supplier r){
  #ifdef DEBUG_ON
  printf("---------------------\n");
  printf("suppkey[%d] name[%s] address[%s] nationkey[%d] phone[%s] \
    acctbal[%f] comment[%s]\n",
    r.suppkey, r.name, r.address, r.nationkey, r.phone, r.acctbal, r.comment);
  #endif
}

void readCustomer(Customer db[], char* fname, size_t row_num){
    int i = 0;
    FILE *fin = readFile(fname);
    for (; fscanf(fin, "%d|%[^|]|%[^|]|%d|%[^|]|%f|%[^|]|%[^\n]%*c", &db[i].custkey,
      db[i].name, db[i].address, &db[i].nationkey, db[i].phone, &db[i].acctbal,
      db[i].mktsegment, db[i].comment) != EOF; ) {
      printCustomer(db[i]);
      i++;
      if (i == row_num) {
        break;
      }
    }
    // 使い終わったらファイルを閉じる
    fclose(fin);
}

void readNation(Nation db[], char* fname, size_t row_num){
    int i = 0;
    FILE *fin = readFile(fname);
    for (; fscanf(fin, "%d|%[^|]|%d|%[^\n]%*c", &db[i].nationkey,
      db[i].name, &db[i].regionkey, db[i].comment) != EOF; ) {
      printNation(db[i]);
      i++;
      if (i == row_num) {
        break;
      }
    }
    // 使い終わったらファイルを閉じる
    fclose(fin);
}

void readOrders(Orders db[], char* fname, size_t row_num){
    int i = 0;
    FILE *fin = readFile(fname);
    for (; fscanf(fin, "%d|%d|%[^|]|%f|%[^|]|%[^|]|%[^|]|%d|%[^\n]%*c",
      &db[i].orderkey, &db[i].custkey, db[i].orderstatus, &db[i].totalprice,
      db[i].orderdate, db[i].orderpriority, db[i].clerk, &db[i].shippriority, db[i].comment) != EOF; ) {
      printOrders(db[i]);
      i++;
      if (i == row_num) {
        break;
      }
    }
    // 使い終わったらファイルを閉じる
    fclose(fin);
}

void readLineitem(Lineitem db[], char* fname, size_t row_num){
    int i = 0;
    FILE *fin = readFile(fname);
    for (; fscanf(fin, "%d|%d|%d|%d|%d|%f|%f|%f|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]%*c",
      &db[i].orderkey, &db[i].partkey, &db[i].suppkey, &db[i].linenumber,
      &db[i].quantity, &db[i].extendedprice, &db[i].discount, &db[i].tax,
      db[i].returnflg, db[i].linestatus, db[i].shipdate, db[i].commitdate,
      db[i].receiptdate, db[i].shipinstruct, db[i].shipmode, db[i].comment) != EOF; ) {
      printLineitem(db[i]);
      i++;
      if (i == row_num) {
        break;
      }
    }
    // 使い終わったらファイルを閉じる
    fclose(fin);
}

void readSupplier(Supplier db[], char* fname, size_t row_num){
    int i = 0;
    FILE *fin = readFile(fname);
    for (; fscanf(fin, "%d|%[^|]|%[^|]|%d|%[^|]|%f|%[^\n]%*c",
       &db[i].suppkey, db[i].name, db[i].address, &db[i].nationkey, db[i].phone,
       &db[i].acctbal, db[i].comment) != EOF; ) {
      printSupplier(db[i]);
      i++;
      if (i == row_num) {
        break;
      }
    }
    fclose(fin);
}

void encCustomerDecimal(cfe_fhipe_ciphertext *cipher, Customer *c_db, int len,
  cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe){
  mpz_t el;
  cfe_vec x;
  size_t L = fhipe->l;
  cfe_vec_inits(L, &x, NULL);
  fillAll(&x, L, 0);
  mpz_set_ui(el, 1);
  cfe_vec_set(&x, el, 0);
  for (size_t i = 0; i < len; i++) {
    cfe_fhipe_ciphertext_init(&cipher[i], fhipe);
    mpz_set_ui(el, c_db[i].custkey);
    mpz_cdiv_r(el, el, fhipe->bound_x+1);
    cfe_vec_set(&x, el, 1);
    mpz_set_ui(el, c_db[i].nationkey+1);
    cfe_vec_set(&x, el, 2);
    int decimal = getIndex(uq_mktsegment, c_db[i].mktsegment, 5);
    mpz_set_ui(el, decimal);
    cfe_vec_set(&x, el, 3);
    cfe_fhipe_encrypt(&cipher[i], &x, sec_key, fhipe);
  }
  printVec("\n-> Example record: ", &x);
  mpz_clears(el, NULL);
  cfe_vec_frees(&x, NULL);
}

void encOrdersDecimal(cfe_fhipe_ciphertext *cipher, Orders *o_db, int len,
  cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe){
  mpz_t el;
  cfe_vec x;
  size_t L = fhipe->l;
  cfe_vec_inits(L, &x, NULL);
  fillAll(&x, L, 0);
  mpz_set_ui(el, 1);
  cfe_vec_set(&x, el, 0);
  int decimal;
  // orderkey, custkey, orderstatus, orderpriority
  for (size_t i = 0; i < len; i++) {
    cfe_fhipe_ciphertext_init(&cipher[i], fhipe);
    mpz_set_ui(el, o_db[i].orderkey);
    // el = orderkey mod bound+1
    mpz_cdiv_r(el, el, fhipe->bound_x+1);
    cfe_vec_set(&x, el, 1);

    mpz_set_ui(el, o_db[i].custkey);
    mpz_cdiv_r(el, el, fhipe->bound_x+1);
    cfe_vec_set(&x, el, 2);

    decimal = getIndex(uq_orderstatus, o_db[i].orderstatus, 3) + 1;
    mpz_set_ui(el, decimal);
    cfe_vec_set(&x, el, 3);

    decimal = getIndex(uq_orderpriority, o_db[i].orderpriority, 5) + 1;
    mpz_set_ui(el, decimal);
    cfe_vec_set(&x, el, 4);
    cfe_fhipe_encrypt(&cipher[i], &x, sec_key, fhipe);
  }
  printVec("\n-> Example record: ", &x);
  mpz_clears(el, NULL);
  cfe_vec_frees(&x, NULL);
}

void encCustomer(cfe_fhipe_ciphertext *cipher, Customer *c_db, int len,
  cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe){
    mpz_t el;
    mpz_init(el);
    size_t L = fhipe->l;
    cfe_vec x;
    cfe_vec_inits(L, &x, NULL);
    fillAll(&x, L, 0);
    mpz_set_ui(el, 1);
    cfe_vec_set(&x, el, 0);
    int idx;
    int encode[bit_nationkey];
    int encode_id[bit_id];
    int encode_mktsegment[bit_mktsegment];
    cfe_fhipe fhipe_copy;
    int base_for_id = mpz_get_si(fhipe->bound_x) - 1;
    // x = [1, custkey, nationkey, mktsegment]
    for (size_t i = 0; i < len; i++) {
      cfe_fhipe_copy(&fhipe_copy, fhipe);
      cfe_fhipe_ciphertext_init(&cipher[i], &fhipe_copy);

      idx = 1;
      encodeDecimal(base_for_id, encode_id, c_db[i].custkey, bit_id);
      for (size_t j = 0; j < bit_id; j++) {
        mpz_set_ui(el, encode_id[j]);
        cfe_vec_set(&x, el, idx+j);
      }

      idx += bit_id;
      encodeDecimal(2, encode, c_db[i].nationkey+1, bit_nationkey);
      for (size_t j = 0; j < bit_nationkey; j++) {
        mpz_set_ui(el, encode[j]);
        cfe_vec_set(&x, el, idx+j);
      }

      idx += bit_nationkey;
      int decimal = getIndex(uq_mktsegment, c_db[i].mktsegment, 5);
      encodeDecimal(2, encode_mktsegment, decimal+1, bit_mktsegment);
      for (size_t j = 0; j < bit_mktsegment; j++) {
        mpz_set_ui(el, encode_mktsegment[j]);
        cfe_vec_set(&x, el, idx+j);
      }
      // printVec("query: ", &x);
      cfe_fhipe_encrypt(&cipher[i], &x, sec_key, &fhipe_copy);
    }
    printVec("\n-> Example record: ", &x);
    mpz_clears(el, NULL);
    cfe_vec_frees(&x, NULL);
    cfe_fhipe_free(&fhipe_copy);
}

void encOrders(cfe_fhipe_ciphertext *cipher, Orders *o_db, int len,
  cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe){
    mpz_t el;
    mpz_init(el);
    size_t L = fhipe->l;
    cfe_vec x;
    cfe_vec_inits(L, &x, NULL);
    fillAll(&x, L, 0);
    mpz_set_si(el, 1);
    cfe_vec_set(&x, el, 0);
    int idx;
    int encode_orderstatus[bit_orderstatus];
    int encode_id[bit_id];
    int encode_orderpriority[bit_orderpriority];
    cfe_fhipe fhipe_copy;
    int base_for_id = mpz_get_si(fhipe->bound_x) - 1;
    // x = [1, orderkey, custkey, orderstatus, orderpriority]
    for (size_t i = 0; i < len; i++) {
      cfe_fhipe_copy(&fhipe_copy, fhipe);
      cfe_fhipe_ciphertext_init(&cipher[i], &fhipe_copy);
      idx = 1;
      // encode_id[1] = 0;
      encodeDecimal(base_for_id, encode_id, o_db[i].orderkey, bit_id);
      for (size_t j = 0; j < bit_id; j++) {
        mpz_set_si(el, encode_id[j]);
        cfe_vec_set(&x, el, idx+j);
      }

      idx += bit_id;
      encodeDecimal(base_for_id, encode_id, o_db[i].custkey+1, bit_id);
      for (size_t j = 0; j < bit_id; j++) {
        mpz_set_si(el, encode_id[j]);
        cfe_vec_set(&x, el, idx+j);
      }

      idx += bit_id;
      int decimal = getIndex(uq_orderstatus, o_db[i].orderstatus, 3);
      encodeDecimal(2, encode_orderstatus, decimal+1, bit_orderstatus);
      for (size_t j = 0; j < bit_orderstatus; j++) {
        mpz_set_si(el, encode_orderstatus[j]);
        cfe_vec_set(&x, el, idx+j);
      }

      idx += bit_orderstatus;
      decimal = getIndex(uq_orderpriority, o_db[i].orderpriority, 5);
      encodeDecimal(2, encode_orderpriority, decimal+1, bit_orderpriority);
      for (size_t j = 0; j < bit_orderpriority; j++) {
        mpz_set_si(el, encode_orderpriority[j]);
        cfe_vec_set(&x, el, idx+j);
      }
      cfe_fhipe_encrypt(&cipher[i], &x, sec_key, &fhipe_copy);
    }
    printVec("\n-> Example record: ", &x);
    mpz_clears(el, NULL);
    cfe_vec_frees(&x, NULL);
    cfe_fhipe_free(&fhipe_copy);
}


void Q0(cfe_fhipe_ciphertext *orders, cfe_fhipe_sec_key *sec_key,
  cfe_fhipe *fhipe, DBlen length, int *encode){
  /* SELECT count(*) FROM orders
    WHERE orderstatus = "O"*/
    cfe_fhipe_fe_key FE_key;
    cfe_fhipe_fe_key_init(&FE_key, fhipe);
    mpz_t el, zero, bound_neg;
    mpz_inits(el, zero, bound_neg, NULL);
    mpz_neg(bound_neg, fhipe->bound_x);
    size_t L = fhipe->l;
    // query vector
    cfe_vec query;
    cfe_vec_inits(L, &query, NULL);
    if ((*encode) == 1) {
      int idx = 1 + bit_id + bit_id;
      int l1_norm = 0;
      int encode_orderstatus[bit_orderstatus];
      int decimal = getIndex(uq_orderstatus, "O", 3);
      encodeDecimal(2, encode_orderstatus, decimal+1, bit_orderstatus);
      for (size_t j = 0; j < bit_orderstatus; j++) {
        if (encode_orderstatus[j] == 1){
          mpz_set_si(el, -1);
          l1_norm += 1;
        }
        else{
          mpz_set_si(el, 1);
        }
        cfe_vec_set(&query, el, idx+j);
      }
      mpz_set_si(el, l1_norm);
      cfe_vec_set(&query, el, 0);
    } else { // Not encode ver.
      fillAll(&query, L, 0);
      SAMPLE:
      mpz_set_si(el, 1);
      // cfe_uniform_sample_range(el, bound_neg, fhipe->bound_x);
      cfe_vec_set(&query, el, 3);
      int decimal = getIndex(uq_orderstatus, "O", 3) + 1;
      mpz_mul_ui(el, el, decimal);
      mpz_neg(el, el);
      if ((mpz_cmp(el, fhipe->bound_x) > 0) || (mpz_cmp(el, bound_neg) < 0)) {
        goto SAMPLE;
      }
      else {
        cfe_vec_set(&query, el, 0);
      }
    }
    printVec("query: ", &query);

    // Generate FE_key
    cfe_fhipe_derive_fe_key(&FE_key, &query, sec_key, fhipe);
    int result = 0;
    mpz_set_si(zero, 0);
    struct timespec start_time, end_time;
    double d_sec;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (size_t i = 0; i < length.orders; i++) {
      cfe_fhipe_decrypt(el, &orders[i], &FE_key, fhipe);
      if(i%500 == 0) gmp_printf("#%d The inner product %Zd\n", i, el);
      if (mpz_cmp(el, zero) == 0) result++;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    d_sec = getExecutiontime(&start_time, &end_time);
    printf("time:%f\n", d_sec);
    printf("Result %d\n", result);
    if (length.orders == 15000) printf("Correct 7333\n");
    if (length.orders == 75000) printf("Correct 36574\n");
    if (length.orders == 150000) printf("Correct 73267\n");
    mpz_clears(el, zero, bound_neg, NULL);
    cfe_vec_frees(&query, NULL);
    cfe_fhipe_fe_key_free(&FE_key);
}

void Q1(cfe_fhipe_ciphertext *orders, cfe_fhipe_sec_key *sec_key,
  cfe_fhipe *fhipe, DBlen length, int *encode){
  /* SELECT count(*) FROM orders
    WHERE orderstatus = "O" AND orderpriority = "1-URGENT"*/
    cfe_fhipe_fe_key FE_key;
    cfe_fhipe_fe_key_init(&FE_key, fhipe);
    size_t L = fhipe->l;
    mpz_t el, el_, tmp, bound_neg;
    mpz_inits(el, el_, tmp, bound_neg, NULL);
    mpz_neg(bound_neg, fhipe->bound_x);
    // query vector
    cfe_vec query;
    cfe_vec_inits(L, &query, NULL);
    fillAll(&query, L, 0);
    if (*encode == 1) {
      int idx = 1 + bit_id + bit_id;
      int l1_norm = 0;
      int encode_orderstatus[bit_orderstatus];
      int decimal = getIndex(uq_orderstatus, "O", 3);
      encodeDecimal(2, encode_orderstatus, decimal+1, bit_orderstatus);
      for (size_t j = 0; j < bit_orderstatus; j++) {
        if (encode_orderstatus[j] == 1){
          mpz_set_si(el, -1);
          l1_norm += 1;
        }
        else{
          mpz_set_si(el, 1);
        }
        cfe_vec_set(&query, el, idx+j);
      }
      idx += bit_orderstatus;
      int encode_orderpriority[bit_orderpriority];
      decimal = getIndex(uq_orderpriority, "1-URGENT", 5);
      encodeDecimal(2, encode_orderpriority, decimal+1, bit_orderpriority);
      for (size_t j = 0; j < bit_orderpriority; j++) {
        if (encode_orderpriority[j] == 1) {
          mpz_set_si(el, -1);
          l1_norm += 1;
        }
        else{
          mpz_set_si(el, 1);
        }
        cfe_vec_set(&query, el, idx+j);
      }
      mpz_set_si(el, l1_norm);
      cfe_vec_set(&query, el, 0);
    } else { // Not encode ver.
      SAMPLE:
      mpz_set_si(el, 1);
      // cfe_uniform_sample_range(el, el_, fhipe->bound_x);
      cfe_vec_set(&query, el, 3);
      int decimal = getIndex(uq_orderstatus, "O", 3) + 1;
      mpz_mul_ui(tmp, el, decimal);
      mpz_neg(tmp, tmp);

      // cfe_uniform_sample_range(el, el_, fhipe->bound_x);
      mpz_set_si(el, 1);
      cfe_vec_set(&query, el, 4);
      decimal = getIndex(uq_orderpriority, "1-URGENT", 5) + 1;
      mpz_mul_ui(el, el, decimal);
      mpz_neg(el, el);
      mpz_add(el, el, tmp);
      if ((mpz_cmp(el, fhipe->bound_x) > 0) || (mpz_cmp(el, bound_neg) < 0)) {
        goto SAMPLE;
      }
      else {
        cfe_vec_set(&query, el, 0);
      }
    }
    printVec("query: ", &query);

    // Generate FE_key
    cfe_fhipe_derive_fe_key(&FE_key, &query, sec_key, fhipe);
    int result = 0;
    mpz_set_si(el_, 0);
    struct timespec start_time, end_time;
    double d_sec;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (size_t i = 0; i < length.orders; i++) {
      cfe_fhipe_decrypt(el, &orders[i], &FE_key, fhipe);
      if(i%500 == 0) gmp_printf("#%d The inner product %Zd\n", i, el);
      if (mpz_cmp(el, el_) == 0) result++;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    d_sec = getExecutiontime(&start_time, &end_time);
    printf("time:%f\n", d_sec);
    printf("Result %d\n", result);
    if (length.orders == 15000) printf("Correct 1488\n");
    if (length.orders == 75000) printf("Correct 7300\n");
    if (length.orders == 150000) printf("Correct 14697\n");
    mpz_clears(el, el_, tmp, NULL);
    cfe_vec_frees(&query, NULL);
    cfe_fhipe_fe_key_free(&FE_key);
}

void Q2(cfe_fhipe_ciphertext *db, cfe_fhipe_sec_key *sec_key,
  cfe_fhipe *fhipe, DBlen length, int *encode, int table){
  /* SELECT orderstatus, orderpriority, count(orderstatus) FROM orders
    GROUP BY orderstatus, orderpriority */
    cfe_fhipe_fe_key FE_key;
    cfe_fhipe_fe_key_init(&FE_key, fhipe);
    size_t L = fhipe->l;
    mpz_t el, el_;
    mpz_inits(el, el_, NULL);
    int groups;
    // query vector
    cfe_vec query;
    cfe_vec_inits(L, &query, NULL);
    int len;
    if (table == 1) {
        // orders
        len = length.orders;
        printf("\nSELECT orderstatus, orderpriority, count(orderstatus)"
         "FROM orders GROUP BY orderstatus, orderpriority\n");
        if (*encode == 1) {
          int idx = 1 + bit_id + bit_id;
          for (size_t j = 0; j < bit_orderstatus; j++) {
            mpz_set_si(el, pow(2, j));
            cfe_vec_set(&query, el, idx+j);
          }
          idx += bit_orderstatus;
          for (size_t j = 0; j < bit_orderpriority; j++) {
              mpz_set_si(el, pow(2, bit_orderstatus+j));
              cfe_vec_set(&query, el, idx+j);
          }
          groups = pow(2, bit_orderstatus+bit_orderpriority);
        } else {
          mpz_set_si(el, 1);
          // cfe_uniform_sample_range(el, el_, fhipe->bound_x);
          cfe_vec_set(&query, el, 3);
          // cfe_uniform_sample_range(el, el_, fhipe->bound_x);
          cfe_vec_set(&query, el, 4);
          groups = mpz_get_si(fhipe->bound_x)*mpz_get_si(fhipe->bound_x)*2;
        }
      } else if (table == 2){
        // customer
        len = length.customer;
        printf("\nSELECT nationkey, mktsegment, count(nationkey)"
         "FROM customer GROUP BY nationkey, mktsegment\n");
        if (*encode == 1) {
          int idx = 1 + bit_id;
          for (size_t j = 0; j < bit_nationkey; j++) {
            mpz_set_si(el, pow(2, j));
            cfe_vec_set(&query, el, idx+j);
          }
          idx += bit_nationkey;
          for (size_t j = 0; j < bit_mktsegment; j++) {
              mpz_set_si(el, pow(2, bit_nationkey+j));
              cfe_vec_set(&query, el, idx+j);
          }
          groups = pow(2, bit_nationkey+bit_mktsegment);
        } else {
          mpz_set_si(el_, 1);
          // cfe_uniform_sample_range(el, el_, fhipe->bound_x);
          cfe_vec_set(&query, el_, 2);
          // cfe_uniform_sample_range(el, el_, fhipe->bound_x);
          cfe_vec_set(&query, el_, 3);
          groups = mpz_get_si(fhipe->bound_x)*mpz_get_si(fhipe->bound_x)*2;
        }
    }
    printVec("query: ", &query);

    // Generate FE_key
    cfe_fhipe_derive_fe_key(&FE_key, &query, sec_key, fhipe);
    int *result = (int*)malloc(sizeof(int)*groups);
    for (size_t j = 0; j < groups; j++) {
      result[j] = 0;
    }
    struct timespec start_time, end_time;
    double d_sec;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (size_t i = 0; i < len; i++) {
      cfe_fhipe_decrypt(el, &db[i], &FE_key, fhipe);
      if(i%500 == 0) gmp_printf("#%d The inner product %Zd\n", i, el);
      result[mpz_get_si(el)] += 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    d_sec = getExecutiontime(&start_time, &end_time);
    printf("time:%f\n", d_sec);
    int cnt_group = 0;
    printf("Result = {");
    for (size_t j = 0; j < groups; j++) {
      if (result[j] > 0) {
        cnt_group += 1;
        printf("%ld: %d, ", j, result[j]);
      }
    }
    printf("}\n");
    printf("cnt_group = %d\n", cnt_group);
    mpz_clears(el, el_, NULL);
    cfe_vec_frees(&query, NULL);
    cfe_fhipe_fe_key_free(&FE_key);
    free(result);
}

void Q3(cfe_fhipe_ciphertext *customer, cfe_fhipe_ciphertext *orders, int begin_idx,
    cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe, DBlen length, int *encode){
  /* SELECT count(*) FROM orders
    JOIN customer ON orders.custkey =  customer.custkey */
    mpz_t el, el_;
    mpz_inits(el, el_, NULL);
    size_t L = fhipe->l;
    // query vector
    cfe_vec query_c, query_o;
    cfe_vec_inits(L, &query_c, &query_o, NULL);
    // fillAll(&query, L, 0);
    if (*encode == 1) {
      int idx = begin_idx;
      int base = 1;
      for (size_t j = 0; j < L; j++) {
        if (idx <= j && j < idx+bit_id) {
          mpz_set_si(el, base);
          base = base * 256;
        }
        else{
          mpz_set_si(el, 0);
        }
        cfe_vec_set(&query_c, el, j);
      }

      idx += bit_id;
      base = 1;
      for (size_t j = 0; j < L; j++) {
        if (idx <= j && j < idx+bit_id) {
          mpz_set_si(el, base);
          base = base * 256;
        }
        else{
          mpz_set_si(el, 0);
        }
        cfe_vec_set(&query_o, el, j);
      }
    } else {
      fillAll(&query_c, L, 0);
      fillAll(&query_o, L, 0);
      mpz_set_si(el_, 1);
      cfe_uniform_sample_range(el, el_, fhipe->bound_x);
      cfe_vec_set(&query_c, el, 1);
      cfe_vec_set(&query_o, el, 2);
    }
    printVec("query: ", &query_c);
    printVec("query: ", &query_o);
    // Generate FE_key
    cfe_fhipe_fe_key FE_key_c, FE_key_o;
    cfe_fhipe_fe_key_init(&FE_key_c, fhipe);
    cfe_fhipe_derive_fe_key(&FE_key_c, &query_c, sec_key, fhipe);
    cfe_fhipe_fe_key_init(&FE_key_o, fhipe);
    cfe_fhipe_derive_fe_key(&FE_key_o, &query_o, sec_key, fhipe);
    printf("cfe_fhipe_derive_fe_key OK!\n");
    cfe_vec c_ip;
    cfe_vec_inits(length.customer, &c_ip, NULL);
    struct timespec start_time, end_time;
    double d_sec;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (size_t i = 0; i < length.customer; i++) {
      cfe_fhipe_decrypt(el, &customer[i], &FE_key_c, fhipe);
      if(i%100 == 0) gmp_printf("#%d The inner product %Zd\n", i, el);
      cfe_vec_set(&c_ip, el, i);
    }

    int result = 0;
    for (size_t i = 0; i < length.orders; i++) {
      cfe_fhipe_decrypt(el, &orders[i], &FE_key_o, fhipe);
      if(i%500 == 0) gmp_printf("#%d The inner product %Zd\n", i, el);
      result += getIntersection(&c_ip, length.customer, el);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    d_sec = getExecutiontime(&start_time, &end_time);
    printf("time:%f\n", d_sec);
    printf("Result count(*)=%d\n", result);
    mpz_clears(el, el_, NULL);
    cfe_vec_frees(&query_c, &query_o, NULL);
    cfe_fhipe_fe_key_free(&FE_key_c);
    cfe_fhipe_fe_key_free(&FE_key_o);
}

int getIntersection(cfe_vec *innerprod, int length, mpz_t v){
  int result = 0;
  for (size_t i = 0; i < length; i++) {
    if (mpz_cmp(innerprod->vec[i], v) == 0) result++;
  }
  return result;
}
