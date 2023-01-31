#ifndef GRAPH_DEPTH_FIRST_SEARCH_HPP
#define GRAPH_DEPTH_FIRST_SEARCH_HPP

#include "traits.hpp"
#include <vector>

namespace graph {

struct DFSNullVisitor {
	template<typename G, typename V>
	void initVertex(const V&, const G&) { }

	template<typename G, typename V>
	void startVertex(const V&, const G&) { }

	template<typename G, typename V>
	void discoverVertex(const V&, const G&) { }

	template<typename G, typename V>
	void finishVertex(const V&, const G&) { }

	template<typename G, typename E>
	void examineEdge(const E&, const G&) { }

	template<typename G, typename E>
	void treeEdge(const E&, const G&) { }

	template<typename G, typename E>
	void backEdge(const E&, const G&) { }

	template<typename G, typename E>
	void forwardOrCrossEdge(const E&, const G&) { }

	template<typename G, typename E>
	void finishEdge(const E&, const G&) { }
};

namespace detail {

enum struct DFSColour {
	White, Grey, Black
};

/**
 * @brief DFS visit algorithm following the pseudo-code from the book Introduction to Algorithms and the Boost Graph library.
 * @tparam Graph graph type AdjacencyList or AdjacencyMatrix
 * @tparam Visitor type, DFSNullVisitor or TopoVisitor
 * @param g graph to perform DFS on
 * @param visitor object descriping the behavior when traversing.
 * @param u VertexDescriptor for the vertex to visit.
 * @param colour array.
 */
template<typename Graph, typename Visitor>
void dfsVisit(const Graph &g, Visitor visitor, typename Traits<Graph>::VertexDescriptor u,
		std::vector<DFSColour> &colour) {
	visitor.discoverVertex(u, g);
	colour[u] = graph::detail::DFSColour::Grey;
	for (auto e = outEdges(u, g).begin(); e != outEdges(u,g).end(); e++) {
		auto v = (*e).tar;
		visitor.examineEdge(*e, g);
		if(colour[v] == graph::detail::DFSColour::White) {
			visitor.treeEdge(*e, g);
			dfsVisit(g, visitor, v, colour);
		} else if (colour[v] == graph::detail::DFSColour::Grey)
			visitor.backEdge(*e, g);
		else
			visitor.forwardOrCrossEdge(*e, g);
		visitor.finishEdge(*e, g);
	}
	colour[u] = graph::detail::DFSColour::Black;
	visitor.finishVertex(u, g);
}

} // namespace detail

/**
 * @brief DFS algorithm following the pseudo-code from the book Introduction to Algorithms and the Boost Graph library.
 * @tparam Graph graph type AdjacencyList or AdjacencyMatrix
 * @tparam Visitor type, DFSNullVisitor or TopoVisitor
 * @param g graph to perform DFS on
 * @param visitor object descriping the behavior when traversing.
 */
template<typename Graph, typename Visitor>
void dfs(const Graph &g, Visitor visitor) {
	std::vector<graph::detail::DFSColour> colour_vec (numVertices(g));
	for (auto v = vertices(g).begin(); v != vertices(g).end(); v++) {
		colour_vec[*v] = graph::detail::DFSColour::White;
		visitor.initVertex(v, g);
	}
	for (auto v = vertices(g).begin(); v != vertices(g).end(); v++) {
		if(colour_vec[*v] == graph::detail::DFSColour::White) {
			visitor.startVertex(v, g);
			graph::detail::dfsVisit(g, visitor, *v, colour_vec);
		}
	}
}

} // namespace graph

#endif // GRAPH_DEPTH_FIRST_SEARCH_HPP
