#ifndef GRAPH_ADJACENCY_LIST_HPP
#define GRAPH_ADJACENCY_LIST_HPP

#include "tags.hpp"
#include "traits.hpp"
#include "properties.hpp"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

#include <cassert>
#include <list>
#include <vector>

namespace graph {

template<typename DirectedCategoryT,
         typename VertexPropT = NoProp,
         typename EdgePropT = NoProp>
struct AdjacencyList {
private:
	/**
	 * @brief object to hold an index to the stored edge in the global storedEdgeList.
	 * 			Represents an out edge of a vertex.
	 */
	struct OutEdge {
		OutEdge (std::size_t storedEdgeIdx) : storedEdgeIdx(storedEdgeIdx) {}
		const std::size_t storedEdgeIdx;
	};

	/**
	 * @brief object to hold an index to the stored edge in the global storedEdgeList.
	 * 			Represents an in edge of a vertex and is only used in Bidirectional graphs
	 */
	struct InEdge {
		InEdge (std::size_t storedEdgeIdx) : storedEdgeIdx(storedEdgeIdx) {}
		std::size_t storedEdgeIdx;
	};

	using InEdgeList = std::vector<InEdge>;
	using OutEdgeList = std::vector<OutEdge>;

	/**
	 * @brief StoredVertex object used when DirectedCategoryT is not bidirectional and VertexPropT is NoProp
	 * 		Holds a list of outEdge objects
	 */
	struct StoredVertexSimple {
		OutEdgeList eOut;
	};

	/**
	 * @brief StoredVertex object used when DirectedCategoryT is not bidirectional and VertexPropT is not NoProp
	 * 		Holds a list of outEdge objects and a VertexProp
	 */
	struct StoredVertexSimpleProp {
		OutEdgeList eOut;
		StoredVertexSimpleProp() = default;
		StoredVertexSimpleProp(VertexPropT vp) : vProp(vp){}
		VertexPropT vProp;
	};

	/**
	 * @brief StoredVertex object used when DirectedCategoryT is bidirectional and VertexPropT is NoProp
	 * 		Holds a list of outEdge objects and a list of inEdge object
	 */
	struct StoredVertexComplex {
		OutEdgeList eOut;
		InEdgeList eIn;
	};

	/**
	 * @brief StoredVertex object used when DirectedCategoryT is bidirectional and VertexPropT is not NoProp
	 * 		Holds a list of outEdge objects and a list of inEdge object
	 * 		Also holds a vertexProp
	 */
	struct StoredVertexComplexProp {
		OutEdgeList eOut;
		InEdgeList eIn;
		StoredVertexComplexProp() = default;
		StoredVertexComplexProp(VertexPropT vp) : vProp(vp) {}
		VertexPropT vProp;
	};

	/**
	 * @brief three conditionals used to determine which vertex object to use.
	 * 			First two conditionsals checks if VertexPropT is NoProp
	 * 			The last conditional checks if the DirectedCategoryT is directed
	 * 			The result is used in the StoredVertex which is used in all other member functions.
	 */
	typedef std::conditional<std::is_same<VertexPropT, graph::NoProp>::value, StoredVertexSimple, StoredVertexSimpleProp>::type VertexSimplePropType;
	typedef std::conditional<std::is_same<VertexPropT, graph::NoProp>::value, StoredVertexComplex, StoredVertexComplexProp>::type VertexComplexPropType;
	typedef std::conditional<std::is_same<DirectedCategoryT, graph::tags::Directed>::value, VertexSimplePropType, VertexComplexPropType>::type VertexType;
	using StoredVertex = VertexType;


	/**
	 * @brief Stored Edge object where EdgePropT is NoProp
	 * 			Holds a source and target number corresponding to the source and target vertex.
	 */
	struct StoredEdgeNoProp {
		StoredEdgeNoProp() = default;
		StoredEdgeNoProp(std::size_t src, std::size_t tar) : src(src), tar(tar) {}
		std::size_t src, tar;
	};

	/**
	 * @brief Stored Edge object where EdgePropT is not NoProp
	 * 			Holds a source and target number corresponding to the source and target vertex.
	 * 			Also holds EdgePropT along with the edge
	 */
	struct StoredEdgeProp {
		StoredEdgeProp() = default;
		StoredEdgeProp(std::size_t src, std::size_t tar) : src(src), tar(tar) {}
		StoredEdgeProp(std::size_t src, std::size_t tar, EdgePropT ep) : eProp(ep), src(src), tar(tar) {}

		EdgePropT eProp;
		std::size_t src, tar;
	};

	/**
	 * @brief conditional determining which StoredEdge object to use.
	 * 			If the EdgePropT type parameter is NoProp it uses the StoredEdgeNoProp object, StoredEdgeProp otherwise.
	 * 			The result is used in the StoredEdge which is used in all member functions
	 */
	typedef std::conditional<std::is_same<EdgePropT, graph::NoProp>::value, StoredEdgeNoProp, StoredEdgeProp>::type StoredEdgePropType;
	using StoredEdge = StoredEdgePropType;

	using VList = std::vector<StoredVertex>;
	using EList = std::vector<StoredEdge>;
public: // Graph
	using DirectedCategory = DirectedCategoryT;
	using VertexDescriptor = std::size_t;

	struct EdgeDescriptor {
		EdgeDescriptor() = default;
		EdgeDescriptor(std::size_t src, std::size_t tar,
		               std::size_t storedEdgeIdx)
			: src(src), tar(tar), storedEdgeIdx(storedEdgeIdx) {}
	public:
		std::size_t src, tar;
		std::size_t storedEdgeIdx;
	public:
		friend bool operator==(const EdgeDescriptor &a,
		                       const EdgeDescriptor &b) {
			return a.storedEdgeIdx == b.storedEdgeIdx;
		}
	};
public: // PropertyGraph
	using VertexProp = VertexPropT;
	using EdgeProp = EdgePropT;
public: // VertexListGraph
	struct VertexRange {
		// the iterator is simply a counter that returns its value when
		// dereferenced
		using iterator = boost::counting_iterator<VertexDescriptor>;
	public:
		VertexRange(std::size_t n) : n(n) {}
		iterator begin() const { return iterator(0); }
		iterator end()   const { return iterator(n); }
	private:
		std::size_t n;
	};
public: // EdgeListGraph
	struct EdgeRange {
		// We want to adapt the edge list,
		// so it dereferences to EdgeDescriptor instead of StoredEdge
		using EListIterator = typename EList::const_iterator;

		struct iterator : boost::iterator_adaptor<
				iterator, // because we use CRTP
				EListIterator, // the iterator we adapt
				// we want to convert the StoredEdge into an EdgeDescriptor:
				EdgeDescriptor,
				// we can use RA as the underlying iterator supports it:
				std::random_access_iterator_tag,
				// when we dereference we return by value, not by reference
				EdgeDescriptor
			> {
			using Base = boost::iterator_adaptor<
				iterator, EListIterator, EdgeDescriptor,
				std::random_access_iterator_tag, EdgeDescriptor>;
		public:
			iterator() = default;
			iterator(EListIterator i, EListIterator first) : Base(i), first(first) {}
		private:
			// let the Boost machinery use our methods:
			friend class boost::iterator_core_access;

			EdgeDescriptor dereference() const {
				// get our current position stored in the
				// boost::iterator_adaptor base class
				const EListIterator &i = this->base_reference();
				return EdgeDescriptor{i->src, i->tar,
					static_cast<std::size_t>(i - first)};
			}
		private:
			EListIterator first;
		};
	public:
		EdgeRange(const AdjacencyList &g) : g(&g) {}

		iterator begin() const {
			return iterator(g->eList.begin(), g->eList.begin());
		}

		iterator end() const {
			return iterator(g->eList.end(), g->eList.begin());
		}
	private:
		const AdjacencyList *g;
	};
public: // IncidenceGraph Associated types
	struct OutEdgeRange {
		using OEListIterator = typename OutEdgeList::const_iterator;
		struct iterator : boost::iterator_adaptor<
				iterator, // because we use CRTP
				OEListIterator, // the iterator we adapt
				// we want to convert the OutEdge into an EdgeDescriptor:
				EdgeDescriptor,
				std::bidirectional_iterator_tag,
				// when we dereference we return by value, not by reference
				EdgeDescriptor
			> {
			using Base = boost::iterator_adaptor<
				iterator, OEListIterator, EdgeDescriptor,
				std::bidirectional_iterator_tag, EdgeDescriptor>;
		public:
			iterator() = default;
			iterator(OEListIterator i, OEListIterator first, const AdjacencyList* g) : Base(i), first(first), g(g) {}
		private:
			// let the Boost machinery use our methods:
			friend class boost::iterator_core_access;

			EdgeDescriptor dereference() const {
				//Get current position from boost iterator and use the storedEdgeIdx member
				// 		variable to access the edge in the StoredEdge list from g.
				//		return the edge descriptor matching the StoredEdge found on the index.
				const OEListIterator& i = this->base_reference();
				StoredEdge e = g->eList[i->storedEdgeIdx];
				return EdgeDescriptor(e.src, e.tar, i->storedEdgeIdx);
			}
		private:
			OEListIterator first;
			const AdjacencyList* g;
		};
	public:
		OutEdgeRange(const VertexDescriptor& v, const AdjacencyList& g) : v(v), g(&g) {}

		iterator begin() const {
			auto idx = getIndex(v, *g);
			return iterator(g->vList[idx].eOut.begin(), g->vList[idx].eOut.begin(), g);
		}

		iterator end() const {
			auto idx = getIndex(v, *g);
			return iterator(g->vList[idx].eOut.end(), g->vList[idx].eOut.begin(), g);
		}
	private:
		const VertexDescriptor& v;
		const AdjacencyList* g;
	};

public: // BidirectionalGraph
	struct InEdgeRange {
		// InEdgeList should not be usable for a directed graph
		static_assert(!std::is_same<DirectedCategoryT, graph::tags::Directed>::value);

		using IEListIterator = typename InEdgeList::const_iterator;

		struct iterator : boost::iterator_adaptor<
				iterator, // because we use CRTP
				IEListIterator, // the iterator we adapt
				// we want to convert the InEdge into an EdgeDescriptor:
				EdgeDescriptor,
				std::bidirectional_iterator_tag,
				// when we dereference we return by value, not by reference
				EdgeDescriptor
			> {
			using Base = boost::iterator_adaptor<
				iterator, IEListIterator, EdgeDescriptor,
				std::bidirectional_iterator_tag, EdgeDescriptor>;
		public:
			iterator() = default;
			iterator(IEListIterator i, IEListIterator first, const AdjacencyList* g) : Base(i), first(first), g(g) {}
		private:
			// let the Boost machinery use our methods:
			friend class boost::iterator_core_access;

			EdgeDescriptor dereference() const {
				//Get current position from boost iterator and use the storedEdgeIdx member
				// 		variable to access the edge in the StoredEdge list from g.
				//		return the edge descriptor matching the StoredEdge found on the index.
				const IEListIterator& i = this->base_reference();
				StoredEdge e = g->eList[i->storedEdgeIdx];
				return EdgeDescriptor(e.src, e.tar, i->storedEdgeIdx);
			}
		private:
			IEListIterator first;
			const AdjacencyList* g;
		};
	public:
		InEdgeRange(const VertexDescriptor &v, const AdjacencyList& g) : v(v), g(&g) {}

		iterator begin() const {
			auto idx = getIndex(v, *g);
			return iterator(g->vList[idx].eIn.begin(), g->vList[idx].eIn.begin(), g);
		}

		iterator end() const {
			auto idx = getIndex(v, *g);
			return iterator(g->vList[idx].eIn.end(), g->vList[idx].eIn.begin(), g);
		}
	private:
		const VertexDescriptor& v;
		const AdjacencyList* g;
	};
public:
	AdjacencyList() = default;
	AdjacencyList(std::size_t n) : vList(n) {}

	//* Copy constructor
	AdjacencyList(const AdjacencyList& a) : vList(a.vList), eList(a.eList) {}
private:
	VList vList;
	EList eList;
public: // Graph
	friend VertexDescriptor source(EdgeDescriptor e, const AdjacencyList &g) {
		return e.src;
	}

	friend VertexDescriptor target(EdgeDescriptor e, const AdjacencyList &g) {
		return e.tar;
	}
public: // VertexListGraph
	friend std::size_t numVertices(const AdjacencyList &g) {
		return g.vList.size();
	}

	friend VertexRange vertices(const AdjacencyList &g) {
		return VertexRange(numVertices(g));
	}
public: // EdgeListGraph
	friend std::size_t numEdges(const AdjacencyList &g) {
		return g.eList.size();
	}

	friend EdgeRange edges(const AdjacencyList &g) {
		return EdgeRange(g);
	}
public: // Other
	friend std::size_t getIndex(VertexDescriptor v, const AdjacencyList &g) {
		return v;
	}
public: // IncidenceGraph Valid Expressions
	/**
	 * @param v vertex descriptor for the vertex which to return its out edges
	 * @param g graph which the vertex belongs to
	 * @return OutEdgeRange of the outEdges belonging to the vertex.
	 */
	friend OutEdgeRange outEdges(VertexDescriptor v, const AdjacencyList& g) {
		return OutEdgeRange(v, g);
	}

	/**
	 * @param v vertex descriptor for the vertex which to return its out degree
	 * @param g graph which the vertex belongs to
	 * @return std::size_t the amount of edges in the outEdge list
	 */
	friend std::size_t outDegree(VertexDescriptor v, const AdjacencyList& g) {
		auto oe = outEdges(v,g);
		return std::distance(oe.begin(), oe.end());
	}

public: // BidirectionalGraph
	/**
	 * @brief The DirectedCategory of the graph must be Bidirectional
	 *
	 * @param v vertex descriptor for the vertex which to return its in edges
	 * @param g graph which the vertex belongs to
	 * @return InEdgeRange of the inEdges belonging to the vertex.
	 */
	friend InEdgeRange inEdges(VertexDescriptor v, const AdjacencyList& g)
	requires (std::same_as<DirectedCategoryT, graph::tags::Bidirectional> ) {
		return InEdgeRange(v, g);
	}

	/**
	 * @brief The DirectedCategory of the graph must be Bidirectional
	 *
	 * @param v vertex descriptor for the vertex which to return its in degree
	 * @param g graph which the vertex belongs to
	 * @return std::size_t the amount of edges in the inEdge list
	 */
	friend std::size_t inDegree(VertexDescriptor v, const AdjacencyList& g)
	requires (std::same_as<DirectedCategoryT, graph::tags::Bidirectional> ) {
		auto ie = inEdges(v,g);
		return std::distance(ie.begin(), ie.end());
	}

public: // MutableGraph
	/**
	 * @brief For the function to be valid the VertexProp type parameter
	 * 			must be default constructible
	 *
	 * @param g graph to add the vertex to.
	 * @return VertexDescriptor for the newly added vertex
	 */
	friend VertexDescriptor addVertex(AdjacencyList& g)
	requires( std::is_default_constructible<VertexProp>::value ) {
		VertexDescriptor newV = numVertices(g);
		g.vList.push_back(StoredVertex());
		return newV;
	}

	/**
	 * @brief The DirectedCategoryT must be directed type and the EdgePropT type parameter
	 * 			must be default constructible
	 *
	 * @param v vertex descriptor of the source vertex
	 * @param u vertex descriptor of the target vertex
	 * @param g graph to add the edge to.
	 * @return EdgeDescriptor for the newly added edge
	 */
	friend EdgeDescriptor addEdge(VertexDescriptor v, VertexDescriptor u,  AdjacencyList& g)
	requires(std::same_as<DirectedCategory, graph::tags::Directed>
	&& std::is_default_constructible<EdgeProp>::value) {
		std::size_t index = numEdges(g);
		g.eList.push_back(StoredEdge(v,u));
		EdgeDescriptor newEdge (v, u, index);

		// Add outedge to `v`
		OutEdge oe (index);
		auto vertexIndex = getIndex(v, g);
		g.vList[vertexIndex].eOut.push_back(oe);
		return newEdge;
	}

	/**
	 * @brief The DirectedCategoryT must be bidirectional type and the EdgePropT type parameter
	 * 			must be default constructible
	 *
	 * @param v vertex descriptor of the source vertex
	 * @param u vertex descriptor of the target vertex
	 * @param g graph to add the edge to.
	 * @return EdgeDescriptor for the newly added edge
	 */
	friend EdgeDescriptor addEdge(VertexDescriptor v, VertexDescriptor u,  AdjacencyList& g)
	requires(std::same_as<DirectedCategory, graph::tags::Bidirectional>
	&& std::is_default_constructible<EdgeProp>::value) {
		std::size_t index = numEdges(g);
		g.eList.push_back(StoredEdge(v,u));
		EdgeDescriptor newEdge (v, u, index);

		// Add outedge to `v`
		OutEdge oe (index);
		auto vertexIndex_v = getIndex(v, g);
		g.vList[vertexIndex_v].eOut.push_back(oe);

		// Add inedge to `u`
		InEdge ie (index);
		auto vertexIndex_u = getIndex(u, g);
		g.vList[vertexIndex_u].eIn.push_back(ie);

		return newEdge;
	}

public: // MutablePropertyGraph
	/**
	 * @brief For the function to be valid the VertexProp type parameter
	 * 			must be move assignable and must not be NoProp type
	 *
	 * @param vp VertexProp rvalue reference to move and store with the vertex.
	 * @param g graph to add the vertex to.
	 * @return VertexDescriptor for the newly added vertex
	 */
	friend VertexDescriptor addVertex(VertexProp&& vp, AdjacencyList& g)
	requires( !std::same_as<VertexProp, graph::NoProp> &&
	std::movable<VertexProp>) {
		VertexDescriptor newV = numVertices(g);
		g.vList.push_back(StoredVertex(vp));
		return newV;
	}

	/**
	 * @brief The DirectedCategoryT must be directed type, the EdgePropT type parameter
	 * 			must be move assignable and the EdgeProp type must not be NoProp
	 *
	 * @param v vertex descriptor of the source vertex
	 * @param u vertex descriptor of the target vertex
	 * @param ep EdgeProp rvalue reference to movr and store with the edge.
	 * @param g graph to add the edge to.
	 * @return EdgeDescriptor for the newly added edge
	 */
	friend EdgeDescriptor addEdge(VertexDescriptor v, VertexDescriptor u, EdgeProp&& ep, AdjacencyList& g)
	requires (!std::same_as<EdgeProp, graph::NoProp> &&
	std::movable<EdgeProp> &&
	std::same_as<DirectedCategory, graph::tags::Directed>) {
		std::size_t index = numEdges(g);
		g.eList.push_back(StoredEdge(v,u,ep));
		EdgeDescriptor newEdge (v, u, index);

		// Add outedge to `v`
		OutEdge oe (index);
		auto vertexIndex = getIndex(v, g);
		g.vList[vertexIndex].eOut.push_back(oe);
		return newEdge;
	}

	/**
	 * @brief The DirectedCategoryT must be bidirectional type, the EdgePropT type parameter
	 * 			must be move assignable and the EdgeProp type must not be NoProp
	 *
	 * @param v vertex descriptor of the source vertex
	 * @param u vertex descriptor of the target vertex
	 * @param ep EdgeProp rvalue reference to movr and store with the edge.
	 * @param g graph to add the edge to.
	 * @return EdgeDescriptor for the newly added edge
	 */
	friend EdgeDescriptor addEdge(VertexDescriptor v, VertexDescriptor u, EdgeProp&& ep, AdjacencyList& g)
	requires (!std::same_as<EdgeProp, graph::NoProp> &&
	std::movable<EdgeProp> &&
	std::same_as<DirectedCategory, graph::tags::Bidirectional>) {
		std::size_t index = numEdges(g);
		g.eList.push_back(StoredEdge(v,u,ep));
		EdgeDescriptor newEdge (v, u, index);

		// Add outedge to `v`
		OutEdge oe (index);
		auto vertexIndex_v = getIndex(v, g);
		g.vList[vertexIndex_v].eOut.push_back(oe);

		// Add inedge to `u`
		InEdge ie (index);
		auto vertexIndex_u = getIndex(u, g);
		g.vList[vertexIndex_u].eIn.push_back(ie);

		return newEdge;
	}

public: // PropertyGraph

	/**
	 * @brief [] operator overloading for vertices. Vertex prop must not be NoProp
	 *
	 * @return VertexProp& of the vertexprop stored at the vertex
	 */
	VertexProp& operator[] (VertexDescriptor v) requires (!std::same_as<VertexProp, graph::NoProp>) {
		return vList[getIndex(v, *this)].vProp;
	}

	/**
	 * @brief [] operator overloading for vertices. Vertex prop must not be NoProp
	 *
	 * @return const VertexProp& of the vertexprop stored at the vertex
	 */
	const VertexProp& operator[] (VertexDescriptor v) const requires (!std::same_as<VertexProp, graph::NoProp>) {
		return vList[getIndex(v, *this)].vProp;
	}

	/**
	 * @brief [] operator overloading for edges. Edge prop must not be NoProp
	 *
	 * @return EdgeProp& of the edgeprop stored at the edge
	 */
	EdgeProp& operator[] (EdgeDescriptor e) requires (!std::same_as<EdgeProp, graph::NoProp>) {
		return eList[e.storedEdgeIdx].eProp;
	}

	/**
	 * @brief [] operator overloading for edges. Edge prop must not be NoProp
	 *
	 * @return const EdgeProp& of the edgeprop stored at the edge
	 */
	const EdgeProp& operator[] (EdgeDescriptor e) const requires (!std::same_as<EdgeProp, graph::NoProp>) {
		return eList[e.storedEdgeIdx].eProp;
	}
};

} // namespace graph

#endif // GRAPH_ADJACENCY_LIST_HPP
