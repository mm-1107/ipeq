#ifdef __cplusplus
extern "C" {
#endif
#define dim 10

struct logistic {
    double w[dim];
    double b;
};
typedef struct logistic Logistic;

double sigmoid(double a);
double logistic(Logistic param, double *x);
Logistic grad(Logistic param, double *x, int label);
void grad_by_client(Logistic grads[], Logistic param, double **data, int *label);
Logistic update_param_server(Logistic param, Logistic grads[], float eta);
#ifdef __cplusplus
}
#endif
