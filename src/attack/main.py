import pandas as pd
import category_encoders as ce
from scipy.spatial.distance import hamming
import matplotlib.pyplot as plt
import numpy as np
import random
from collections import Counter

# [1, orderkey, custkey, orderstatus, orderpriority]
def get_freq(df, columns):
    return df.groupby(columns).count()["o_key"]/len(df)


def read_order():
    columns = ["o_key", "c_key", "o_status", "price", "date", "o_priority",
        "cleak", "s_priority", "comment"]
    original_df = pd.read_csv("../../dataset/TPCH/scale_01/orders.csv",
                     sep="|", names=columns)

    return original_df


def binary_order(df):
    list_cols = ["o_status", "o_priority"]
    encoder =  ce.BinaryEncoder(cols=list_cols, drop_invariant=True)
    encode_df =  encoder.fit_transform(df[list_cols])
    return encode_df


def plot_hist(hamming, title, query):
    fig, ax = plt.subplots()
    left = hamming.keys()
    height = hamming.values()
    ax.bar(left, height, align="center")
    ax.title(query)
    fig.savefig(title)
    plt.close(fig)
    return (n, bins, patches)


def gen_queries(df):
    row_unique = df.drop_duplicates(keep='last')
    queries = [row_unique.iloc[idx, :].values.tolist() for idx in range(len(row_unique))]
    print("Queries =", queries)
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
    return l1_norm


def attack(order_all, order_adv, query_obs, queries):
    # query execution for obs dataset
    hamming_obs = [hamming(query_obs, list(order_all.iloc[idx])) * len(query)
                    for idx in range(len(order_all))]
    # {distance: num of row}
    distance_obs = Counter(hamming_obs)
    # plot_hist(distance_obs, "hamming_obs", query_obs)

    # Return the minimum distance query
    min_dict = {"query": None, "l1norm": 1}
    for idx, query_adv in enumerate(queries):
        # query execution for adv dataset
        hamming_adv = [hamming(query_adv, list(order_adv.iloc[idx])) * len(query_adv)
                        for idx in range(len(order_adv))]
        # (n_adv, bins_adv, patches) = plot_hist(hamming_adv, f"hamming_adv_{idx}.png")
        if max(hamming_obs) >= max(hamming_adv):
            distance_adv = Counter(hamming_adv)
            # compare distibution
            diff = compare_hist(distance_obs, distance_adv)
            if diff < min_dict["l1norm"]:
                min_dict["l1norm"] = diff
                min_dict["query"] = query_adv
    print(query_obs, min_dict)
    return query_obs == min_dict["query"]

if __name__ == '__main__':
    order_df = read_order()
    order_all = binary_order(order_df)
    queries = gen_queries(order_all)

    acc = 0
    trial = 10
    frac = 0.5
    for i in range(trial):
        print(f"#{i}")
        # Sample adv dataset
        order_adv = order_all.sample(frac=frac, replace=False)
        for query in queries:
            print(f"## query_obs={query}")
            acc += attack(order_all, order_adv, query, queries)
    print(f"frac={frac}, accuracy={acc / (len(queries) * trial)}")
