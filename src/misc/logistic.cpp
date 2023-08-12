#include <stdio.h>
#include <cmath>
#include <vector>
#include <map>
#include <numeric>
#include <cstring>
#include "logistic.h"
using namespace std;


double sigmoid(double a)
{
  return 1.0 / (1.0 + exp(-a));
}


double logistic(Logistic param, double *x)
{
    double dot = 0;
    size_t length = sizeof(&x) / sizeof(x[0]);
    for (size_t i = 0; i < length; i++) {
      dot += param.w[i] * x[i];
    }
    return sigmoid(dot + param.b);
}


Logistic grad(Logistic param, double x[], int label)
{
    Logistic grad;
    double error = label - logistic(param, x);
    for (size_t i = 0; i < dim; ++i){
      grad.w[i] = (-1) * x[i] * error;
    }
    // printf("grad.w[3]=%f\t", grad.w[3]);
    grad.b = (-1) * error;
    return grad;
}

void clipping(Logistic *grad, int clip_size)
{
  double norm = 0;
  for (size_t i = 0; i < dim; ++i){
    norm += grad->w[i] * grad->w[i];
  }
  norm += grad->b * grad->b;
  norm = sqrt(norm);
  if(norm > clip_size){
    for (size_t i = 0; i < dim; ++i){
      grad->w[i] = (clip_size * grad->w[i]) / norm;
    }
    grad->b = (clip_size * grad->b) / norm;
  }
}

void grad_by_client(
  Logistic *grads,
  Logistic param,
  double data[][dim],
  int *label,
  int n_clients)
{
  for (size_t i = 0; i < n_clients; i++) {
    grads[i] = grad(param, data[i], label[i]);
    clipping(&grads[i], 1);
  }
}


void sum_vector(double *vec_r, double *vec_a, double *vec_b)
{
  // vec_r = vec_a + vec_b
  size_t length = sizeof(&vec_a) / sizeof(vec_a[0]);
  for (size_t i = 0; i < length; i++) {
    vec_r[i] = vec_a[i] + vec_b[i];
  }
}


void mult_vector(double* vec_r, double* vec_a, double cons)
{
  // vec_r = vec_a * cons
  size_t length = sizeof(&vec_a) / sizeof(vec_a[0]);
  for (size_t i = 0; i < length; i++) {
    vec_r[i] = vec_a[i] * cons;
  }
}


void divide_vector(double* vec_r, double* vec_a, double cons)
{
  // vec_r = vec_a / cons
  size_t length = sizeof(&vec_a) / sizeof(vec_a[0]);
  for (size_t i = 0; i < length; i++) {
    vec_r[i] = vec_a[i] / cons;
  }
}

void split_array(int *sub_array, int *array, int start, int length) {
  memmove(sub_array, &array[start], sizeof(int) * length);
  // printf("split_array: %d -> %d\n", array[start+length-1], sub_array[length-1]);
}


void split_mat(double sub_array[][dim], double array[][dim], int start, int length) {
  for (size_t i = 0; i < length; i++) {
      memmove(sub_array[i], &array[start+i], sizeof(double) * dim);
  }
}

Logistic update_param_server(Logistic param, Logistic grads[], float eta, int n_clients)
{
    Logistic update_param;
    // size_t n_clients = sizeof(&grads) / sizeof(grads[0]);
    for (size_t i = 0; i < n_clients; i++) {
      sum_vector(update_param.w, update_param.w, grads[i].w);
      update_param.b += grads[i].b;
    }
    // update_param.w = update_param.w / n_clients;
    divide_vector(update_param.w, update_param.w, n_clients);
    update_param.b = update_param.b / n_clients;

    // update_param.w = param.w - eta * update_param.w;
    mult_vector(update_param.w, update_param.w, (-1)*eta);
    sum_vector(update_param.w, param.w, update_param.w);
    update_param.b = param.b - eta * update_param.b;
    return update_param;
}


double loss(Logistic param, double x[][dim], int *label, int n_clients)
{
  double loss = 0;
  for (size_t i = 0; i < n_clients; i++) {
    loss += abs(label[i] - logistic(param, x[i]));
  }
  loss = loss / n_clients;
  return loss;
}


void gen_random_train(int size, double x[][dim], int *label)
{
  srand(time(NULL));
  int half = round(size/2);
  double x1[half][dim];
  double x2[half][dim];

  int label1[half];
  int label2[half];
  // Initialzation
  for (int i = 0; i < half; i++) {
    label1[i] = 0;
    label2[i] = 1;
    label[2*i] = 0;
    label[2*i+1] = 1;
    for (int j = 0; j < dim; ++j) {
      x1[i][j] = (double)rand()/RAND_MAX;
      x2[i][j] = (double)rand()/RAND_MAX + 6;
      x[2*i][j] = x1[i][j];
      x[2*i+1][j] = x2[i][j];
    }
  }
}


Logistic init_param()
{
  Logistic param;
  srand(time(NULL));
  for (int i = 0; i < dim; ++i) {
    param.w[i] = (double)rand()/RAND_MAX;
  }
  param.b = (double)rand()/RAND_MAX;
  return param;
}


void logistic_regression_plain()
{
  int N = 10000;
  double x[N][dim];
  int label[N];
  gen_random_train(N, x, label);
  Logistic param = init_param();

  double eta = 0.1;
  int minibatch_size = 100;
  int epoch = 1;
  int iteration = N/minibatch_size;

  Logistic* agg_grad = (Logistic *)malloc(minibatch_size * sizeof(Logistic));
  double sub_x[minibatch_size][dim];
  int sub_label[minibatch_size];

  printf("## Loss = %f\n", loss(param, x, label, N));
  for (size_t i = 0; i < epoch; i++) {
    for (size_t j = 0; j < iteration; j++) {
      printf("# Epoch %ld, Iteration %ld\n", i, j);
      split_mat(sub_x, x, j*minibatch_size, minibatch_size);
      split_array(sub_label, label, j*minibatch_size, minibatch_size);
      grad_by_client(agg_grad, param, sub_x, sub_label, minibatch_size);
      param = update_param_server(param, agg_grad, eta, minibatch_size);
      printf("## Loss = %f\n", loss(param, x, label, N));
    }
  }
  free(agg_grad);
}
