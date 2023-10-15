#!/usr/bin/env python3

from cyaron import Graph,randint,IO

def main():
    n_testcase=int(1e4)
    weight_limit=(1,int(1e3))
    for i_testcase in range(1,n_testcase+1):
        n=randint(2,int(1e2))
        m=randint(n-1,min(int(n*(n-1)/2),int(1e3)))
        graph=Graph.UDAG(n,m,weight_limit=weight_limit)
        file_testcase=IO(file_prefix="s/",input_suffix="",disable_output=True,data_id=i_testcase)
        file_testcase.input_writeln(n)
        file_testcase.input_writeln()
        for edge in graph.iterate_edges():
            file_testcase.input_writeln(edge.start-1,edge.end-1,edge.weight)
            file_testcase.input_writeln(edge.end-1,edge.start-1,edge.weight)
        print("s", i_testcase, 0, n-1)

if __name__=='__main__':
    main()
