/**
 * @file
 * @brief Definition of a simple, undirected, unlabelled Graph class.
 *
 * @copyright Copyright (c) 2015-2017 Sean Chester
 * <br />
 * This file is part of the GraphAnon suite.
 * GraphAnon, version 2.0, is distributed freely under the *MIT License*:
 * <br />
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * <br />
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * <br />
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef UNLABELLED_GRAPH_H_
#define UNLABELLED_GRAPH_H_

#include <cstdint>	/* For uint32_t */
#include <fstream>	/* for std::ofstream */

/* STL libraries in use */
#include <vector>
#include <unordered_set>
#include <map>

namespace graphAnon
{
	/** The supported file formats for ascii representations of undirected graphs. */
	enum class FileFormat {
		/**
		 * The file format is an adjacency list. 
		 * The first row gives white-space separated meta-data about 
		 * the UnlabelledGraph, namely the number of vertices (e.g., "6" is a 6-node 
		 * graph). Each subsequent line corresponds to one vertex with a variable-length 
		 * list of space-separated integers indicating the node ids of all neighbours 
		 * (e.g., "1 2 5 9" indicates a vertex with who is connected (only) to vertices 
		 * 1, 2, 5, and 9. Note that there should be exactly n+1 lines in the file, 
		 * the first line should contain exactly one number, and every subsequent line 
		 * can contain zero or more point ids.
		 */
		adjacencyList, 
		
		/**
		 * The file format is a vertex-labelled adjacency list. 
		 * The first row gives white-space
		 * separated meta-data about the LabelledGraph, namely the number of vertices
		 * and then the number of distinct labels (e.g., "6 2" is a 6-node graph
		 * that has a binary label alphabet).
		 * Each subsequent line corresponds to a vertex. The first value is the
		 * label of the vertex, and the remaining variable-length space-separated
		 * integers are the node ids of all neighbours (e.g., "1 2 5 9" indicates a
		 * vertex with label 1 who is connected (only) to vertices 2, 5, and 9.
		 * Note that there should beexactly n+1 lines in the file, the first line
		 * should contain exactly two numbers, and every subsequent line must
		 * contain at least one number.
		 */		
		adjacencyListVertexLabelled,
		
		/**
		 * Input file is a list of edges. The first line is a single number indicating 
		 * the number of vertices in the graph. Each subsequent line gives two white-space 
		 * separated integers representing the origin and destination of an edge, respectively.
		 * E.g., Figure 1 of @cite waldo would be represented as follows:
		 * <pre>4
		 * 0 1 
		 * 1 0 
		 * 1 2 
		 * 1 3 
		 * 2 1 
		 * 2 3 
		 * 3 1 
		 * 3 2</pre>
		 */		
		edgeList
	};
}


/**
 * A HopPlot is a histogram of path lengths in a graph.
 * It maps from each integer i = 1,... the number of 
 * vertex pairs in the graph that are reachable with a 
 * shortest path of exactly i hops.
 */
typedef std::map< uint32_t, uint64_t > HopPlot;

/**
 * A DegreeSequence is a list of the degrees for each of the n_ 
 * vertices in a graph, sorted in descending order.
 */
typedef std::vector< std::pair< uint32_t, uint32_t > > DegreeSequence;

/**
 * A NeighbourList is a set of neighbours for a given vertex. 
 * If vertex i is in the list, then the vertex to whom this 
 * NeighbourList is associated is connected to vertex i.
 */
typedef std::unordered_set < uint32_t > NeighbourList;

/**
 * An AdjacencyList is a format for representing the connectivity of 
 * a graph. It is a list of length n_, where the i'th element is the list 
 * of neighbours for the i'th vertex.
 */
typedef std::vector< NeighbourList > AdjacencyList;

/**
 * @brief A simple, undirected, unlabelled graph with no self-loops that is
 * equipped with methods for identity disclosure protection.
 */
class UnlabelledGraph {
public:

	/**
	 * Constructs an UnlabelledGraph object from a file
	 * @param filename The path to the input file containing the graph
	 * @param file_type Indicates the format of the input file.
	 * @post Constructs a new UnlabelledGraph object
	 * @warning Does minimal error-checking. If the file format is
	 * invalid or filename is an incorrect path, then the behaviour
	 * of this constructor is undefined.
	 * @see <a href="../../workloads/snam_example.adjList">An example file</a>
	 * consisting of the example UnlabelledGraph from Figure 1 of @cite waldo ,
	 * represented in the adjacency list format.
	 */
	UnlabelledGraph( const std::string filename, 
		const graphAnon::FileFormat format = graphAnon::FileFormat::adjacencyList );


	/**
	 * Constructs an unlabelled graph with n isloated vertices.
	 * @param num_vertices The number of vertices in the graph.
	 * @post Constructs a new UnlabelledGraph object
	 */
	UnlabelledGraph( const uint32_t num_vertices );
	
	/**
	 * Empty constructor to create an UnlabelledGraph with no vertices 
	 * and no edges.
	 */
	UnlabelledGraph();

	/**
	 * Destroys the UnlabelledGraph.
	 */
	virtual ~UnlabelledGraph();

	/**
	 * Initializes empty UnlabelledGraph data structures: should be called
	 * by all overloaded constructors once n_ and l_ are set.
	 */
	void init();
	
	/**
	 * Accessor method to retrieve the number of vertices in the graph, |V|.
	 */
	uint32_t num_vertices() const;
	
	/**
	 * Accessor method to retrieve the number of edges in the graph, |E|.
	 */
	uint32_t num_edges() const;

	/**
	 * Populates the UnlabelledGraph with num_edges undirected edges, 
	 * randomly chosen with uniform distribution.
	 * @param num_edges The number of edges to insert into the graph.
	 * @returns false if num_edges cannot be inserted; true otherwise.
	 * @post The graph contains num_edges more edges than it had before the
	 * method was invoked, unless it is impossible to add num_edges more edges
	 * to the graph (then no edges are added).
	 *
	 * This method iterates , picking two vertices u,v uniformly at random.
	 * If the edge (u,v) does not yet exist, it is added. Once num_edges
	 * successful edge additions have taken place, the routine terminates. If
	 * num_edges > n * (n - 1) - the number of edges already in the graph, the
	 * method returns false (failure).
	 */
	bool populate_uniformly( const uint32_t num_edges );

	/**
	 * Retrieves the percentage of possible edges tha are present in the graph.
	 * @return If E is the edge set and V is the vertex set, the return value is
	 * |E| / |V| / ( |V| - 1 ). Will also return 0 if |V| = 0.
	 */
	float get_occupancy() const;
	
	/**
	 * Calculates the clustering coefficient of the graph.
	 */
	float clustering_coefficient() const;
	
	/**
	 * Calculates the harmonic mean of the graph from a hop plot.
	 * @param hop_plot The hop plot generated for this graph
	 * @returns The harmonic mean of the graph.
	 * @pre Assumes that hop_plot has been populated with a call to hop_plot()
	 */
	float harmonic_mean( HopPlot const& hop_plot ) const;
	
	/**
	 * Calculates the average path length of the graph from a hop plot.
	 * @tparam include_self_paths A boolean suggesting whether paths of 
	 * the form (u,u) with length 0 should be included; if so, the denominator 
	 * of the APL expression will increase. Note that this option is adopted 
	 * inconsistently in @cite waldo : the football dataset is calculated with a 
	 * false value and the netscience dataset is calculated with a true value, for 
	 * example. To obtain the values from @cite ying use _false_
	 * @param hop_plot The hop plot generated for this graph
	 * @returns The average path length between any two connected vertices.
	 * @pre Assumes that hop_plot has been populated with a call to hop_plot()
	 */
	template <bool include_self_paths >
	float average_path_length( HopPlot *hop_plot ) const;
	
	/**
	 * Calculates the subgraph centrality of the graph.
	 * @param limit The maximum length walk over which to compute subgraph 
	 * centrality.
	 * @returns The subgraph centrality of the graph.
	 */
	double subgraph_centrality( const uint32_t limit ) const;
	
	/**
	 * Populates the hop plot for this graph.
	 * @param hop_plot A map from path length i to the number of ordered 
	 * vertex pairs whose shortest path between them is of length i.
	 * @post The hop_plot map is first cleared and then populated with the 
	 * hop plot data for this graph.
	 * @see average_path_length()
	 */
	HopPlot hop_plot() const;

	bool is_complete() const;
	
	/**
	 * Determines whether or not the UnlabelledGraph is 
	 * k-degree-anonymous.
	 * @param k The privacy threshold, k. Every vertex must 
	 * have the same degree as at least k-1 other vertices in 
	 * order to be k-degree-anonymous.
	 * @returns True if the UnlabelledGraph is k-degree-anonymous; 
	 * false if not.
	 */
	bool is_anonymous( const uint32_t k ) const;
	
	/**
	 * Modifies the UnlabelledGraph so that it is k-degree-anonymous 
	 * using the algorithm from @cite waldo .
	 * @tparam hide_new_vertices A boolean flag indicating whether or 
	 * not the newly added vertices should also be anonymised. Note that 
	 * the experiments in @cite waldo have this flag set to _false_.
	 * @param k The privacy threshold, k.
	 * @post The UnlabelledGraph is modified to be a super-graph with 
	 * a larger vertex set and edge set such that it is k-degree-anonymous.
	 * @see is_anonymous()
	 * @note You may observe some minor variances from the anonymisation 
	 * reported in @cite waldo because the order of vertices within an 
	 * equivalence class is not clearly defined.
	 */
	template < bool hide_new_vertices >
	void hide_waldo( const uint32_t k );
	
	friend std::ostream& operator << ( std::ostream& os, UnlabelledGraph const& g );

protected:

	/**
	 * Inserts the undirected edge (u,v) into the graph if it does not already exist.
	 * @param u The source vertex of the edge
	 * @param v The destination vertex of the edge
	 * @return True if the edge was added, false if it already existed
	 * @post Edge (u,v) exists in the graph (irrespective of whether it was there
	 * prior to invoking the method)
	 */
	bool add_edge( const uint32_t u, const uint32_t v );
	
	/**
	 * Adds a specified number of isolated vertices to the graph.
	 * @param num_vertices The number of vertices that should be added
	 * to the graph.
	 * @post The graph contains num_vertices more vertices (all isolated) 
	 * than before execution of the subroutine.
	 */
	void add_vertices( const uint32_t num_vertices );

	/**
	 * Inserts a random new edge into the graph if the graph is not already
	 * a complete graph.
	 * @post The graph remains unaffected if it is complete. Otherwise,
	 * one edge that previously was not in the graph now appears.
	 */
	void add_random_edge();
	
	/**
	 * Populates the degree sequence for this UnlabelledGraph.
	 * @param degrees A vector to populate with the degree sequence, where each 
	 * element is a pair of the form (degree, vertex id).
	 * @post degrees is emptied and then populated with a list of degrees 
	 * for each vertex, not necessarily unique and in ascending order.
	 */
	DegreeSequence retrieve_degree_sequence() const;

	/**
	 * Returns the path length between vertex u and vertex v.
	 * @param u The id of the source vertex
	 * @param v The id of the destination vertex
	 * @returns -1 if u and v are disconnected; otherwise, the 
	 * minimum number of edges that must be traversed in order to 
	 * reach v from u.
	 */
	int calculate_path_length( uint32_t u, uint32_t v ) const;


	/* Member variables */
	
	uint32_t n_; /**< The number of vertices in the graph. */
	uint32_t m_; /**< The number of edges in the graph. */
	graphAnon::FileFormat const io_format_; /**< The file format for reading/writing graphs. */
	
	/**
	 * The adjacency list: adjacency_list[i] is a set
	 * of node ids that are neighbours for the node with id i.
	 */
	AdjacencyList adjacency_list_;
	
private:
	
	/**
	 * Calculates the average path length of the graph in a slow 
	 * but definitely correct manner by executing a breadth first 
	 * search for shortest paths between every pair of vertices 
	 * in the graph.
	 * @tparam include_self_paths A boolean suggesting whether paths of 
	 * the form (u,u) with length 0 should be included; if so, the denominator 
	 * of the APL expression will increase.This option is adopted in @cite waldo 
	 * in Figures 10-13, but the football dataset in Table 2 mistakenly is 
	 * calculated with a false value. To obtain the values from @cite ying 
	 * one should again use use _true_.
	 * @returns The average path length between any two connected 
	 * vertices.
	 * @note This method is slow and primarily for testing purposes.
	 * @see average_path_length()
	 */
	template <bool include_self_paths >
	float average_path_length_brute_force() const;
	
	/**
	 * Calculates the clustering coefficient of the graph in a slow 
	 * but definitely correct manner.
	 * @returns The clustering coefficient of the graph.
	 * @note This method is slow and primarily for testing purposes.
	 * @see clustering_coefficient()
	 */
	float clustering_coefficient_brute_force() const;
};

#include "unlabelled_graph.tpp"

#endif /* UNLABELLED_GRAPH_H_ */
