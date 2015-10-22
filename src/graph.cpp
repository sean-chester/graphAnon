/**
 * @file
 * @brief Implementation of the Graph class in graph.h
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

#include <cstdint>		/* for uint32_t */
#include <algorithm>	/* for random_shuffle */
#include <iostream>		/* for cout, endl */
#include <cstdlib>		/* for srand, rand */
#include <cstring>		/* for ffs and std::string */
#include <fstream>		/* for ifstream, infile */
#include <sstream>		/* for istringstream, getline */

/* STL stuff in use. */
#include <vector>
#include <unordered_set>

#include "graph.h" /* implementing this class. */

void Graph::init() {
	/* Initialize adjacency list with n_ empty vectors and every vertex
	 * to have the same label. */
	adjacency_list_.reserve( n_ );
	for( uint32_t i = 0; i < n_ ; ++i ) {
		adjacency_list_.push_back( std::unordered_set< uint32_t >() );
		vertex_labels_.push_back ( 0 );
	}

	/* Originally, there are no edges yet (every vertex is  isolated). */
	m_ = 0;

	/* initialize random seed for generating random edges later */
	srand (time(NULL));
}

Graph::Graph( const uint32_t num_vertices, const uint32_t num_labels ) :
	n_ ( num_vertices ), l_ ( num_labels ) { init(); }

Graph::Graph( const std::string filename ) {
	std::string line;
	std::cout << filename << std::endl;
	std::ifstream infile( filename );

	/* first parse the graph and label alphabet sizes from
	 * the first line of the file
	 */
	std::getline( infile, line );
	std::istringstream iss( line );
	iss >> n_ >> l_;

	/* check whether n_ was at least read correctly -- the only real
	 * error checking done in this constructor.
	 */
	if( n_ < 0 ) {
		std::cerr << "Did not parse a positive number of vertices from input file. "
				<< "Did you format the file correctly and specify the correct path?"
				<< std::endl;
		return;
	}

	/* Init like other constructors now that the data structure sizes
	 * are known.
	 */
	init();

	/* Iterate exactly enough times to fill the data structures,
	 * irrespective of the length of the file. Each iteration handles
	 * one vertex, and the file is assumed to be sorted by increasing vertex
	 * id, with all vertex ids represented as integers in a contiguous sequence
	 * starting from 0.
	 */
	for( uint32_t u = 0; u < n_; ++u ) {

		/* grab line related to vertex u. */
		std::getline( infile, line );
		std::istringstream iss( line );

		/* set label to be first character */
		iss >> vertex_labels_[ u ];

		/* all other numbers on the adjacency list line
		 * neighbours of u: add them to u's adjacency list.
		 * Note: undirected graph, so also reciprocally adds
		 * (v, u), even if that isn't in the input file
		 */
		uint32_t v;
		while( iss >> v ) { add_edge( u, v ); }
	}
}

Graph::~Graph() {}

bool Graph::add_edge( const uint32_t u, const uint32_t v ) {
	if( adjacency_list_[ u ].count( v ) > 0 || u == v ) { return false; }
	adjacency_list_[ u ].insert( v );
	adjacency_list_[ v ].insert( u );
	++m_;
	return true;
}

void Graph::add_random_edge() {

	/* Error checking -- are there edges to add? */
	if( is_complete() ) { return; }

	while( true ) {
		/* get random edge */
		const uint32_t u = rand() % n_;
		const uint32_t v = rand() % n_;

		/* add it if it doesn't yet exist */
		if ( add_edge( u, v ) ) { return; }
	}
}


bool Graph::populate_uniformly( const uint32_t num_edges ) {
	/* error checking: can we add this many edges? */
	if ( num_edges > n_ * ( n_ - 1 ) - m_ ) { return false; }

	/* create a list of all possible edges and randomly shuffle the list */
	std::vector< std::pair < uint32_t, uint32_t > > possible_edges;
	for( uint32_t i = 0; i < n_; ++i ) {
		for( uint32_t j = i + 1; j < n_; ++j ) {
			possible_edges.push_back( std::pair< uint32_t, uint32_t > ( i, j ) );
		}
	}
	std::random_shuffle( possible_edges.begin(), possible_edges.end() );

	/* Add the first num_edges randomly shuffled edges that do not already
	 * exist in the graph.
	 */
	uint32_t num_added = 0;
	for( auto it = possible_edges.begin(); it != possible_edges.end(); ++it ) {
		if ( add_edge( it->first, it->second ) ) {
			if( ++num_added == num_edges ) { return true; } /* Done! */
		}
	}
	return false; /* should be an unreachable statement! */
}

void Graph::evenly_distribute_labels() {
	const uint32_t vertices_per_label = n_ / l_;
	uint32_t labels_left = n_ - vertices_per_label;

	/* Starts with all labels set to 0. Iterates in total all but vertices_per_label times, plus
	 * the remainder from the division. When iteration is done, label 0 is left with only
	 * vertices_per_label plus the remainder vertices left.
	 */
	for( uint32_t cur_label = 1; cur_label < l_; ++cur_label ) {
		for( uint32_t num_assigned = 0; num_assigned < vertices_per_label; ++num_assigned ) {
			const uint32_t v = rand() % n_;
			if( vertex_labels_[ v ] == 0 ) { vertex_labels_[ v ] = cur_label; }
			else { --num_assigned; }
		}
		labels_left -= vertices_per_label;
	}

	/* The remainder is then distributed with a random permutation. */
	if( labels_left > 0 ) {
		std::unordered_set< uint32_t > s;
		while( labels_left > 0 ) {
			const uint32_t v = rand() % n_;
			if( vertex_labels_[ v ] == 0 ) {
				uint32_t l = rand() % l_;
				while( s.count( l ) > 0 ) {
					l = rand() % l_;
				}
				s.insert( l );
				vertex_labels_[ v ] = l;
				--labels_left;
			}
			else {
				/* with equal probability, keep it 0. */
				if( rand() % l_ == 0 ) { --labels_left; }
			}
		}
	}
}

bool Graph::is_complete() { return m_ == n_ * ( n_ - 1 ); }

float Graph::get_occupancy() {
	if( n_ == 0 ) { return 0; }
	else return m_ / (float) ( n_ * ( n_ - 1 ) ) * 2; /* x2 because undirected */
}

void Graph::print( std::ofstream *outstream ) {
	(*outstream) << n_ << " " << l_ << std::endl;
	for( uint32_t i = 0; i < n_; ++i ) {
		(*outstream) << vertex_labels_[ i ] << " ";
		for( auto it = adjacency_list_[i].begin(); it != adjacency_list_[i].end(); ++it ) {
			(*outstream) << *it << " ";
		}
		(*outstream) << std::endl;
	}
}

void inline Graph::get_global_ld( LabelDistribution **ld ) {

	/* Initialize an empty solution. */
	std::vector< uint32_t > counts;
	for( uint32_t i = 0; i < l_; ++i ) { counts.push_back( 0 ); }

	/* Iterate vertices, incrementing the relative label frequency
	 * counts for each one.
	 */
	for( auto it = vertex_labels_.begin(); it != vertex_labels_.end(); ++it ) {
		++counts[ *it ];
	}

	/* Create new LabelDistribuion from he counts vector. */
	*ld = new LabelDistribution( &counts );
}

void inline Graph::get_neighbourhood_ld( LabelDistribution **ld, const uint32_t v ) {

	/* Initialize an empty solution. */
	std::vector< uint32_t > counts;
	for( uint32_t i = 0; i < l_; ++i ) { counts.push_back( 0 ); }

	/* Increment count for v's label. */
	++counts[ vertex_labels_[ v ] ];

	/* Iterate neighbours of v, incrementing the relative label frequency
	 * counts for each one.
	 */
	for( auto it = adjacency_list_[ v ].begin(); it != adjacency_list_[ v ].end(); ++it ) {
		++counts[ vertex_labels_[ *it ] ];
	}

	/* Create new LabelDistribuion from he counts vector. */
	*ld = new LabelDistribution( &counts );
}

bool Graph::is_alpha_proximal( const float alpha ) {
	LabelDistribution *global, *neighbourhood;
	float max_distance = 0;

	get_global_ld( &global );

	/* Iterate every vertex, checking its susceptibility to an
	 * attribute disclosure (NAD) attack
	 */
	for( uint32_t v = 0; v < n_; ++v ) {
		get_neighbourhood_ld( &neighbourhood, v );
		const float distance = global->distance( neighbourhood );
		if( distance > max_distance ) { max_distance = distance; }
		delete neighbourhood;
	}

	delete global;
	return max_distance <= alpha;
}

void Graph::hopeful( const float alpha ) {
	bool leaks_privacy = !is_alpha_proximal( alpha );
	while( leaks_privacy && !is_complete() ) {
		if( is_alpha_proximal( alpha ) ) { leaks_privacy = false; }
		else { add_random_edge(); }
	}
}

uint32_t Graph::run_greedy_iteration( const float alpha ) {

	LabelDistribution *global, *neighbourhood;
	std::vector< std::pair< uint32_t, uint32_t > > visit_order;
	uint32_t num_edges_added = 0;

	get_global_ld( &global );

	for( uint32_t i = 0; i < n_; ++i ) {

		/* First determine which "partition" vertex i belongs to. */
		get_neighbourhood_ld( &neighbourhood, i );
		const uint32_t defs = neighbourhood->get_deficiencies( global, alpha );

		/* If vertex i is already alpha-proximal, exclude it
		 * from further processing. */
		if( defs > 0 ) {
			visit_order.push_back ( std::pair< uint32_t, uint32_t > ( i, defs ) );
		}

		/* clean up from vertex i. */
		delete neighbourhood;
	}

	/* Randomize the order of the points so that edges are added more
	 * "evenly." */
	std::random_shuffle( visit_order.begin(), visit_order.end() );

	/* Process each deficient point v with label l1 by, for each deficient label
	 * l2, finding a mate u with label l who is deficient in l2 and adding
	 * edge (u,v) to the graph (if it can be done).
	 */
	for( auto it = visit_order.begin(); it != visit_order.end(); ++it ) {
		/* redeclare for readability the variables related to this iteration. */
		const uint32_t v = it->first;
		const uint32_t v_label_bitmask = 1 << vertex_labels_[ v ];
		uint32_t defs = it->second;
		const uint32_t num_def_labels = __builtin_popcount( defs );

		/* Iterate deficient labels, trying to correct them. */
		for( uint32_t i = 0; i < num_def_labels; ++i ) {
			/* grab next deficient label from bitmask */
			const uint32_t l = ffs( defs ) - 1;

			/* find a mate with whom to connect (if there is one) */
			for( auto it_mate = it + 1; it_mate != visit_order.end(); ++it_mate ) {
				if( it_mate->second & v_label_bitmask ) { /* mate deficient in my label */
					if( vertex_labels_[ it_mate->first ] == l ) { /* mate offers my deficiency */
						if( add_edge( v, it_mate->first ) ) {
							it_mate->second ^= v_label_bitmask;
							++num_edges_added;
							break; /* success! */
						}
					}
				}
			}

			/* clear bit as having been resolved */
			defs ^= 1 << ( l - 1 );
		}
	}

	/* clean up and return */
	delete global;
	return num_edges_added;
}

void Graph::greedy( const float alpha ) {
	bool leaks_privacy = !is_alpha_proximal( alpha );
	while( leaks_privacy && !is_complete() ) {
		const uint32_t num_new_edges = run_greedy_iteration( alpha );
		if( is_alpha_proximal( alpha ) ) { leaks_privacy = false; }
		else if ( num_new_edges == 0 ) { add_random_edge(); }
	}
}

