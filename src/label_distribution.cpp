/**
 * @file
 * @brief An implementation of the LabelDistribution class.
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

#include <cstdint>	/* for uint32_t */
#include <iostream>	/* for cout, endl */

#include "label_distribution.h"

LabelDistribution::LabelDistribution( const uint32_t n ) {
	for( uint32_t i = 0; i < n; ++i ) {
		frequencies_.push_back( 0 );
	}
	sum_ = 0;
}

LabelDistribution::LabelDistribution( std::vector< uint32_t > *counts ) {
	sum_ = 0;
	for( auto it = counts->begin(); it != counts->end(); ++it ) {
		frequencies_.push_back( *it );
		sum_ += *it;
	}
}

LabelDistribution::~LabelDistribution( ) { }

float LabelDistribution::get_frequency( const uint32_t pos ) {
	if( pos > frequencies_.size() || sum_ == 0 ) { return 0; }
	else return frequencies_[ pos ] / (float) sum_;
}

uint32_t LabelDistribution::get_length( ) { return frequencies_.size(); }

float LabelDistribution::distance( LabelDistribution *another ) {
	float distance = 0;

	/* Error checking. */
	const uint32_t my_length = frequencies_.size();
	if( my_length != another->get_length() ) { return LD_INCOMPARABLE; }

	/* Pairwise comparison of values. */
	for( uint32_t i = 0; i < my_length - 1; ++i ) {
		const float label_distance = get_frequency( i ) - another->get_frequency( i );
		distance += label_distance > 0 ? label_distance: -1 * label_distance; /* take absolute value. */
	}
	return distance;
}

uint32_t LabelDistribution::get_deficiencies( LabelDistribution *another, const float alpha ) {

	/* cur_bit tracks the current bit of the bitmask (shifting left once
	 * for each label/iteration. i keeps track of which label is currently
	 * begin processed, and controls the number of iterations of the loop.
	 */
	uint32_t deficiencies = 0;
	float difference = 0;

	/* Iterate the set of labels, detecting deficiencies and scoring alpha-proximity */
	const uint32_t num_labels = frequencies_.size();
	for( uint32_t i = 0, cur_bit = 1; i < num_labels; ++i ) {

		float pairwise_diff = another->get_frequency( i ) - get_frequency( i );
		if( pairwise_diff > 0 ) {
			/* the other has more, set bit and add to difference */
			deficiencies |= cur_bit;
			difference += pairwise_diff;
		}
		else { difference -= pairwise_diff; } /* invert sign for absolute value difference */

		cur_bit *= 2; /* shift current working bit in bitmask */
	}

	if( difference < alpha ) return 0; /* alpha-proximal */
	else return deficiencies;
}

void LabelDistribution::print() {
	if( sum_ == 0 ) { std::cout << std::endl; }
	else {
		for(auto it = frequencies_.begin(); it != frequencies_.end(); ++it ) {
			std::cout << ( *it / (float) sum_ ) << " ";
		}
		std::cout << std::endl;
	}
}
