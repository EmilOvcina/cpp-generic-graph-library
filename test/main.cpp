
// #include "graph/adjacency_list.hpp"
#include "../src/graph/adjacency_list.hpp"
#include "../src/graph/concepts.hpp"
#include "../src/graph/depth_first_search.hpp"
#include "../src/graph/topological_sort.hpp"
#include <iostream>

using namespace graph;

void testTopoSort(int graphNr);

int main() {
    /**
     * @brief Test topoSort function, only 1,2,3 is useful.
     */
    std::cout << "Graph1:\n";
    testTopoSort(1);
    std::cout << "\nGraph2:\n";
    testTopoSort(2);
    std::cout << "\nGraph3:\n";
    testTopoSort(3);
    std::cout << "\n";


    /**
     * @brief Checks if the concepts in concepts.hpp are satisfied
     */
    static_assert(Graph<AdjacencyList<tags::Directed>>);
    static_assert(Graph<AdjacencyList<tags::Bidirectional>>);

    static_assert(VertexListGraph<AdjacencyList<tags::Directed>>);
    static_assert(VertexListGraph<AdjacencyList<tags::Bidirectional>>);

    static_assert(EdgeListGraph<AdjacencyList<tags::Directed>>);
    static_assert(EdgeListGraph<AdjacencyList<tags::Bidirectional>>);

    static_assert(IncidenceGraph<AdjacencyList<tags::Directed>>);
    static_assert(IncidenceGraph<AdjacencyList<tags::Bidirectional>>);
    static_assert(BidirectionalGraph<AdjacencyList<tags::Bidirectional>>);

    static_assert(MutableGraph<AdjacencyList<tags::Directed>>);
    static_assert(MutableGraph<AdjacencyList<tags::Bidirectional>>);

    static_assert(PropertyGraph<AdjacencyList<tags::Directed, int, int>>);
    static_assert(PropertyGraph<AdjacencyList<tags::Bidirectional, int, int>>);

    static_assert(MutablePropertyGraph<AdjacencyList<tags::Directed, int, int>>);
    static_assert(MutablePropertyGraph<AdjacencyList<tags::Bidirectional, int, int>>);


    /**
     * @brief testing other functionality of the graph
     */
    graph::AdjacencyList<graph::tags::Bidirectional, int, int> g;

    auto v = addVertex(g);
    auto u = addVertex(43, g);
    auto u2 = addVertex(g);
    addVertex(g);
    addVertex(g);

    auto e1 = addEdge(v,u, 123,g);
    auto w2 = addEdge(u,v, 321,g);
    addEdge(u2,v,g);
    auto oer = outEdges(v, g);

    auto ier = inEdges(v, g);

    std::cout << g[e1] << std::endl;
    std::cout << g[w2] << std::endl;

    std::cout << g[u] << std::endl;

    for(auto e = ier.begin(); e != ier.end(); e++) {
        std::cout << (*e).src << " - " << (*e).tar << "\n";
    }
    std::cout << "\n" << std::endl;

    for(auto e = oer.begin(); e != oer.end(); e++) {
        std::cout << (*e).src << " - " << (*e).tar << "\n";
    }

    std::cout << std::endl;
}


/**
 * @brief Tests graph::topoSort() function using three different graphs
 * @param num int where only 1, 2 or 3 is allowed.
 */
void testTopoSort(int num) {
    using Graph = AdjacencyList<graph::tags::Directed>;
    Graph g;

    if(num == 1) {
        //* Graph1
        auto v0 = addVertex(g);
        addVertex(g);
        auto v2 = addVertex(g);
        auto v3 = addVertex(g);
        auto v4 = addVertex(g);
        auto v5 = addVertex(g);
        auto v6 = addVertex(g);
        auto v7 = addVertex(g);

        addEdge(v0, v3, g);
        addEdge(v3, v5, g);
        addEdge(v5, v7, g);
        addEdge(v2, v4, g);
        addEdge(v4, v6, g);
        addEdge(v6, v7, g);
    }

    else if (num == 2) {
        //* Graph2
        auto v0 = addVertex(g);
        auto v1 = addVertex(g);
        auto v2 = addVertex(g);
        auto v3 = addVertex(g);
        auto v4 = addVertex(g);
        auto v5 = addVertex(g);
        auto v6 = addVertex(g);
        auto v7 = addVertex(g);

        addEdge(v0, v2, g);
        addEdge(v1, v2, g);
        addEdge(v1, v3, g);
        addEdge(v1, v6, g);
        addEdge(v2, v4, g);
        addEdge(v2, v5, g);
        addEdge(v3, v7, g);
        addEdge(v4, v7, g);
    }

    else if (num == 3) {
        //* Graph3
        auto v0 = addVertex(g);
        auto v1 = addVertex(g);
        auto v2 = addVertex(g);
        auto v3 = addVertex(g);
        addVertex(g);
        auto v5 = addVertex(g);
        auto v6 = addVertex(g);
        addVertex(g);

        addEdge(v0, v3, g);
        addEdge(v1, v2, g);
        addEdge(v1, v6, g);
        addEdge(v5, v6, g);
    }


    std::vector<Graph::VertexDescriptor> vs;
    vs.reserve(numVertices(g));
    graph::topoSort(g, std::back_inserter(vs));
    std::reverse(vs.begin(), vs.end());
    //Print result of topo_sort
    for(auto i = vs.begin(); i != vs.end(); i++)
        std::cout << *i << std::endl;
}