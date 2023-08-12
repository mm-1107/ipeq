import math
from fractions import Fraction

import discretegauss
import cdp2adp
import numpy as np
import sys

args = sys.argv
dim = int(args[1])

data = np.random.randint(-128, high=128, size=dim+1).tolist()

#set overall DP parameters
eps=1
delta=1e-6
#convert to concentrated DP
rho=cdp2adp.cdp_rho(eps,delta)
print(str(rho)+"-CDP implies ("+str(eps)+","+str(delta)+")-DP")
#number of queries
k = 1
#divide privacy budget up amongst queries
#Each query needs to be (rho/k)-concentrated DP
#cast to Fraction so subsequent arithmetic is exact
rho_per_q = Fraction(rho)/k
#compute noise variance parameter per query
sigma2=1/(2*rho_per_q)
#actual variance, at most sigma2
var = discretegauss.variance(sigma2)
print("standard deviation for each count = "+str(math.sqrt(var)))

prvate_vec = np.empty(shape=len(data), dtype=int).tolist()
noise_vec = np.empty(shape=len(data), dtype=int).tolist()
#evaluate queries and privatize
for idx, true_ans in enumerate(data):
    assert isinstance(true_ans,int) #important that it is rounded to an integer
    #sample noise and add
    noise = discretegauss.sample_dgauss(sigma2)
    prvate_vec[idx] = true_ans + noise
    noise_vec[idx] = noise
    #output
print("true =", data)
print("private =", prvate_vec)

with open(r'./noise.txt', 'w') as fp:
    for noise in noise_vec:
        # write each item on a new line
        fp.write(str(noise)+"\n")
    print('Done')
