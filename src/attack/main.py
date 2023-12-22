import itertools
import pandas as pd
import category_encoders as ce
from scipy.spatial.distance import hamming
import matplotlib.pyplot as plt
import numpy as np
import random
from copy import deepcopy
from collections import Counter
from munkres import Munkres
# columns = ["o_status", "o_priority", "year", "month", "day"]
columns = ["o_status", "o_priority"]
# [1, orderkey, custkey, orderstatus, orderpriority]
def get_freq(df, columns):
    return df.groupby(columns).count()["o_key"]/len(df)


def read_order():
    cols = ["o_key", "c_key", "o_status", "price", "date", "o_priority",
        "cleak", "s_priority", "comment"]
    original_df = pd.read_csv("../../dataset/TPCH/scale_05/orders.csv",
                     sep="|", names=cols)
    original_df[["year", "month", "day"]] = original_df["date"].str.split("-", expand=True)
    return original_df


def binary_order(df):
    encoder =  ce.BinaryEncoder(cols=columns, drop_invariant=True)
    encode_df =  encoder.fit_transform(df[columns])
    return encode_df


def plot_hist(hamming, title, query):
    fig, ax = plt.subplots()
    left = hamming.keys()
    height = hamming.values()
    ax.bar(left, height, align="center")
    ax.set_xlabel("Hamming Distance")
    ax.set_ylabel("Number of records")
    ax.set_title(query)
    fig.savefig(title)
    plt.close(fig)


def product(args):
    pools = [tuple(pool) for pool in args]
    result = [[]]
    for pool in pools:
        result = [x+[y] for x in result for y in pool]
    for prod in result:
        yield tuple(prod)

def gen_queries(df):
    row_unique = df.drop_duplicates(keep='last')
    query_set = []
    for col in columns:
        unique_ = row_unique.filter(like=col, axis=1).drop_duplicates(keep='last')
        size = unique_.shape[1]
        l = [unique_.iloc[idx, :].values.tolist() for idx in range(len(unique_))]
        l.append([0]*size)
        query_set.append(l)

    queries = []
    pair = product(query_set)
    for p in pair:
        concat = []
        for list in p:
            concat += list
        if concat != [0] * len(concat):
            queries.append(concat)
    return queries


def aux_hamming(order_aux, queries):
    hamming_aux = dict()
    for query_aux in queries:
        # query execution for aux dataset
        hamming_aux[tuple(query_aux)] = \
            Counter([hamming(query_aux, list(order_aux.iloc[idx])) * len(query_aux)
                for idx in range(len(order_aux))])
    return hamming_aux


def kl_divergence(d_obs, d_aux):
    epsilon = 0.000001
    size_obs = sum(d_obs.values())
    size_aux = sum(d_aux.values())
    # l1 norm between hist_obs and hist_aux
    list_obs = []
    list_aux = []
    for key in d_obs:
        if key not in d_aux:
            d_aux[key] = 0
        list_obs.append(d_obs[key] / size_obs)
        list_aux.append(d_aux[key] / size_aux)
    np_obs = np.asarray(list_obs) + epsilon
    np_aux = np.asarray(list_aux) + epsilon
    divergence = np.sum(np_obs*np.log(np_obs/np_aux))
    return divergence, d_aux


def get_accurary(pairs):
    acc = 0
    for p in pairs:
        if p[0] == p[1]:
            acc += 1
    return acc/len(pairs)*100


def attack(order_all, queries, n_aux=500):
    order_obs = order_all[:20000]
    # Sample aux dataset
    order_aux = order_all[20000:].sample(frac=1, replace=False)[:n_aux]
    hamming_aux_dict = aux_hamming(order_aux, queries)
    acc = 0
    query_candidate = deepcopy(queries)
    all_query = len(queries)
    mat_l1_norms = []
    for idx, query_obs in enumerate(queries):
        print(f"## {idx+1}/{all_query}: query_obs={query_obs}")
        # query execution for obs dataset
        # {distance: num of row}
        hamming_obs = Counter([hamming(query_obs, list(order_obs.iloc[idx])) * len(query_obs)
                        for idx in range(len(order_obs))])
        # plot_hist(distance_obs, "hamming_obs", query_obs)
        l1_norms = []
        for idx, query_aux in enumerate(query_candidate):
            hamming_aux = hamming_aux_dict[tuple(query_aux)]
            if max(hamming_obs) >= max(hamming_aux):
                # compare distibution
                diff, dict_aux = kl_divergence(hamming_obs, hamming_aux)
                # plot_hist(distance_aux, f"hamming_aux{idx}", query_aux)
                l1_norms.append(diff)
            else:
                l1_norms.append(100)
        mat_l1_norms.append(l1_norms)
    # Matching with Hungarian Algorithm
    m = Munkres()
    opt = m.compute(mat_l1_norms)
    return get_accurary(opt)


if __name__ == '__main__':
    order_df = read_order()
    order_all = binary_order(order_df)
    queries = gen_queries(order_all)

    trial = 5
    n_aux = 500
    acc = []
    print(f"Size of queries = {len(queries)}")
    for i in range(trial):
        print(f"# Trial {i+1}")
        acc_ = attack(order_all, queries, n_aux)
        acc.append(acc_)
        print(f"n_aux={n_aux}, accuracy={acc_}\n")
    acc = np.array(acc)
    print(f"\nMean accuracy={np.mean(acc)}, Median accuracy={np.median(acc)}")
