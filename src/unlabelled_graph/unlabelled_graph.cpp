/**
 * @file
 * @brief Implementation of the UnlabelledGraph class in unlabelled_graph.h
 * @see unlabelled_graph.tpp for the implementation of templated functions.
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
#include <unordered_map>
#include <queue>

#include "omp.h"

#include "unlabelled_graph.h" /* implementing this class. */

void UnlabelledGraph::init() {
	/* Initialize adjacency list with n_ empty vectors and every vertex
	 * to have the same label. */
	adjacency_list_.reserve( n_ );
	for( uint32_t i = 0; i < n_ ; ++i ) {
		adjacency_list_.push_back( std::unordered_set< uint32_t >() );
	}

	/* Originally, there are no edges yet (every vertex is isolated). */
	m_ = 0;

	/* initialize random seed for generating random edges later */
	srand (time(NULL));
}

UnlabelledGraph::UnlabelledGraph( const uint32_t num_vertices ) :
	n_ ( num_vertices ), io_format_( graphAnon::FileFormat::adjacencyList ) { init(); }


UnlabelledGraph::UnlabelledGraph() : n_ ( 0 ), 
	io_format_( graphAnon::FileFormat::adjacencyList ) { init(); }

UnlabelledGraph::UnlabelledGraph( const std::string filename, graphAnon::FileFormat format )
	: io_format_( format )
{
	std::string line;
	std::cout << filename << std::endl;
	std::ifstream infile( filename );

	/* first parse the graph and label alphabet sizes from
	 * the first line of the file
	 */
	std::getline( infile, line );
	std::istringstream iss( line );
	iss >> n_;

	/* check whether n_ was at least read correctly -- the only real
	 * error checking done in this constructor.
	 */
	if( n_ <= 0 ) {
		std::cerr << "Did not parse a positive number of vertices from input file. "
				<< "Did you format the file correctly and specify the correct path?"
				<< std::endl;
		return;
	}

	/* Init like other constructors now that the data structure sizes
	 * are known.
	 */
	init();
	
	if( io_format_ == graphAnon::FileFormat::adjacencyList 
		|| io_format_ == graphAnon::FileFormat::adjacencyListVertexLabelled )
	{
	
		uint32_t v;
			
		/* check if extra meta data for label set size and throw out. */
		if( io_format_ == graphAnon::FileFormat::adjacencyListVertexLabelled ) {
			iss >> v;
		}

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
			
			/* if labelled, throw out label first. */
			if( io_format_ == graphAnon::FileFormat::adjacencyListVertexLabelled ) {
				iss >> v;
			}

			/* all numbers on the adjacency list line
			 * neighbours of u: add them to u's adjacency list.
			 * Note: undirected graph, so also reciprocally adds
			 * (v, u), even if that isn't in the input file
			 */
			while( iss >> v ) { add_edge( u, v ); }
		}
	}
	else if( io_format_ == graphAnon::FileFormat::edgeList ) {
		uint32_t u, v;
		/* Iterate every edge in the input file. */
		while( std::getline( infile, line ) ) {
			std::istringstream iss( line );
			iss >> u >> v;
			add_edge( u, v );
		}
	}
}

UnlabelledGraph::~UnlabelledGraph() {}

uint32_t UnlabelledGraph::num_vertices() { return n_; }
uint32_t UnlabelledGraph::num_edges() { return m_; }

bool UnlabelledGraph::add_edge( const uint32_t u, const uint32_t v ) {
	if( adjacency_list_[ u ].count( v ) > 0 || u == v ) { return false; }
	if( adjacency_list_[ v ].count( u ) > 0 ) { return false; }
	adjacency_list_[ u ].insert( v );
	adjacency_list_[ v ].insert( u );
	++m_;
	return true;
}

void UnlabelledGraph::add_vertices( const uint32_t num_vertices ) {

	for( uint32_t i = 0; i < num_vertices ; ++i ) {
		adjacency_list_.push_back( std::unordered_set< uint32_t >() );
	}
	n_ += num_vertices;
}

void UnlabelledGraph::add_random_edge() {

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


bool UnlabelledGraph::populate_uniformly( const uint32_t num_edges ) {
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

bool UnlabelledGraph::is_complete() { return m_ == n_ * ( n_ - 1 ); }

bool UnlabelledGraph::is_anonymous( const uint32_t k ) {

	/* First calculate the counts for every degree in the graph. */
	std::unordered_map< uint32_t, uint32_t > degree_counts;
	for (uint32_t i = 0; i < n_; ++i ) {
		const uint32_t next_degree = adjacency_list_[ i ].size();
		if( degree_counts.count( next_degree ) == 0 ) {
			degree_counts[ next_degree ] = 1;
		}
		else { ++degree_counts[ next_degree ]; }
	}
	
	/* Then ensure every count is at least k. */
	for( auto it = degree_counts.begin(); it != degree_counts.end(); ++it ) {
		if( it->second < k ) { return false; }
	}
	return true;
}

float UnlabelledGraph::get_occupancy() {
	if( n_ == 0 ) { return 0; }
	else return m_ / (float) ( n_ * ( n_ - 1 ) ) * 2; /* x2 because undirected */
}

float UnlabelledGraph::clustering_coefficient() {
	
	//return clustering_coefficient_brute_force();
	
	uint64_t closed_triangles = 0;
	uint64_t possible_triangles = 0;
	
	/* First count denominator -- how many open triangles exist. */
#pragma omp parallel for reduction( +: possible_triangles )
	for( uint32_t i = 0; i < n_; ++i ) {
		NeighbourList *my_neighbours = &( adjacency_list_[ i ] );
		possible_triangles += my_neighbours->size() * ( my_neighbours->size() - 1 );
	} 
	
	/* Then count numerator -- how many closed triangles exist. */
#pragma omp parallel for reduction( +: closed_triangles )
	for( uint32_t u = 0; u < n_; ++u ) {
		for( auto v = adjacency_list_[ u ].begin(); v != adjacency_list_[ u ].end(); ++v ) {
			for( auto w = adjacency_list_[ u ].begin(); w != adjacency_list_[ u ].end(); ++w ) {
				if( *v == *w ) { continue; }
				auto closure = std::find( adjacency_list_[ *v ].begin(), adjacency_list_[ *v ].end(), *w );
				if( closure != adjacency_list_[ *v ].end() ) { ++closed_triangles; } 
			}
		}
	}
	
	return closed_triangles / (float) possible_triangles;
}

float UnlabelledGraph::clustering_coefficient_brute_force() {
	uint64_t closed_triangles = 0;
	uint64_t possible_triangles = 0;
	
	/* Iterate all ordered triplets of vertices. */
#pragma omp parallel for reduction ( +: closed_triangles, possible_triangles )
	for( uint32_t u = 0; u < n_; ++u ) {
		for( uint32_t v = 0; v < n_; ++v ) {
			if( u == v ) { continue; }
			for( uint32_t w = 0; w < n_; ++w ) {
				if( u == w || v == w ) { continue; }
				if( adjacency_list_[ u ].count( v ) == 1 &&
						 adjacency_list_[ v ].count( w ) == 1 ) {
						 
					++possible_triangles;
					if( adjacency_list_[ u ].count( w ) == 1 ) {
						++closed_triangles;
					}
				}
			}
		}
	}
	
	return closed_triangles / (float) possible_triangles;
}


void UnlabelledGraph::hop_plot( HopPlot *hop_plot ) {

	std::unordered_set< uint32_t > visited;
	std::queue< std::pair< uint32_t, uint32_t > > q; /* (vertex, path length) pairs. */	
	uint32_t num_threads;
	
#pragma omp parallel 
	{
		num_threads = omp_get_num_threads();
	}
	HopPlot hopplots[ num_threads ];
	
#pragma omp parallel for private( visited, q )
	for( uint32_t i = 0; i < n_; ++i ) {
		
		HopPlot *my_hop_plot = hopplots + omp_get_thread_num();

		/* clear queue and visited set, although set i so we don't loop. */
		visited.clear();
		visited.insert( i );
	
		/* Init queue to contain all direct neighbours of vertex i. */
		for( auto it = adjacency_list_[ i ].begin(); it != adjacency_list_[ i ].end(); ++it ) {
			q.push( std::pair< uint32_t, uint32_t >( *it, 1 ) );
			visited.insert( *it );
		}
		/* Add all neighbours of i to hop plot score = 1. */
		if( my_hop_plot->count( 1 ) == 1 ) { my_hop_plot->at( 1 ) += adjacency_list_[ i ].size(); }
		else { (*my_hop_plot)[ 1 ] = adjacency_list_[ i ].size(); }
	
		/* Iterate breadth-first through remaining paths. */
		while( ! q.empty() ) {
			const uint32_t v = q.front().first;
			const uint32_t d = q.front().second;
			q.pop();
		
			for( auto it = adjacency_list_[ v ].begin(); it != adjacency_list_[ v ].end(); ++it ) {
				if( visited.count( *it ) == 0 ) {
					visited.insert( *it );
					q.push( std::pair< uint32_t, uint32_t >( *it, d + 1 ) );
					if( my_hop_plot->count( d + 1 ) == 0 ) { (*my_hop_plot)[ d + 1 ] = 1; }
					else { ++my_hop_plot->at( d + 1 ); }
				}
			}
		}
	}
		
	/* Reduce all the hop plots from each thread. */
	for( uint32_t t = 0; t < num_threads; ++t ) {
		for( auto it = hopplots[ t ].begin(); it != hopplots[ t ].end(); ++it ) {
			if( hop_plot->count( it->first ) == 0 ) { (*hop_plot)[ it->first ] = it->second; }
			else { hop_plot->at( it->first ) += it->second; }
		}
	}
}


float UnlabelledGraph::harmonic_mean( HopPlot *hop_plot ) {
	float h = 0;
	
	for( auto it = hop_plot->begin(); it != hop_plot->end(); ++it ) {
		h += it->second / (float) it->first; 
	}
	return ( h == 0 ? -1 : n_ * ( n_ - 1 ) / h );
}

/* Computes sc by repeatedly exponentiating matrix and summing diagonals. */
double UnlabelledGraph::subgraph_centrality( const uint32_t limit ) {
	double summation = 0;
	double factorial = 1;
	
	/* First, create double-buffer adjacency matrix explicitly. 
	 * Need doubles to avoid overflow in matrix. */
	double *adjacency_matrix = new double[ n_ * n_ ];
	double *adjacency_matrix_to_lth = new double[ n_ * n_ ];
	double *new_values = new double[ n_ * n_ ];
	
	/* Populate adjacency matrix. */
#pragma omp parallel for
	for( uint32_t i = 0; i < n_; ++i ) {
		const uint32_t offset = i * n_;
		
		for( uint32_t j = 0; j < n_; ++j ) {
			adjacency_matrix[ offset + j ] = 0;
			adjacency_matrix_to_lth[ offset + j ] = 0;
		}
		
		NeighbourList *neighbours = &( adjacency_list_[ i ] );
		for( auto it = neighbours->cbegin(); it != neighbours->cend(); ++it ) {
			adjacency_matrix[ offset + *it ] = 1;
			adjacency_matrix_to_lth[ offset + *it ] = 1;
		}
	}
	
	/* Iterate over path lengths */ 
	for( uint32_t l = 2; l <= limit; ++l ) {
		factorial *= l;
		
		/* Raise adjacency matrix to next power. */
#pragma omp parallel for reduction ( +: summation )
		for( uint32_t i = 0; i < n_; ++i ) {
			const uint32_t row_offset = i * n_;
			for( uint32_t j = 0; j < n_; ++j ) {
				double cell_value = 0;
				for( uint32_t k = 0; k < n_; ++k ) {
					const uint32_t transpose_offset = k * n_;
					cell_value += adjacency_matrix[ row_offset + k ] 
											* adjacency_matrix_to_lth[ transpose_offset + j];
				}
				new_values[ row_offset + j ] = cell_value;
				if( j == i ) {
					
					/* divide by factorial and add to running sum */
					summation += cell_value / factorial;
				}
			}
		}
		
		/* swap buffers */
		double *tmp = adjacency_matrix_to_lth;
		adjacency_matrix_to_lth = new_values;
		new_values = tmp;
	}
	
	delete [] adjacency_matrix;
	delete [] adjacency_matrix_to_lth;
	delete [] new_values;
	return summation / n_;
}

std::ostream& operator << ( std::ostream& os, UnlabelledGraph const& g )
{
	os << g.n_ << std::endl;
	for( uint32_t i = 0; i < g.n_; ++i ) {
		for( auto it = g.adjacency_list_[i].begin(); it != g.adjacency_list_[i].end(); ++it ) {
			os << *it << " ";
		}
		os << std::endl;
	}
	return os;
}
