#include "adult.h"

void readAdult(double **dataset, int *label, char* fname, size_t row_num){
  FILE *fin = fopen(fname, "rt");
    if (!fin) {
        perror("fopen");
        exit(1);
    }
    char workclass[20];
    char education[10];
    char marital_status[10];
    char occupation[20];
    char relationship[20];
    char race[10];
    char sex[10];
    char native_country[20];
    char income_flg[10];
    int age;
    float fnlwgt;
    // dataset = [age, workclass, education, education_num]
    // printf("|idx|age|workclass|fnlwgt|education|education_num|marital_status|occupation|relationship|race|sex|capital_gain|capital_loss|hours_per_week|native_country|income_flg|\n");
    for (size_t i = 0; i < row_num; i++) {
      fscanf(fin, "%d, %[^,], %d, %[^,], %d, %[^,], %[^,], %[^,], %[^,], %[^,], %d, %d, %d, %[^,], %s\n",
      &dataset[i][0], workclass, &db[i].fnlwgt, education,
      &db[i].education_num, marital_status, occupation,
      relationship, race, sex, &db[i].capital_gain,
      &db[i].capital_loss, &db[i].hours_per_week, native_country, income_flg);
      // String -> Int
      db[i].workclass = writeWorkclass(workclass);
      db[i].education = writeEducation(education);
      db[i].marital_status = writeMarital_status(marital_status);
      db[i].occupation = writeOccupation(occupation);
      db[i].relationship = writeRelationship(relationship);
      db[i].race = writeRace(race);
      db[i].sex = writeSex(sex);
      db[i].native_country = writeNative_country(native_country);
      label[i].income_flg = writeIncome_flg(income_flg);
      if(i%100==0) printf("wc: %s, %d\n", workclass, db[i].workclass);
    }
    printf("OK\n");
    // 使い終わったらファイルを閉じる
    fclose(fin);
}

void adultWorkload(int num) {
  // SELECT COUNT(*) FROM ADULT WHERE workclass="Federal-gov" and race="White"

}

int writeWorkclass(char x[]){
  // Private, Self-emp-not-inc, Self-emp-inc, Federal-gov, Local-gov, State-gov,
  // Without-pay, Never-worked.
  int r;
  if(strcmp(x, "Private") == 0) r = 1;
  else if(strcmp(x, "Self-emp-not-inc") == 0) r = 2;
  else if(strcmp(x, "Self-emp-inc") == 0) r = 3;
  else if(strcmp(x, "Federal-gov") == 0) r = 4;
  else if(strcmp(x, "Local-gov") == 0) r = 5;
  else if(strcmp(x, "State-gov") == 0) r = 6;
  else if(strcmp(x, "Without-pay") == 0) r = 7;
  else if(strcmp(x, "Never-worked") == 0) r = 8;
  else r = 9;
  return r;
}

int writeEducation(char x[]){
  // Bachelors, Some-college, 11th, HS-grad, Prof-school, Assoc-acdm, Assoc-voc,
  // 9th, 7th-8th, 12th, Masters, 1st-4th, 10th, Doctorate, 5th-6th, Preschool.
  int r;
  if(strcmp(x, "Bachelors") == 0) r = 1;
  else if(strcmp(x, "Some-college") == 0) r = 2;
  else if(strcmp(x, "11th") == 0) r = 3;
  else if(strcmp(x, "HS-grad") == 0) r = 4;
  else if(strcmp(x, "Prof-school") == 0) r = 5;
  else if(strcmp(x, "Assoc-acdm") == 0) r = 6;
  else if(strcmp(x, "Assoc-voc") == 0) r = 7;
  else if(strcmp(x, "9th") == 0) r = 8;
  else if(strcmp(x, "7th-8th") == 0) r = 9;
  else if(strcmp(x, "12th") == 0) r = 10;
  else if(strcmp(x, "Masters") == 0) r = 11;
  else if(strcmp(x, "1st-4th") == 0) r = 12;
  else if(strcmp(x, "10th") == 0) r = 13;
  else if(strcmp(x, "Doctorate") == 0) r = 14;
  else if(strcmp(x, "5th-6th") == 0) r = 15;
  else if(strcmp(x, "Preschool") == 0) r = 16;
  else  r = 17;
  return r;
}
//
int writeMarital_status(char x[]){
  // Married-civ-spouse, Divorced, Never-married, Separated, Widowed,
  // Married-spouse-absent, Married-AF-spouse.
  int r;
  if(strcmp(x, "Married-civ-spouse") == 0) r = 1;
  else if(strcmp(x, "Divorced") == 0) r = 2;
  else if(strcmp(x, "Never-married") == 0) r = 3;
  else if(strcmp(x, "Separated") == 0) r = 4;
  else if(strcmp(x, "Widowed") == 0) r = 5;
  else if(strcmp(x, "Married-spouse-absent") == 0) r = 6;
  else if(strcmp(x, "Married-AF-spouse") == 0) r = 7;
  else  r = 8;
  return r;
}

int writeOccupation(char x[]){
  int r;
  // Tech-support, Craft-repair, Other-service, Sales, Exec-managerial,
  // Prof-specialty, Handlers-cleaners, Machine-op-inspct, Adm-clerical,
  // Farming-fishing, Transport-moving, Priv-house-serv, Protective-serv, Armed-Forces.
  if(strcmp(x, "Tech-support") == 0) r = 1;
  else if(strcmp(x, "Craft-repair") == 0) r = 2;
  else if(strcmp(x, "Other-service") == 0) r = 3;
  else if(strcmp(x, "Sales") == 0) r = 4;
  else if(strcmp(x, "Exec-managerial") == 0) r = 5;
  else if(strcmp(x, "Prof-specialty") == 0) r = 6;
  else if(strcmp(x, "Handlers-cleaners") == 0) r = 7;
  else if(strcmp(x, "Machine-op-inspct") == 0) r = 8;
  else if(strcmp(x, "Adm-clerical") == 0) r = 9;
  else if(strcmp(x, "Farming-fishing") == 0) r = 10;
  else if(strcmp(x, "Transport-moving") == 0) r = 11;
  else if(strcmp(x, "Priv-house-serv") == 0) r = 12;
  else if(strcmp(x, "Protective-serv") == 0) r = 13;
  else if(strcmp(x, "Armed-Forces") == 0) r = 14;
  else  r = 15;
  return r;
}

int writeRelationship(char x[]){
  // Wife, Own-child, Husband, Not-in-family, Other-relative, Unmarried.
  int r;
  if(strcmp(x, "Wife") == 0) r = 1;
  else if(strcmp(x, "Own-child") == 0) r = 2;
  else if(strcmp(x, "Husband") == 0) r = 3;
  else if(strcmp(x, "Not-in-family") == 0) r = 4;
  else if(strcmp(x, "Other-relative") == 0) r = 5;
  else if(strcmp(x, "Unmarried") == 0) r = 6;
  else  r = 7;
  return r;
}

int writeRace(char x[]){
  // White, Asian-Pac-Islander, Amer-Indian-Eskimo, Other, Black.
  int r;
  if(strcmp(x, "White") == 0) r = 1;
  else if(strcmp(x, "Asian-Pac-Islander") == 0) r = 2;
  else if(strcmp(x, "Amer-Indian-Eskimo") == 0) r = 3;
  else if(strcmp(x, "Other") == 0) r = 4;
  else if(strcmp(x, "Black") == 0) r = 5;
  else  r = 6;
  return r;
}

int writeSex(char x[]){
  // Female, Male
  int r;
  if(strcmp(x, "Female") == 0) r = 1;
  else if(strcmp(x, "Male") == 0) r = 2;
  else  r = 3;
  return r;
}

int writeNative_country(char x[]){
  // United-States, Cambodia, England, Puerto-Rico, Canada, Germany,
  // Outlying-US(Guam-USVI-etc), India, Japan, Greece, South, China,
  // Cuba, Iran, Honduras, Philippines, Italy, Poland, Jamaica, Vietnam,
  // Mexico, Portugal, Ireland, France, Dominican-Republic, Laos, Ecuador,
  // Taiwan, Haiti, Columbia, Hungary, Guatemala, Nicaragua, Scotland, Thailand,
  // Yugoslavia, El-Salvador, Trinadad&Tobago, Peru, Hong, Holand-Netherlands.
  int r;
  if(strcmp(x, "United-States") == 0) r = 1;
  else if(strcmp(x, "Cambodia") == 0) r = 2;
  else if(strcmp(x, "England") == 0) r = 3;
  else if(strcmp(x, "Puerto-Rico") == 0) r = 4;
  else if(strcmp(x, "Canada") == 0) r = 5;
  else if(strcmp(x, "Germany") == 0) r = 6;
  else if(strcmp(x, "Outlying-US(Guam-USVI-etc)") == 0) r = 7;
  else if(strcmp(x, "India") == 0) r = 8;
  else if(strcmp(x, "Japan") == 0) r = 9;
  else if(strcmp(x, "Greece") == 0) r = 10;
  else if(strcmp(x, "South") == 0) r = 11;
  else if(strcmp(x, "China") == 0) r = 12;
  else if(strcmp(x, "Cuba") == 0) r = 13;
  else if(strcmp(x, "Iran") == 0) r = 14;
  else if(strcmp(x, "Honduras") == 0) r = 15;
  else if(strcmp(x, "Philippines") == 0) r = 16;
  else if(strcmp(x, "Italy") == 0) r = 17;
  else if(strcmp(x, "Poland") == 0) r = 18;
  else if(strcmp(x, "Jamaica") == 0) r = 19;
  else if(strcmp(x, "Vietnam") == 0) r = 20;
  else if(strcmp(x, "Mexico") == 0) r = 21;
  else if(strcmp(x, "Portugal") == 0) r = 22;
  else if(strcmp(x, "Ireland") == 0) r = 23;
  else if(strcmp(x, "France") == 0) r = 24;
  else if(strcmp(x, "Dominican-Republic") == 0) r = 25;
  else if(strcmp(x, "Laos") == 0) r = 26;
  else if(strcmp(x, "Ecuador") == 0) r = 27;
  else if(strcmp(x, "Taiwan") == 0) r = 28;
  else if(strcmp(x, "Haiti") == 0) r = 29;
  else if(strcmp(x, "Columbia") == 0) r = 30;
  else if(strcmp(x, "Hungary") == 0) r = 31;
  else if(strcmp(x, "Guatemala") == 0) r = 32;
  else if(strcmp(x, "Nicaragua") == 0) r = 33;
  else if(strcmp(x, "Scotland") == 0) r = 34;
  else if(strcmp(x, "Thailand") == 0) r = 35;
  else if(strcmp(x, "Yugoslavia") == 0) r = 36;
  else if(strcmp(x, "El-Salvador") == 0) r = 37;
  else if(strcmp(x, "Trinadad&Tobago") == 0) r = 38;
  else if(strcmp(x, "Peru") == 0) r = 39;
  else if(strcmp(x, "Hong") == 0) r = 40;
  else if(strcmp(x, "Holand-Netherlands") == 0) r = 41;
  else  r = 42;
  return r;
}

int writeIncome_flg(char x[]){
  // >50K, <=50K
  int r;
  if(strcmp(x, ">50K") == 0) r = 0;
  else if(strcmp(x, "<=50K") == 0) r = 1;
  else  r = 3;
  return r;
}
/*
void encAdult(cfe_fhipe_ciphertext *cipher, Adult *db, int len,
  cfe_fhipe_sec_key *sec_key, cfe_fhipe *fhipe){
    mpz_t el;
    mpz_init(el);
    cfe_vec x;
    cfe_vec_inits(L, &x, NULL);
    fillAll(&x, L, 0);  // Must be outside of the loop
    cfe_fhipe fhipe_copy;
    for (size_t i = 0; i < len; i++) {
      cfe_fhipe_copy(&fhipe_copy, fhipe);
      cfe_fhipe_ciphertext_init(&cipher[i], &fhipe_copy);
      // [1, age, workclass, ..., income_flg, 0,...,0]
      mpz_set_ui(el, 1);
      cfe_vec_set(&x, el, 0);

      mpz_set_ui(el, db[i].age);
      cfe_vec_set(&x, el, 1);

      mpz_set_ui(el, db[i].workclass);
      cfe_vec_set(&x, el, 2);

      // mpz_set_ui(el, db[i].fnlwgt);
      mpz_set_ui(el, 0);
      cfe_vec_set(&x, el, 3);

      mpz_set_ui(el, db[i].education);
      cfe_vec_set(&x, el, 4);

      mpz_set_ui(el, db[i].education_num);
      cfe_vec_set(&x, el, 5);

      mpz_set_ui(el, db[i].marital_status);
      cfe_vec_set(&x, el, 6);

      mpz_set_ui(el, db[i].occupation);
      cfe_vec_set(&x, el, 7);

      mpz_set_ui(el, db[i].relationship);
      cfe_vec_set(&x, el, 8);

      mpz_set_ui(el, db[i].race);
      cfe_vec_set(&x, el, 9);

      mpz_set_ui(el, db[i].sex);
      cfe_vec_set(&x, el, 10);

      // mpz_set_ui(el, db[i].capital_gain);
      mpz_set_ui(el, 0);
      cfe_vec_set(&x, el, 11);

      // mpz_set_ui(el, db[i].capital_loss);
      mpz_set_ui(el, 0);
      cfe_vec_set(&x, el, 12);

      mpz_set_ui(el, db[i].hours_per_week);
      cfe_vec_set(&x, el, 13);

      mpz_set_ui(el, db[i].native_country);
      cfe_vec_set(&x, el, 14);

      mpz_set_ui(el, db[i].income_flg);
      cfe_vec_set(&x, el, 15);
      if (i%100==1) printVec("adult[%ld]: ", &x);
      cfe_fhipe_encrypt(&cipher[i], &x, sec_key, &fhipe_copy);
    }
    mpz_clears(el, NULL);
    cfe_vec_frees(&x, NULL);
    cfe_fhipe_free(&fhipe_copy);
}
*/
