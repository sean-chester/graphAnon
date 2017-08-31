/**
 * @file Implementation of templated functions for unlabelled_graph.h
 *
 * @date 6 Nov 2015
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

#include <unordered_set>
#include <queue>
#include <cassert>

#include "omp.h"

int inline UnlabelledGraph::calculate_path_length( uint32_t u, uint32_t v ) {
	std::unordered_set< uint32_t > visited;
	std::queue< std::pair< uint32_t, uint32_t > > q; /* (vertex, path length) pairs. */
	
	/* Check if source and destination are the same. */
	if( u == v ) { return 0; }
	
	/* Insert all direct neighbours into the visited list. */
	for( auto it = adjacency_list_[ u ].begin(); it != adjacency_list_[ u ].end(); ++it ) {
	
		/* If neighbour is v, we are done. */
		if( *it == v ) { 
			return 1; 
		}
		/* Otherwise, push it onto the queue for revisiting in breadth-first order. */
		else {
			q.push( std::pair< uint32_t, uint32_t >( *it, 1 ) );
			visited.insert( *it );
		}
	}
	
	while( !q.empty() ) {
	
		/* Pop top off the queue. */
		const uint32_t vertex = q.front().first;
		const uint32_t num_hops = q.front().second;
		q.pop();
		
		/* Iterate neighbours of vertex to see if they are v. */
		std::unordered_set< uint32_t > *neighbours = &( adjacency_list_[ vertex ] );
		for( auto it = neighbours->begin(); it != neighbours->end(); ++it ) {
			/* First check if we have found our destination. */
			if( *it == v ) { return num_hops + 1; }
			
			/* Otherwise, push it onto the queue if we have not already visited it. */
			else if( visited.count( *it ) == 0 ) {
				visited.insert( *it );
				q.push( std::pair< uint32_t, uint32_t >( *it, num_hops + 1 ) );
			}
		}
	} 
	return -1;
}

template <bool include_self_paths >
float UnlabelledGraph::average_path_length_brute_force() {
	uint32_t sum_of_path_lengths = 0;
	uint32_t number_of_connected_paths = 0;
	
	/* Iterate all pairs of distinct vertices. */
#pragma omp parallel for reduction ( +: sum_of_path_lengths, number_of_connected_paths )
	for( uint32_t u = 0; u < n_; ++u ) {
		for( uint32_t v = ( include_self_paths ? u : u + 1 ); v < n_; ++v ) {
			const int path_length = calculate_path_length( u , v );
			if( path_length >= 0 ) {
				if( u < v ) { /* double count when u<v, to account for path v->u too */
					number_of_connected_paths += 2;
					sum_of_path_lengths += 2 * path_length;
				}
				else { /* u == v */ 
					++number_of_connected_paths;
					sum_of_path_lengths += path_length;
				}
			}
		}
	}
	
	if ( number_of_connected_paths > 0  ) {
		return sum_of_path_lengths / (float) number_of_connected_paths;
	}
	else { return 0; }
}

template <bool include_self_paths >
float UnlabelledGraph::average_path_length( HopPlot *hop_plot ) {
	
	//return average_path_length_brute_force< include_self_paths >();
	
	/* init with/without the paths (i,i) of length 0 */
	uint64_t sum = 0;
	uint64_t count = ( include_self_paths ? n_ : 0 );
	
	/* Iterate hop plot to process all length > 0 paths */
	for( auto it = hop_plot->begin(); it != hop_plot->end(); ++it ) {
		if( it->second > 0 ) {
			sum += it->first * it->second;
			count += it->second;
		}
	}
	
	return ( count == 0 ? 0 : sum / (float) count );
}

/**
 * Optimally k-anonymizes the degree sequence such that max_deficiency is minimized.
 * @param degrees The original degree sequence as pairs of (degree, vertex id)
 * @param k The privacy threshold, k.
 * @return The maximum deficiency calculated to transform the original degree sequence 
 * into a k-anonymous one.
 * @post The degree sequence, degrees, is modified such that every element that appears 
 * in the list appears at least k times.
 * @see Section 3.1 and Table 1 of @cite waldo
 */
uint32_t inline anonymize_degree_sequence( DegreeSequence *degrees, const uint32_t k ) {
	
	const uint32_t n = degrees->size();
	
	// Check if the graph is large enough to meaningfully anonymise. 
	// Cannot split fewer than 2k vertices into two groups; so, a graph 
	// of n < 2k vertices must already be transformed into the complete graph.
	if( n < 2 * k )
	{
		uint32_t deficiency = 0;
		for( uint32_t i = 1; i < n; ++i )
		{
			deficiency += degrees->at( 0 ).first - degrees->at( i ).first;
		}
		return deficiency;
	}
	
	/* arrays to store dynamic programming results. */
	uint32_t costs[ n ];
	uint32_t starts[ n ];
	
	/* trivially populate first 2k - 1 positions, since cannot split. */
	for( uint32_t i = 0; i < 2 * k - 1; ++i ) {
		starts[ i ] = 0;
		costs[ i ] = degrees->at( 0 ).first - degrees->at( i ).first;
	}
	
	/* compute best split for remaining n - (2k - 1) positions. */
	for( uint32_t i = 2 * k - 1; i < n; ++i ) {
		const uint32_t range_end = i - k;
		const uint32_t range_start = ( k - 1 > i - 2 * k + 1 ? k - 1 : i - 2 * k + 1 );
		
		uint32_t best_split_pos = range_start + 1;
		
		const uint32_t cost_left = costs[ range_start ];
		const uint32_t cost_right = degrees->at( range_start + 1 ).first - degrees->at( i ).first;
		uint32_t best_cost = ( cost_left > cost_right ? cost_left : cost_right );
		uint32_t best_sum = cost_left + cost_right;
		
		for( uint32_t j = range_start + 1; j <= range_end; ++j ) {
			const uint32_t cost_left = costs[ j ];
			const uint32_t cost_right = degrees->at( j + 1 ).first - degrees->at( i ).first;
			const uint32_t full_cost = ( cost_left > cost_right ? cost_left : cost_right );
			const uint32_t sum_cost = cost_left + cost_right;
			if( full_cost < best_cost || ( full_cost == best_cost && sum_cost < best_sum ) ) {
				best_split_pos = j + 1;
				best_cost = full_cost;
				best_sum = sum_cost;
			}
		}
		starts[ i ] = best_split_pos;
		costs[ i ] = best_cost;
	}
	
	/* Update degrees to k-anonymize the degree sequence by replaying the
	 * dynamic programming results backwards. 
	 * Be aware of crazy loop logic arising from use of unsigned ints: 
	 * the termination condition is when i == -1, which for unsigned ints 
	 * means that i == max_int > n.
	 */
	for( uint32_t i = n - 1; i < n; i = starts[ i ] - 1 ) {
		const uint32_t block_start = starts[ i ];
		assert( block_start <= i );
		const uint32_t block_degree = degrees->at( block_start ).first;
		assert( block_degree <= n );
		for( uint32_t j = block_start + 1; j <= i; ++j ) {
			degrees->at( j ).first = block_degree;
		}
	}
	
	/* Return max deficiency. */
	return costs[ n - 1 ];
}

void inline UnlabelledGraph::retrieve_degree_sequence( DegreeSequence *degrees ) {
	
	/* First create list of pairs. */
	degrees->clear();
	for( uint32_t i = 0; i < n_; ++i ) {
		const uint32_t next_degree = adjacency_list_[ i ].size();
		degrees->push_back( std::pair< uint32_t, uint32_t >( next_degree, i ) );
	}
	
	/* Then sort them by descending degree. */
	std::sort( degrees->begin(), degrees->end(), 
		std::greater< std::pair< uint32_t, uint32_t > >() );
}

template < bool hide_new_vertices >
void UnlabelledGraph::hide_waldo( const uint32_t k ) {
	
	assert( k <= n_ );
	
	/* Section 3.1: First anonymize degree sequence. */
	DegreeSequence degrees;
	retrieve_degree_sequence( &degrees );
	DegreeSequence anon_degrees( degrees );
	uint32_t max_def = anonymize_degree_sequence( &anon_degrees, k );
	
	/* Section 3.2: Augment graph with min # vertices. */ 
	if( max_def > 0 ) {
		const uint32_t first_new_vertex = n_;
		if ( hide_new_vertices ) {
			const uint32_t md_or_k = ( max_def > k ? max_def : k );
			const uint32_t new_vertices = ( md_or_k % 2 ? md_or_k : md_or_k + 1 );
			add_vertices( new_vertices );
		}
		else { add_vertices( max_def ); }
	
		/* Section 3.3: Add new edges cyclically to anonymize original graph. */
		uint32_t cursor = first_new_vertex;
		for( uint32_t i = 0; i < first_new_vertex; ++i ) {
			const uint32_t deficiency = anon_degrees[ i ].first - degrees[ i ].first;
			for( uint32_t j = 0; j < deficiency; ++j ) {
				add_edge( degrees[ i ].second, cursor );
				if( cursor == n_ - 1 ) { cursor = first_new_vertex; }
				else { ++cursor; }
			}
		}
	
		/* finally, check whether the new vertices are k-anonymous, or whether the 
		 * pairing procedure is necessary. */
		if( hide_new_vertices && cursor != first_new_vertex ) {
			if( ! is_anonymous( k ) ) {
				while( cursor < n_ - 1 ) {
					add_edge( cursor, cursor + 1 );
					cursor += 2;
				}
				if( cursor == n_ - 1 ) {
					add_edge( n_ - 1, first_new_vertex );
					for( cursor = first_new_vertex + 1; cursor < n_; cursor += 2 ) {
						add_edge( cursor, cursor + 1 );
					}
				}
			}
		}
	}
	return;
}
