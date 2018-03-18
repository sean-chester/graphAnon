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

#include <numeric>		/* for std::accumulate() */

#include "omp.h"

#include "unlabelled_graph.h" /* implementing this class. */

void UnlabelledGraph::init() {
	
	adjacency_list_ = AdjacencyList( n_ );

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

uint32_t UnlabelledGraph::num_vertices() const { return n_; }
uint32_t UnlabelledGraph::num_edges() const { return m_; }

bool UnlabelledGraph::add_edge( const uint32_t u, const uint32_t v ) {
	if( adjacency_list_[ u ].count( v ) > 0 || u == v ) { return false; }
	if( adjacency_list_[ v ].count( u ) > 0 ) { return false; }
	adjacency_list_[ u ].insert( v );
	adjacency_list_[ v ].insert( u );
	++m_;
	return true;
}

void UnlabelledGraph::add_vertices( const uint32_t num_vertices ) {

	n_ += num_vertices;
	adjacency_list_.resize( n_ );
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
	for( auto const& e : possible_edges ) {
		if ( add_edge( e.first, e.second ) ) {
			if( ++num_added == num_edges ) { return true; } /* Done! */
		}
	}
	return false; /* should be an unreachable statement! */
}

bool UnlabelledGraph::is_complete() const { return m_ == n_ * ( n_ - 1 ); }

bool UnlabelledGraph::is_anonymous( const uint32_t k ) const {

	/* First calculate the counts for every degree in the graph. */
	std::unordered_map< uint32_t, uint32_t > degree_counts;
	for( auto const& neighbours : adjacency_list_ ) {
		const uint32_t next_degree = neighbours.size();
		if( degree_counts.count( next_degree ) == 0 ) {
			degree_counts[ next_degree ] = 1;
		}
		else { ++degree_counts[ next_degree ]; }
	}
	
	/* Then ensure every count is at least k. */
	for( auto const count : degree_counts ) {
		if( count.second < k ) { return false; }
	}
	return true;
}

float UnlabelledGraph::get_occupancy() const {
	if( n_ == 0 ) { return 0; }
	else return m_ / static_cast< float >( ( n_ * ( n_ - 1 ) ) * 2 ); /* x2 because undirected */
}


DegreeSequence UnlabelledGraph::retrieve_degree_sequence() const {
	
	/* First create list of pairs. */
	DegreeSequence degrees;
	uint32_t i = 0;
	std::transform( adjacency_list_.cbegin(), adjacency_list_.cend(), std::back_inserter( degrees ),
		[ &i ]( auto const& neighbour_list ) {
		 return std::make_pair( neighbour_list.size(), i++ );
		}
	);
	
	/* Then sort them by descending degree. */
	std::sort( degrees.begin(), degrees.end(), 
		std::greater< std::pair< uint32_t, uint32_t > >() );

	return degrees;
}


int UnlabelledGraph::calculate_path_length( uint32_t u, uint32_t v ) const {
	std::unordered_set< uint32_t > visited;
	std::queue< std::pair< uint32_t, uint32_t > > q; /* (vertex, path length) pairs. */
	
	/* Check if source and destination are the same. */
	if( u == v ) { return 0; }
	
	/* Insert all direct neighbours into the visited list. */
	for( auto const neighbour : adjacency_list_[ u ] ) {
	
		/* If neighbour is v, we are done. */
		if( neighbour == v ) { 
			return 1; 
		}
		/* Otherwise, push it onto the queue for revisiting in breadth-first order. */
		else {
			q.push( std::make_pair( v, 1 ) );
			visited.insert( v );
		}
	}
	
	while( !q.empty() ) {
	
		/* Pop top off the queue. */
		const uint32_t vertex = q.front().first;
		const uint32_t num_hops = q.front().second;
		q.pop();
		
		/* Iterate neighbours of vertex to see if they are v. */
		for( auto const neighbour : adjacency_list_[ vertex ] ) {
			/* First check if we have found our destination. */
			if( neighbour == v ) { return num_hops + 1; }
			
			/* Otherwise, push it onto the queue if we have not already visited it. */
			else if( visited.count( neighbour ) == 0 ) {
				q.push( std::make_pair( neighbour, num_hops + 1 ) );
				visited.insert( neighbour );
			}
		}
	} 
	return -1;
}

float UnlabelledGraph::clustering_coefficient() const {
	
	uint64_t closed_triangles = 0;

	/* First count denominator -- how many open triangles exist. */
	uint64_t possible_triangles = std::accumulate( 
		adjacency_list_.cbegin(), adjacency_list_.cend(), 0llu,
		[]( auto const sum, auto const& neighbour_list )
		{
			return sum + neighbour_list.size() * ( neighbour_list.size() - 1 );
		}
	);

	
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
	
	return closed_triangles / static_cast< float >( possible_triangles );
}

float UnlabelledGraph::clustering_coefficient_brute_force() const {
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


HopPlot UnlabelledGraph::hop_plot() const {

	std::unordered_set< uint32_t > visited;
	std::queue< std::pair< uint32_t, uint32_t > > q; /* (vertex, path length) pairs. */	
	uint32_t num_threads;
	
#pragma omp parallel 
	{
		num_threads = omp_get_num_threads();
	}
	std::vector< HopPlot > hopplots( num_threads );
	
#pragma omp parallel for private( visited, q )
	for( uint32_t i = 0; i < n_; ++i ) {
		
		HopPlot & my_hop_plot = hopplots[ omp_get_thread_num() ];

		/* clear queue and visited set, although set i so we don't loop. */
		visited.clear();
		visited.insert( i );
	
		/* Init queue to contain all direct neighbours of vertex i. */
		for( auto const neighbour : adjacency_list_[ i ] ) {
			q.push( std::make_pair( neighbour, 1 ) );
			visited.insert( neighbour );
		}
		/* Add all neighbours of i to hop plot score = 1. */
		if( my_hop_plot.count( 1 ) == 1 ) { my_hop_plot.at( 1 ) += adjacency_list_[ i ].size(); }
		else { ( my_hop_plot)[ 1 ] = adjacency_list_[ i ].size(); }
	
		/* Iterate breadth-first through remaining paths. */
		while( ! q.empty() ) {
			const uint32_t v = q.front().first;
			const uint32_t d = q.front().second;
			q.pop();
		
			for( auto const neighbour : adjacency_list_[ v ] ) {
				if( visited.count( neighbour ) == 0 ) {
					visited.insert( neighbour );
					q.push( std::make_pair( neighbour, d + 1 ) );

					if( my_hop_plot.count( d + 1 ) == 0 ) { my_hop_plot[ d + 1 ] = 1; }
					else { ++my_hop_plot.at( d + 1 ); }
				}
			}
		}
	}
		
	/* Reduce all the hop plots from each thread. */
	auto & result = hopplots[ 0 ];
	std::for_each( std::next( hopplots.cbegin(), 1 ), hopplots.cend(),
		[ &result ]( auto const& hopplot )
		{
			for( auto const& hp_entry : hopplot ) {
				if( result.count( hp_entry.first ) == 0 )
				{
					result.insert( std::make_pair( hp_entry.first, hp_entry.second ) );
				}
				else
				{
					result[ hp_entry.first ] += hp_entry.second;
				}
			}
	} );
	return result;
}


float UnlabelledGraph::harmonic_mean( HopPlot const& hop_plot ) const {
	
	float const mean = std::accumulate( hop_plot.cbegin(), hop_plot.cend(), 0.0,
		[]( float const f, auto const& hp )
		{
			return f + hp.second / static_cast< float >( hp.first );
		}
	);

	return mean == 0 ? -1.0 : n_ * ( n_ - 1 ) / mean;
}

/* Computes sc by repeatedly exponentiating matrix and summing diagonals. */
double UnlabelledGraph::subgraph_centrality( const uint32_t limit ) const {
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
		
		for( auto const neighbour : adjacency_list_[ i ] ) {
			adjacency_matrix[ offset + neighbour ] = 1;
			adjacency_matrix_to_lth[ offset + neighbour ] = 1;
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
		std::swap( adjacency_matrix_to_lth, new_values );
	}
	
	delete [] adjacency_matrix;
	delete [] adjacency_matrix_to_lth;
	delete [] new_values;
	return summation / n_;
}

std::ostream& operator << ( std::ostream& os, UnlabelledGraph const& g )
{
	// Just ignoring the vertex labels	
	os << g.n_ << std::endl;

	for( uint32_t u = 0; u < g.n_; ++u ) {
		for( uint32_t const v : g.adjacency_list_[ u ] )
		{
			if( u <= v ) // only print undirected
			{
				if( g.io_format_ == graphAnon::FileFormat::edgeList )
				{
					os << u << " " << v << std::endl;
				}
				else
				{
					os << v << " ";
				}
			}
		}
		if( g.io_format_ != graphAnon::FileFormat::edgeList )
		{	
			os << std::endl;
		}
	}
	return os;
}
