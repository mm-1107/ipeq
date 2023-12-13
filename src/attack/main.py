import itertools
import pandas as pd
import category_encoders as ce
from scipy.spatial.distance import hamming
import matplotlib.pyplot as plt
import numpy as np
import random
from collections import Counter
columns = ["o_status", "o_priority", "year", "month", "day"]
# [1, orderkey, custkey, orderstatus, orderpriority]
def get_freq(df, columns):
    return df.groupby(columns).count()["o_key"]/len(df)


def read_order():
    cols = ["o_key", "c_key", "o_status", "price", "date", "o_priority",
        "cleak", "s_priority", "comment"]
    original_df = pd.read_csv("../../dataset/TPCH/scale_01/orders.csv",
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


def compare_hist(d_obs, d_adv):
    l1_norm = 0
    size_obs = sum(d_obs.values())
    size_adv = sum(d_adv.values())
    # l1 norm between hist_obs and hist_adv
    for key in d_obs:
        if key not in d_adv:
            d_adv[key] = 0
        freq_obs = d_obs[key] / size_obs
        freq_adv = d_adv[key] / size_adv
        l1_norm += abs(freq_obs - freq_adv)
    return l1_norm, d_adv


def adv_hamming(order_adv, queries):
    hamming_adv = dict()
    for query_adv in queries:
        # query execution for adv dataset
        hamming_adv[tuple(query_adv)] = \
            [hamming(query_adv, list(order_adv.iloc[idx])) * len(query_adv)
                for idx in range(len(order_adv))]
    return hamming_adv


def attack(order_all, queries, frac=0.1):
    # Sample adv dataset
    order_adv = order_all.sample(frac=frac, replace=False)
    hamming_adv_dict = adv_hamming(order_adv, queries)
    acc = 0
    for query_obs in queries:
        print(f"## query_obs={query_obs}")
        # query execution for obs dataset
        hamming_obs = [hamming(query_obs, list(order_all.iloc[idx])) * len(query_obs)
                        for idx in range(len(order_all))]
        # {distance: num of row}
        distance_obs = Counter(hamming_obs)
        # plot_hist(distance_obs, "hamming_obs", query_obs)

        # Return the minimum distance query
        min_dict = {"query": None, "l1norm": 1}
        for idx, query_adv in enumerate(queries):
            hamming_adv = hamming_adv_dict[tuple(query_adv)]
            if max(hamming_obs) >= max(hamming_adv):
                distance_adv = Counter(hamming_adv)
                # compare distibution
                diff, distance_adv = compare_hist(distance_obs, distance_adv)
                # plot_hist(distance_adv, f"hamming_adv{idx}", query_adv)
                if diff < min_dict["l1norm"]:
                    min_dict["l1norm"] = diff
                    min_dict["query"] = query_adv
        print("### Correct:", query_obs, "Predict:", min_dict)
        acc += (query_obs == min_dict["query"])
    return acc


if __name__ == '__main__':
    order_df = read_order()
    order_all = binary_order(order_df)
    queries = gen_queries(order_all)

    acc = 0
    trial = 1
    frac = 0.05
    print(f"Size of queries = {len(queries)}")
    for i in range(trial):
        print(f"# Trial {i}")
        acc += attack(order_all, queries, frac)
    print(f"frac={frac}, accuracy={acc / (len(queries) * trial)}")
