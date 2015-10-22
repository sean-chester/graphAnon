/**
 * @file
 * @brief Definition of a simple, undirected Graph class.
 *
 * @date Jun 12, 2015
 * @version 1.0
 * @author Sean Chester (schester@cs.au.dk)
 *
 * @copyright &copy; 2015, Sean Chester (schester@cs.au.dk)<br />
 *  All rights reserved.
 *                         <br /><br />
 *  This file is a part of the AlphaProximity suite.
 *  The AlphaProximity suite is free software: redistribution and use in
 *  source and binary forms, with or without modification, are
 *  permitted provided that the following conditions are met:
 *                         <br /><br />
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *                         <br /><br />
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *                         <br /><br />
 *  3. Neither the name of the copyright holder nor the names of its
 *  contributors may be used to endorse or promote products derived
 *  from this software without specific prior written permission.
 *                         <br /><br />
 *  4. Any and all academic use of this, or any part of this, software
 *  must cite the article referenced here: @cite asonam .
 *                         <br /><br />
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <cstdint>	/* For uint32_t */
#include <fstream>	/* for std::ofstream */

/* STL libraries in use */
#include <vector>
#include <unordered_set>

#include "label_distribution.h"

/**
 * @brief A simple, undirected, vertex-labelled graph with no self-loops that is
 * equipped with methods for attribute disclosure protection.
 * @todo Unit test this class, particular the is_alpha_proximal() and
 * greedy() methods.
 */
class Graph {
public:

	/**
	 * Constructs a Graph object from a file
	 * @param filename The path to the input file containing the graph
	 * @post Constructs a new Graph object
	 * @warning Does minimal error-checking. If the file format is
	 * invalid or filename is an incorrect path, then the behaviour
	 * of this constructor is undefined.
	 * @see <a href="../../workloads/paper_example.adjList">An example file</a>
	 * consisting of the example Graph from Figure 1 of @cite asonam ,
	 * represented in the vertex-labelled adjacency list format.
	 *
	 * Constructs a pre-built graph from a file. The file format is a
	 * vertex-labelled adjacency list. The first row gives white-space
	 * separated meta-data about the Graph, namely the number of vertices
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
	Graph( const std::string filename );


	/**
	 * Constructs a vertex-labelled graph with n isloated vertices.
	 * @param num_vertices The number of vertices in the graph.
	 * @param num_labels The size of the label alphabet (i.e., the
	 * number of unique vertex labels).
	 * @post Constructs a new Graph object
	 */
	Graph( const uint32_t num_vertices, const uint32_t num_labels );

	/**
	 * Destroys the graphs.
	 */
	virtual ~Graph();

	/**
	 * Initializes empty Graph data structures: should be called
	 * by all overloaded constructors once n_ and l_ are set.
	 */
	void init();

	/**
	 * Populates the graph with num_edges undirected edges, randomly chosen
	 * with uniform distribution.
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
	 * Assigns a random label to each vertex such that (to the maximum extent
	 * possible) every label appears with the same frequency.
	 */
	void evenly_distribute_labels();

	/**
	 * Retrieves the percentage of possible edges tha are present in the graph.
	 * @return If E is the edge set and V is the vertex set, the return value is
	 * |E| / |V| / ( |V| - 1 ). Will also return 0 if |V| = 0.
	 */
	float get_occupancy();

	bool is_complete();

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
	 * Inserts the undirected edge (u,v) into the graph if it does not already exist.
	 * @param u The source vertex of the edge
	 * @param v The destination vertex of the edge
	 * @return True if the edge was added, false if it already existed
	 * @post Edge (u,v) exists in the graph (irrespective of whether it was there
	 * prior to invoking the method)
	 */
	bool add_edge( const uint32_t u, const uint32_t v );

	/**
	 * Inserts a random new edge into the graph if the graph is not already
	 * a complete graph.
	 * @post The graph remains unaffected if it is complete. Otherwise,
	 * one edge that previously was not in the graph now appears.
	 */
	void add_random_edge();

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

	/**
	 * The number of vertices in the graph
	 */
	uint32_t n_;
	/**
	 * The number of edges in the graph.
	 */
	uint32_t m_;
	/**
	 * The size of the label set.
	 */
	uint32_t l_;
	/**
	 * The adjacency list: adjacency_list[i] is a set
	 * of node ids that are neighbours for the node with id i.
	 */
	std::vector< std::unordered_set < uint32_t > > adjacency_list_;
	/**
	 * The vertex-labelling function (i.e., a mapping between vertex and
	 * vertex label).
	 */
	std::vector< uint32_t > vertex_labels_;
};


#endif /* GRAPH_H_ */
