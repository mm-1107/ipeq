#ifdef __cplusplus
extern "C" {
#endif
#define dim 2
#define batch_size 100

struct logistic {
    double w[dim];
    double b;
};
typedef struct logistic Logistic;

double sigmoid(double a);
double logistic(Logistic param, double *x);
Logistic grad(Logistic param, double x[], int label);
void grad_by_client(Logistic *grads, Logistic param, double data[][dim], int *label, int n_clients);
Logistic update_param_server(Logistic param, Logistic grads[], float eta, int n_clients);
double loss(Logistic param, double x[][dim], int *label, int n_clients);
void sum_vector(double *vec_r, double *vec_a, double *vec_b);
void mult_vector(double* vec_r, double* vec_a, double cons);
void divide_vector(double* vec_r, double* vec_a, double cons);
void split_array(int *sub_array, int *array, int start, int length);
void split_mat(double sub_array[][dim], double array[][dim], int start, int length);
Logistic init_param();
void gen_random_train(int size, double x[][dim], int *label);
void logistic_regression_plain();
#ifdef __cplusplus
}
#endif
