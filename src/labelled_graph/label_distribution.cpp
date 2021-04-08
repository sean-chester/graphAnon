/**
 * @file
 * @brief An implementation of the LabelDistribution class.
 *
 * @date 22 Oct 2015
 * @version 1.1
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
	if( pos >= frequencies_.size() || sum_ == 0 ) { return 0; }
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
