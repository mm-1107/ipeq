import numpy as np
# import matplotlib.pyplot as plt

def sigmoid(a):
    return 1.0 / (1.0 + np.exp(-a))


def logistic(param, x):
    return sigmoid(np.dot(x, param["w"]) + param["b"])


def grad(param, x, label):
    grad = dict()
    error = label - logistic(param, x) # (正解ラベルy) - (予測値y*)
    grad["w"] = -np.mean(x.T * error, axis=1)
    grad["b"] = -np.mean(error) # 式（１）
    # print(grad)
    return grad


def grad_by_client(param, data_list, label_list):
    agg_w_grad = []
    agg_b_grad = []
    data_size = len(data_list)
    agg_grad = [grad(param, data_list[idx:idx+1], label_list[idx:idx+1]) for idx in range(data_size)]
    return agg_grad


def update_param_server(param, agg_grad, eta=0.1):
    mean_grad = dict()
    for name in param:
        mean_grad[name] = np.zeros_like(agg_grad[0][name])
        for grad in agg_grad:
            mean_grad[name] += grad[name]
        mean_grad[name] = mean_grad[name] / len(agg_grad)
        param[name] -= eta * mean_grad[name]
    return param


if __name__ == '__main__':
    d = 10
    N = 10000
    x1 = np.random.randn(N//2, d)
    x2 = np.random.randn(N//2, d) + np.array(np.full(d, 6))

    x = np.vstack((x1, x2))

    label1 = np.zeros(N//2)
    label2 = np.ones(N//2)
    label = np.hstack((label1, label2))

    dataset = np.column_stack((x,label))
    np.random.shuffle(dataset) #データ点の順番をシャッフル

    x = dataset[:, :d]
    label = dataset[:, d]

    # plt.scatter(x1[:, 0], x1[:, 1], c='r')
    # plt.scatter(x2[:, 0], x2[:, 1], c='b')
    # plt.show()
    # plt.close()

    param = dict()
    param["w"] = np.random.rand(d)
    param["b"] = np.random.random()

    eta = 0.1


    # １つ選択
    # minibatch_size = N # 最急降下法
    # minibatch_size = 1 # 確率的勾配降下法
    minibatch_size = 100 # ミニバッチ確率的勾配降下法

    # パラメータ更新毎の損失
    loss_list = list()

    for epoch in range(1):
        print("##", epoch)
        for iteration, index in enumerate(range(0, x.shape[0], minibatch_size)):
            _x = x[index:index + minibatch_size]
            _label = label[index:index + minibatch_size]
            # print(_x, _label)
            agg_grad = grad_by_client(param, _x, _label)
            # w_grad, b_grad = grad(_x, _label)
            param = update_param_server(param, agg_grad, eta=0.1)
            # w -= eta * w_grad
            # b -= eta * b_grad
            loss_list.append(np.mean(np.abs(label - logistic(param, x))))

    # 損失の確認
    print(np.mean(np.abs(label - logistic(param, x))))
    # plt.plot(loss_list)
    # plt.show()
    # plt.close()

    # 回帰直線の確認
    # bx = np.arange(-6, 10, 0.1)
    # by = -b/w[1] - w[0]/w[1]*bx
    # plt.xlim([-5, 10])
    # plt.ylim([-5, 9])
    # plt.plot(bx, by)
    # plt.scatter(x1[:, 0], x1[:, 1], c='r')
    # plt.scatter(x2[:, 0], x2[:, 1], c='b')
    # plt.show()
    # plt.close()
