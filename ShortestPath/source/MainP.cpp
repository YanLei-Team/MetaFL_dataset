/************************************************************************/
/* $Id: MainP.cpp 65 2010-09-08 06:48:36Z yan.qi.asu $                                                                 */
/************************************************************************/

#include <cassert>
#include "GraphElements.h"
#include "Graph.h"
#include "DijkstraShortestPathAlg.h"

using namespace std;


int main(int argc,char *argv[]) {
    assert(argc==2 || argc==4);
    Graph* my_graph_pt = new Graph(argv[1]);
    DijkstraShortestPathAlg shortest_path_alg(my_graph_pt);
    int source,target;
    if(argc==2){
      source=0;
      target=1;
    }
    else{
      source=atoi(argv[2]);
      target=atoi(argv[3]);
    }
    BasePath* result = shortest_path_alg.get_shortest_path(
        my_graph_pt->get_vertex(source),
        my_graph_pt->get_vertex(target)
    );
    result->PrintOut(cout);
}
