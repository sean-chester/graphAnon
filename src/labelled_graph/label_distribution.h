/**
 * @file
 * @brief The definition of a Label Distribution (Definition 2.3 in @cite asonam)
 * @see label_distribution.test.h (The unit test suite for this class)
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
