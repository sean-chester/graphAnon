/**
 * @file
 * @brief Definition of a simple, undirected, vertex-labelled Graph class.
 *
 * @date 22 Oct 2015
 * @version 2.0
 * @author Sean Chester (sean.chester@idi.ntnu.no)
 *
 * @copyright Copyright (c) 2015 Sean Chester
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

#ifndef LABELLED_GRAPH_H_
#define LABELLED_GRAPH_H_

#include <cstdint>	/* For uint32_t */
#include <fstream>	/* for std::ofstream */

/* STL libraries in use */
#include <vector>
#include <unordered_set>

#include "../unlabelled_graph/unlabelled_graph.h"
#include "label_distribution.h"

/**
 * @brief A simple, undirected, vertex-labelled graph with no self-loops that is
 * equipped with methods for attribute disclosure protection.
 * @extends UnlabelledGraph
 * @todo Unit test this class, particular the is_alpha_proximal() and
 * greedy() methods.
 */
class LabelledGraph : public UnlabelledGraph {
public:

	/**
	 * Constructs a LabelledGraph object from a file
	 * @param filename The path to the input file containing the graph
	 * @post Constructs a new LabelledGraph object
	 * @warning Does minimal error-checking. If the file format is
	 * invalid or filename is an incorrect path, then the behaviour
	 * of this constructor is undefined.
	 * @see <a href="../../workloads/paper_example.adjList">An example file</a>
	 * consisting of the example LabelledGraph from Figure 1 of @cite asonam ,
	 * represented in the vertex-labelled adjacency list format.
	 */
	LabelledGraph( const std::string filename );


	/**
	 * Constructs a vertex-labelled graph with n isloated vertices.
	 * @param num_vertices The number of vertices in the graph.
	 * @param num_labels The size of the label alphabet (i.e., the
	 * number of unique vertex labels).
	 * @post Constructs a new LabelledGraph object
	 */
	LabelledGraph( const uint32_t num_vertices, const uint32_t num_labels );

	/**
	 * Destroys the LabelledGraph.
	 */
	virtual ~LabelledGraph();

	/**
	 * Initializes empty LabelledGraph data structures: should be called
	 * by all overloaded constructors once n_ and l_ are set.
	 */
	void init();

	/**
	 * Assigns a random label to each vertex such that (to the maximum extent
	 * possible) every label appears with the same frequency.
	 */
	void evenly_distribute_labels();

	/**
	 * Determines whether this graph is alpha-proximal.
	 * @param alpha The privacy threshold
	 * @return True if every vertex has a LabelDistribution within a distance
	 * of alpha of the global LabelDistribution
	 * @see Definition 2.6 of @cite asonam
	 */
	bool is_alpha_proximal( const float alpha );

	/**
	 * Naively transforms the graph into an alpha-proximal graph by alternately
	 * adding a random edge and then checking if the graph is alpha-proximal. The
	 * algorithm is guaranteed to reach a solution because the complete graph is
	 * a solution (every vertex's neighbourhood LabelDistribution is exactly the global
	 * LabelDistribution).
	 * @param alpha The privacy threshold
	 * @post Inserts edges into the graph so that the graph is alpha-proximal
	 */
	void hopeful( const float alpha );

	/**
	 * Transforms the graph into an alpha-proximal graph, using the Greedy
	 * alpha-proximity algorithm from @cite asonam (Algorithm 1), hopefully
	 * inducing much fewer edge additions than the hopeful algorithm.
	 * @param alpha The privacy threshold
	 * @post Inserts edges into the graph so that the graph is alpha-proximal
	 */
	void greedy( const float alpha );

	/**
	 * Prints the graph to outstream in vertex-labelled adjacency list format
	 * (primarily for the purpose of testing).
	 * @param outstream The file stream to which the Graph should be output
	 * @see <a href="../../workloads/paper_example.adjList">An example file</a>
	 * consisting of the example Graph from Figure 1 of @cite asonam ,
	 * represented in the vertex-labelled adjacency list format.
	 */
	void print( std::ofstream *outstream );

private:

	/**
	 * Obtains and constructs at address ld a LabelDistribution
	 * corresponding to the global frequencies of all labels for all
	 * vertices in the graph.
	 * @param ld The address at which the new LabelDistribution should
	 * be constructted.
	 * @post ld contains a new LabelDistribution instance.
	 */
	void inline get_global_ld( LabelDistribution **ld );

	/**
	 * Obtains and constructs at address ld a LabelDistribution
	 * corresponding to the frequencies of all labels of vertices
	 * within the 1-hop neighbourhood of vertex v.
	 * @param ld The address at which the new LabelDistribution should
	 * be constructed.
	 * @param v The vertex id for whom the neighbourhood LabelDistribution
	 * should be calculated
	 * @post ld contains a new LabelDistribution instance.
	 */
	void inline get_neighbourhood_ld( LabelDistribution **ld, const uint32_t v );


	/**
	 * Runs an iteration of the Greedy Alpha-Proximity algorithm (Lines 2--4 in
	 * Algorithm 1 of @cite asonam ).
	 * @param alpha The privacy threshold
	 * @return The number of edges that were added to the graph during
	 * this iteration
	 * @post The graph contains new edges and has greedily moved closer to being
	 * alpha-proximal.
	 */
	uint32_t run_greedy_iteration( const float alpha );


	/* Private member variables. */
	/**
	 * The vertex-labelling function (i.e., a mapping between vertex and
	 * vertex label).
	 */
	std::vector< uint32_t > vertex_labels_;
	uint32_t l_; /**< The size of the label set. */
	
};


#endif /* LABELLED_GRAPH_H_ */
