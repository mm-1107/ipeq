#include <stdio.h>
#include <cmath>
#include <vector>
#include <map>
#include <numeric>
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
    // vector<double> vec_w(std::begin(param_w), end(param_w));
    // vector<double> vec_x(std::begin(x), end(x));
    // double dot = inner_product(vec_w.begin(), vec_w.end(), vec_x.begin(), 0);
    return sigmoid(dot + param.b);
}

Logistic grad(Logistic param, double *x, int label)
{
    Logistic grad;
    double error = label - logistic(param, x);
    size_t length = sizeof(&x) / sizeof(x[0]);
    for (size_t i = 0, size = length; i < size; ++i)
      grad.w[i] = (-1) * x[i] * error;
    grad.b = (-1) * error;
    return grad;
}

void grad_by_client(Logistic grads[], Logistic param, double **data, int *label)
{
  size_t n_clients = sizeof(&grads) / sizeof(grads[0]);
  for (size_t i = 0; i < n_clients; i++) {
    grads[i] = grad(param, data[i], label[i]);
  }
}

template <typename T1, typename T2, typename T3>
void sum_vector(T1 vec_r, T2 vec_a, T3 vec_b){
  // vec_r = vec_a + vec_b
  size_t length = sizeof(&vec_a) / sizeof(vec_a[0]);
  for (size_t i = 0; i < length; i++) {
    vec_r[i] = vec_a[i] + vec_b[i];
  }
}

template <typename T1, typename T2, typename T3>
void mult_vector(T1 vec_r, T2 vec_a, T3 cons)
{
  // vec_r = vec_a * cons
  size_t length = sizeof(&vec_a) / sizeof(vec_a[0]);
  for (size_t i = 0; i < length; i++) {
    vec_r[i] = vec_a[i] * cons;
  }
}

template <typename T1, typename T2, typename T3>
void divide_vector(T1 vec_r, T2 vec_a, T3 cons)
{
  // vec_r = vec_a / cons
  size_t length = sizeof(&vec_a) / sizeof(vec_a[0]);
  for (size_t i = 0; i < length; i++) {
    vec_r[i] = vec_a[i] / cons;
  }
}

Logistic update_param_server(Logistic param, Logistic grads[], float eta)
{
    Logistic update_param;
    size_t n_clients = sizeof(&grads) / sizeof(grads[0]);
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

void test()
{
  int d = 5;
  int N = 100;
  x1 = np.random.randn(N/2, d);
  std::srand(time(NULL));
  vector<double> x1(round(N/2));
  vector<double> x2(round(N/2));

  vector<int> label1(round(N/2));
  vector<int> label2(round(N/2));
  for (int i = 0; i < round(N/2); ++i) {
    x1[i] = (double)rand()/RAND_MAX;
    label1[i] = 0;
  }
  for (int i = 0; i < round(N/2); ++i) {
    x2[i] = (double)rand()/RAND_MAX + 6;
    label2[i] = 1;
  }
  vector<double> x(x1);
  x.insert(x.end(), x2.begin(), x2.end());
  vector<int> label(label1);
  label.insert(label.end(), label2.begin(), label2.end());

  // dataset = np.column_stack((x,label))
  // np.random.shuffle(dataset)
  //
  // x = dataset[:, :d]
  // label = dataset[:, d]

  param = dict()
  param["w"] = np.random.rand(d)
  param["b"] = np.random.random()

  eta = 0.1

  int minibatch_size = 100;

  for epoch in range(10):
      print("##", epoch)
      for iteration, index in enumerate(range(0, x.shape[0], minibatch_size)):
          _x = x[index:index + minibatch_size]
          _label = label[index:index + minibatch_size]
          # print(_x, _label)
          grad_by_client(grads, param, double **data, int *label)
          agg_grad = grad_by_client(param, _x, _label)
          # w_grad, b_grad = grad(_x, _label)
          param = update_param_server(param, agg_grad, eta=0.1)
          # w -= eta * w_grad
          # b -= eta * b_grad
          loss_list.append(np.mean(np.abs(label - logistic(param, x))))

  # 損失の確認
  print(np.mean(np.abs(label - logistic(param, x))))
}
