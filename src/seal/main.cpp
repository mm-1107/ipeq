#include "seal/seal.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <tuple>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <fstream>
using namespace std;
using namespace seal;

struct Orders {
    vector<uint64_t> orderstatus;  // continuous
    vector<uint64_t> orderpriority;
    vector<uint64_t> get(string key) {
      if (key == "orderstatus") return orderstatus;
      else if (key == "orderpriority") return orderpriority;
      else exit(EXIT_FAILURE);
    }
};

int getLength(const char* fname){
  int line = 0;
  FILE *fin = fopen(fname, "rt");
  int c;
  while ((c = fgetc(fin)) != EOF) {
    if (c == '\n') line++;
  }
  fclose(fin);
  printf("%s: %d line\n", fname, line);
  return line;
}

class HEDB{
private:
    Encryptor* encryptor;
    Evaluator* evaluator;
    Decryptor* decryptor;
    BatchEncoder* batch_encoder;
    GaloisKeys galois_keys;
    RelinKeys relin_keys;
    size_t slot_count;
    size_t row_size;
public:
    int dbsize;
    map<string, vector<string>> columns;
    HEDB(int n){
      dbsize = n;
      setup();
    }
    void setup(){
      EncryptionParameters parms(scheme_type::bfv);
      size_t poly_modulus_degree = 8192;
      parms.set_poly_modulus_degree(poly_modulus_degree);
      parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
      parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
      SEALContext context(parms);
      KeyGenerator keygen(context);
      SecretKey secret_key = keygen.secret_key();;
      PublicKey public_key;
      keygen.create_public_key(public_key);
      keygen.create_galois_keys(galois_keys);
      keygen.create_relin_keys(relin_keys);
      encryptor = new Encryptor(context, public_key);
      evaluator = new Evaluator(context);
      decryptor = new Decryptor(context, secret_key);
      batch_encoder = new BatchEncoder(context);
      slot_count = batch_encoder->slot_count();
      row_size = slot_count / 2;
    }
    void defineColumns(string dataset){
      if (dataset == "orders"){
        columns["orderstatus"] = vector<string>{"O", "F", "P"};
        columns["orderpriority"] = vector<string>{"5-LOW", "1-URGENT", "4-NOT SPECIFIED", "2-HIGH", "3-MEDIUM"};
      }
    }

    vector<Orders> readOrders(const char* fname, int len){
      vector<Orders> db(len);
      FILE *fin = fopen(fname, "rt");
        if (!fin) {
            // 開くのに失敗したらエラーを出力する
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        defineColumns("orders");
        int orderkey; // base q (2 bit)
        int custkey; // base q (2 bit)
        char orderstatus[3]; // (2 bit)
        float totalprice; // not use
        char orderdate[20]; // not use
        char orderpriority[20]; // (3 bit)
        char clerk[20]; // not use
        int shippriority; // not use
        char comment[100]; // not use
        // printf("|idx|age|workclass|fnlwgt|education|education_num|marital_status|occupation|relationship|race|sex|capital_gain|capital_loss|hours_per_week|native_country|income_flg|\n");
        for (size_t i = 0; i < len; i++) {
          fscanf(fin, "%d|%d|%[^|]|%f|%[^|]|%[^|]|%[^|]|%d|%[^\n]%*c",
            &orderkey, &custkey, orderstatus, &totalprice,
            orderdate, orderpriority, clerk, &shippriority, comment);
          // String -> Int
          db[i].orderstatus = writeCategory("orderstatus", orderstatus);
          db[i].orderpriority = writeCategory("orderpriority", orderpriority);

          // if(i%100==0) printf("wc: %s, %d\n", workclass, age);
        }
        printf("Read OK\n");
        // 使い終わったらファイルを閉じる
        fclose(fin);
        return db;
    }

    vector<uint64_t> writeContinuous(string name, int age){
      vector<uint64_t> onehot(columns[name].size(), 0);
      for (int i = 0; i < onehot.size(); i++) {
          if(to_string(age) == columns[name][i]){
            onehot[i] = 1;
            break;
          }
      }
      return onehot;
    }

    vector<uint64_t> writeCategory(string name, char* x){
      vector<uint64_t> onehot(columns[name].size(), 0);
      for (int i = 0; i < onehot.size(); i++) {
        if(x == columns[name][i]) {
          onehot[i] = 1;
          break;
        }
      }
      return onehot;
    }

    vector<Ciphertext> encOrders(vector<Orders>& db) {
      int num_row = db.size();
      Plaintext plain_matrix;
      vector<uint64_t> pod_matrix;
      vector<Ciphertext> encrypted_matrix;
      for (size_t i = 0; i < num_row; i++) {
        // Encode
        pod_matrix = db[i].orderpriority;
        pod_matrix.insert(pod_matrix.end(), db[i].orderstatus.begin(), db[i].orderstatus.end());
        vector<uint64_t> padding(row_size-pod_matrix.size(), 0);
        pod_matrix.insert(pod_matrix.end(), padding.begin(), padding.end());
        Plaintext plain_matrix;
        batch_encoder->encode(pod_matrix, plain_matrix);
        Ciphertext encrypted;
        encryptor->encrypt(plain_matrix, encrypted);
        encrypted_matrix.push_back(encrypted);
      }
      return encrypted_matrix;
    }

    template <typename T>
    inline void print_matrix(std::vector<T> matrix, std::size_t row_size)
    {
        /*
        We're not going to print every column of the matrix (there are 2048). Instead
        print this many slots from beginning and end of the matrix.
        */
        std::size_t print_size = row_size/32;

        std::cout << std::endl;
        std::cout << "    [";
        for (std::size_t i = 0; i < print_size; i++)
        {
            std::cout << std::setw(3) << std::right << matrix[i] << ",";
        }
        std::cout << std::setw(3) << " ...,";
        for (std::size_t i = row_size - print_size; i < row_size; i++)
        {
            std::cout << std::setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
        }
    }

    int decryptResult(Ciphertext result){
      Plaintext plain_result;
      decryptor->decrypt(result, plain_result);
      vector<uint64_t> pod_matrix(slot_count, 0ULL);
      batch_encoder->decode(plain_result, pod_matrix);
      // print_matrix(pod_matrix, row_size);
      return pod_matrix[0];
    }

    int get_index(vector<string> vec, string wanted){
      vector<string>::iterator itr;
      itr = find(vec.begin(), vec.end(), wanted);
      if (itr == vec.end()) cout << "search failed" << endl;
      const int wanted_index = distance(vec.begin(), itr);
      return wanted_index;
    }

    vector<Plaintext> getFilter(map<string, string> query){
      vector<Plaintext> encoded_filter;
      Plaintext plain_filter;
      for (const auto& [key, value] : query){
        int position = 0;
        for (const auto& [name, vec] : columns){
          if (key != name) position += vec.size();
          else break;
        }
        int idx = position + get_index(columns[key], value);
        vector<uint64_t> filter(slot_count, 0);
        filter[idx] = 1;
        batch_encoder->encode(filter, plain_filter);
        encoded_filter.push_back(plain_filter);
      }
      return encoded_filter;
    }

    Ciphertext totalSum(Ciphertext ctxt){
      Ciphertext sum = ctxt;
      evaluator->rotate_rows_inplace(sum, 1, galois_keys);
      // Ciphertext sum = ctxt_rot;
      evaluator->add_inplace(sum, ctxt);
      int loop = log2(row_size);
      for (size_t i = 1; i < loop; i++) {
        // ctxt_rot = sum;
        Ciphertext tmp = sum;
        evaluator->rotate_rows_inplace(tmp, pow(2, i), galois_keys);
        evaluator->add_inplace(sum, tmp);
      }
      return sum;
    }

    Ciphertext countQuery(vector<Ciphertext>& db, map<string, string> query){
      Ciphertext result;
      vector<Plaintext> encoded_filter = getFilter(query);
      int numfilter = encoded_filter.size();

      vector<Ciphertext> enc_filter;
      for (size_t i = 0; i < numfilter; i++) {
        // Encrypt filter
        Ciphertext tmp;
        encryptor->encrypt(encoded_filter[i], tmp);
        enc_filter.push_back(tmp);
      }

      for (size_t i = 0; i < dbsize; i++) {
        Ciphertext tmp = db[i];
        // evaluator->multiply_plain_inplace(tmp, encoded_filter[0]);
        evaluator->multiply_inplace(tmp, enc_filter[0]);
        evaluator->relinearize_inplace(tmp, relin_keys);
        Ciphertext match = totalSum(tmp);
        for (size_t j = 1; j < numfilter; j++) {
          Ciphertext tmp = db[i];
          // evaluator->multiply_plain_inplace(tmp, encoded_filter[j]);
          evaluator->multiply_inplace(tmp, enc_filter[j]);
          evaluator->relinearize_inplace(tmp, relin_keys);
          evaluator->multiply_inplace(match, totalSum(tmp));
        }
        if (i == 0) result = match;
        else evaluator->add_inplace(result, match);
      }
      return result;
    }

    template <typename T>
    int correctCount(vector<T>& db, map<string, string> query) {
      int cnt = 0;
      int num_condition = query.size();
      for (size_t j = 0; j < db.size(); j++) {
        int matched = 0;
        for (const auto& [key, value] : query){
          vector<uint64_t> onehot = db[j].get(key);
          int idx = get_index(columns[key], value);
          if(onehot[idx] == 1) matched += 1;
        }
        if (num_condition == matched) cnt += 1;
      }
      return cnt;
    }
};

template<typename K, typename V>
void print_map(map<K, V> const &m)
{
    for (auto const &pair: m) {
        std::cout << "{" << pair.first << ": " << pair.second << "}\n";
    }
}

void saveCiphertext(const char* fname, Ciphertext ctxt){
  ofstream ofs(fname, ios::binary);
  if (!ofs.is_open()) {
      cout << fname << " can not be opened !" << endl;
      exit(EXIT_FAILURE);
  }
  ctxt.save(ofs);
  ofs.close();
}

int main(int argc, char const *argv[]) {
  int fix_size = 5000;
  HEDB hedb(fix_size);
  char fname[50];
  sprintf(fname, "%s%s%s", "../../../dataset/TPCH/scale_", argv[1], "/orders.csv");
  int len = getLength(fname);
  vector<Orders> a_db = hedb.readOrders(fname, len);
  map<string, string> query = {
    {"orderstatus", "O"}
  };
  cout << "Query = ";
  print_map(query);
  cout << "correct = " << hedb.correctCount(a_db, query) << endl;
  Ciphertext enc_result;
  int result = 0;
  double duration = 0;
  for (size_t i = 0; i < ((len + fix_size - 1) / fix_size); i++) {
    vector<Orders> small_db(a_db.begin() + i*fix_size, a_db.begin() + (i+1)*fix_size);
    vector<Ciphertext> a_encrypted = hedb.encOrders(small_db);
    cout << "Encrypted !" << endl;
    saveCiphertext("row_encrypted.bin", a_encrypted[0]);
    clock_t start = clock();
    enc_result = hedb.countQuery(a_encrypted, query);
    clock_t end = clock();
    duration += (double)(end - start) / CLOCKS_PER_SEC;
    cout << "duration = " << duration << "sec.\n";
    result += hedb.decryptResult(enc_result);
  }
  printf("Result %d\n", result);
  return 0;
}
