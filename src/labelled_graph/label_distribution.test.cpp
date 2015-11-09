/**
 * @file
 * @brief A set of functions for unit testing the LabelDistribution class.
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

#include <cstdint> /* for uint32_t */
#include <iostream>
#include <iomanip>
#include <vector>

#include "label_distribution.test.h"
#include "label_distribution.h"

bool test_distance() {

	bool passed = true;

	/**
	 * @test Example from paper
	 * Definition 2.4 of @cite asonam contains an example of the distance
	 * function. This test case is exactly that example.
	 * The distance between <0.7, 0.2, 0.1> and <0.2, 0.4, 0.4> should be 0.7
	 * (the pairwise absolute difference between the first n-1 elements).
	 */
	std::vector< uint32_t > l1_counts { 7, 2, 1 };
	std::vector< uint32_t > l2_counts { 2, 4, 4 };
	LabelDistribution *l1 = new LabelDistribution( &l1_counts );
	LabelDistribution *l2 = new LabelDistribution( &l2_counts );
	if( l1->distance( l2 ) < 0.699999 || l1->distance( l2 ) > 0.700001 ) { passed = false; }
	delete l2;
	delete l1;

	/**
	 * @test Boundary case: only one label
	 * This test case checks boundary case handling for a
	 * LabelDistribution that has only one element. The
	 * distance between < 5 > and < 9 > should be 0, because
	 * the first label has the same relative frequency.
	 */
	std::vector< uint32_t > l1_onelabel { 5 };
	std::vector< uint32_t > l2_onelabel { 9 };
	l1 = new LabelDistribution( &l1_onelabel );
	l2 = new LabelDistribution( &l2_onelabel );
	if( l1->distance( l2 ) < -0.000001 || l1->distance( l2 ) > 0.000001 ) { passed = false; }
	delete l2;
	delete l1;

	/**
	 * @test Malformed input: inequal lengths
	 * This test case ensures that LabelDistributions with inequal
	 * lengths will throw the LD_INCOMPARABLE error condition.
	 */
	std::vector< uint32_t > l1_badlengths { 5 };
	std::vector< uint32_t > l2_badlengths { 9, 4 };
	l1 = new LabelDistribution( &l1_badlengths );
	l2 = new LabelDistribution( &l2_badlengths );
	if( l1->distance( l2 ) != LD_INCOMPARABLE ) { passed = false; }
	delete l2;
	delete l1;


	return passed;
}


