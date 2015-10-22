/**
 * @file
 * @brief A set of functions for unit testing the LabelDistribution class.
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


