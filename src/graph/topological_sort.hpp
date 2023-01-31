#ifndef GRAPH_TOPOLOGICAL_SORT_HPP
#define GRAPH_TOPOLOGICAL_SORT_HPP

#include "depth_first_search.hpp"

namespace graph {
namespace detail {

/**
 * @brief Derived from the DFSNullVisitor class.
 * @tparam OIter output iterator to write to when a vertex's visit is finished.
 */
template<typename OIter>
struct TopoVisitor : DFSNullVisitor {
	TopoVisitor(OIter iter) : iter(iter) {}

	/**
	 * @brief overrides the finishVertex function from DFSNullVisitor
	 * @tparam V Vertex Descriptor
	 * @tparam G Graph typename
	 * @param v Vertex to add to the output iterator.
	 * @param g graph reference.
	 */
	template<typename V, typename G>
	void finishVertex(const V& v, const G& g) {
		*iter = v;
	}

private:
	OIter iter;
};

} // namespace detail

/**
 * @brief Runs dfs on a graph using the TopoVisitor class and
 * 			uses the output iterator to save the reverse order of the sort.
 * @tparam Graph AdjacencyList or AdjacencyMatrix
 * @tparam OutputIterator
 * @param g graph to run the sort on.
 * @param oIter output iterator to save the reverse result in.
 */
template<typename Graph, typename OutputIterator>
void topoSort(const Graph &g, OutputIterator oIter) {
	dfs(g, graph::detail::TopoVisitor (oIter));
}

} // namespace graph

#endif // GRAPH_TOPOLOGICAL_SORT_HPP
