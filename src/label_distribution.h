/**
 * @file
 * @brief The definition of a Label Distribution (Definition 2.3 in @cite asonam)
 * @see label_distribution.test.h (The unit test suite for this class)
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

#ifndef LABEL_DISTRIBUTION_H_
#define LABEL_DISTRIBUTION_H_

/* STL libraries in use. */
#include <vector>

/**
 * A sentinel value indicating that two LabelDistribution objects cannot
 * be compared to each other
 */
#define LD_INCOMPARABLE -1

/**
 * @brief A mapping from a set of labels to the frequency with which each occurs.
 */
class LabelDistribution {
public:

	/**
	 * Constructs a new LabelDistribution of length n with all frequencies
	 * initialized to zero.
	 * @param n The number of unique labels represented by the LabelDistribution
	 */
	LabelDistribution( const uint32_t n );

	/**
	 * Constructs a new LabelDistribution from a vector of initial frequencies
	 * @param counts A vector indicating the frequencies for each label in the
	 * LabelDistribution
	 */
	LabelDistribution( std::vector< uint32_t > *counts );

	/**
	 * Destroys the LabelDistribution instance.
	 */
	virtual ~LabelDistribution();

	/**
	 * Accessor method to return the number of labels represented in
	 * this LabelDistribution
	 * @return The number of frequencies stored in the LabelDistribution
	 */
	uint32_t get_length();

	/**
	 * Accessor method to return the relative frequency of a given label
	 * in the LabelDistribution.
	 * @param pos The index/position of the label of interest
	 * @return The relative frequency of the pos'th label. If there is
	 * no pos'th label (e.g., an index is provided that is greater than the
	 * size of the array), then the return value is 0.
	 */
	float get_frequency( const uint32_t pos );

	/**
	 * Calculates the distance from this LabelDistribution to another.
	 * @param another The other LabelDistribution against which the distance
	 * computation should be calculated.
	 * @see Definition 2.4 in @cite asonam
	 * @return The distance between this LabelDistribution and another. If the
	 * LabelDistributions are not of the same length, and thus incomparable,
	 * LD_INCOMPARABLE is returned.
	 */
	float distance( LabelDistribution *another );

	/**
	 * Determines, relative to another LabelDistribution, in which labels this
	 * LabelDistribution is lacking.
	 * @param another The reference LabelDistribution (typically the global
	 * distribution)
	 * @param alpha The privacy threshold
	 * @return A bitmask indicating the deficient labels, if the vertex does not
	 * already have an alpha-proximal neighbourhood. (If the neighbourhood is alpha-
	 * proximal, then 0/no deficiencies ire returned.) If the i'th bit
	 * (counting from the rightmost, least significant bit)
	 * is set in the bitmask, then another has a higher relative frequency
	 * of the i'th label than does this LabelDistribution.
	 * @warning Assumes the alphabet size for the set of labels is <= 32,
	 * because the bitmask returned is a 32-bit unsigned integer. For larger
	 * alphabets, a new implementation is needed.
	 * @warning Behaviour is undefined if this LabelDistribution and another
	 * have different lengths
	 */
	uint32_t get_deficiencies( LabelDistribution *another, const float alpha );

	/**
	 * Echoes the LabelDistribution to stdout. Primarily for the purpose
	 * of testing.
	 */
	void print();

private:
	/**
	 * The mapping between labels and their (absolute) frequencies
	 */
	std::vector< uint32_t > frequencies_;
	/**
	 * The sum of all (absolute) frequencies in the LabelDistribution,
	 * for the sake of calculating relative frequencies and distances
	 * between separate LabelDistribution objects.
	 */
	uint32_t sum_;
};

#endif /* LABEL_DISTRIBUTION_H_ */
