#!/usr/bin/env python3

from random import randint,normalvariate

def main():
    pool=set()
    n=int(1e4)
    low=int(0)
    up=int(1e5)
    mu=0
    sigma=3e3
    while(len(pool)<n):
        pool.add(randint(low,up))
        # pool.add(round(abs(normalvariate(mu,sigma))))
    pool=sorted(pool)
    for x,i in enumerate(pool,1):
        print('s',x,i)


if __name__=='__main__':
    main()
