/**
 * @file
 * @brief The main driver for the AlphaProximity suite.
 * It parses user input to generate and anonymise a Graph.
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
 *
 * @mainpage
 * <b>AlphaProximity Suite, version 1.0</b><br />
 * This is the AlphaProximity suite, an implementation of the algorithms and experiments <br />
 * that appeared in the 2011 article "Social network privacy for attribute disclosure attacks" <br />
 * by S. Chester and G. Srivastava @cite asonam . which introduces a greedy algorithm for <br />
 * calculating alpha-proximal graphs
 * that are resistant to attribute disclosure attacks.
 *                  <br /><br />
 * An attribute disclosure attack occurs when an adversary targets an individual in a network <br />
 * not necessarily with the intention of identifying the target, but instead to learn a sensitive <br />
 * attribute. For example, an adversary may not need to identify you to learn your political <br />
 * affiliation; it can sometimes be sufficient to learn the distribution of political affiliations <br />
 * among your friends. This can perhaps suggest a higher probability estimate of <br />
 * your political affiliation than the adversary would have known prior to the attack. <br />
 * For more information, you are encouraged to read the five page article.
 *                  <br /><br />
 * This documentation describes the code. For licensing details, consult the LICENSE <br />
 * file in the root directory. For general installation instructions, consult the README <br />
 * file, also in the root directory. The code consists primarily of a Graph class. The <br />
 * greedy() and hopeful() member functions anonymise a Graph instance using Algorithm 1 <br />
 * from the paper and a naive randomized approach, respectively. Running either anonymisation <br />
 * function will modify the Graph instance so that its global LabelDistribution is within a <br />
 * factor of &alpha; of the LabelDistribution for any neighbourhood in the Graph.
 *                  <br /><br />
 * If, after consulting this documentation, you still have questions about the code, please <br />
 * contact the author (schester@cs.au.dk).
 */

#include <iostream>		/* For std::cout, std::endl */
#include <algorithm>	/* For std::find */

#include "graph.h"
#include "label_distribution.test.h"

/**
 * Finds a specified option among the command line arguments
 * @param begin The first command line argument
 * @param end Pointer to the end of the command line argument list
 * @param option The specific option being searched (e.g., "-f")
 * @param has_val Indicates whether the parametre has a value or is
 * just a flag (e.g., -h is simply a flag saying "display help menu")
 * and so requires has_val = false, whereas -f requires a filename
 * argument and so requires has_val = true.
 * @see <a href="http://stackoverflow.com/questions/865668/parse-command-line-arguments#868894">
 * Stack Overflow solution</a>
 */
char* getCmdOption(char **begin, char **end, const std::string &option, bool has_val )
{
    char **itr = std::find(begin, end, option);
    if (itr != end ) {
    	if( !has_val ) { return *itr; }
    	else if( ++itr != end ) { return *itr; }
    }
    return 0;
}


void print_usage_instructions( const char *bin_path ) {
	std::cout << "Usage: "
			<< bin_path << " [-option value]" << std::endl << std::endl;
	std::cout << "\tPossible options include:" << std::endl;
	std::cout << "\t\t[-h] or [--help] shows these usage instructions" << std::endl;
	std::cout << "\t\t[-f [path to input file]]" << std::endl;
	std::cout << "\t\t[-o [path to output file]]" << std::endl;
	std::cout << "\t\t[-alpha [privacy threshold]]" << std::endl;
	std::cout << "\t\t[-n [number of vertices in random graph]]" << std::endl;
	std::cout << "\t\t[-occ [occupancy rate in random graph (i.e., percentage of possible edges)]]" << std::endl;
	std::cout << "\t\t[-l [label set size in random graph]]" << std::endl << std::endl;
	std::cout << "\tNote that if an input file is specified, all random graph parametres are ignored. " << std::endl
			<< "\tIf no input file is specified, -n, -occ, and -l are mandatory. " << std::endl
			<< "\t-alpha, the privacy threshold, is always mandatory." << std::endl << std::endl;
	std::cout << "\tExample usage:" << std::endl;
	std::cout << "\t\t" << bin_path << " -alpha 0.10001 -f ./workloads/paper_example.adjList -o private_graph.adjList" << std::endl;
	std::cout << "\t\t" << bin_path << " -alpha 0.05 -n 100 -occ .01 -l 2" << std::endl << std::endl;
	std::cout << "\tOutput:" << std::endl;
	std::cout << "\t\tThe input graph is made alpha-secure from a neighbourhood attribute disclosure (NAD) " << std::endl;
	std::cout << "\t\tattack. The extent to which the graph is modified is echoed to stdout in the form: " << std::endl;
	std::cout << "\t\t[original occupancy] [final occupancy] [% change in occupancy]" << std::endl << std::endl;
	std::cout << "\tWarning:" << std::endl;
	std::cout << "\t\tThis software uses floating point values and some real values (e.g., 0.1) will be " << std::endl;
	std::cout << "\t\tapproximated. You may wish to consider adding a small correction factor (e.g., 0.00001) "  << std::endl;
	std::cout << "\t\tto your privacy threshold, alpha. " << std::endl << std::endl;
}

/**
 * Main driver method that constructs a new Graph, anonymises it, and
 * then reports the change to the graph's occupancy rate.
 * @param argc The number of command line arguments provided by the user
 * @param argv An array of strings, each string containing a command
 * line argument.
 * @returns 0 on graceful exit. (Theoretically, other values could have
 * been returned, but every path through this method returns 0).
 */
int main(int argc, char** argv) {

	Graph *g;

	/* Parse input parametres to create a graph. */
	if( argc == 1 || getCmdOption( argv, argv + argc, "-h", false ) > 0
			|| getCmdOption( argv, argv + argc, "--help", false ) > 0 ) {

		print_usage_instructions( *argv );
		return 0;
	}

	char *filename = getCmdOption( argv, argv + argc, "-f", true );
	char *alpha = getCmdOption( argv, argv + argc, "-alpha", true );
	if( alpha == 0 ) {

		//print_usage_instructions( *argv );
		std::cerr << std::endl
				<< "\tYou must specify a value for alpha (e.g., -alpha 0.1)"
				<< std::endl;
		return 0;
	}
	if( filename != 0 ) {
		g = new Graph( filename );
	}
	else {
		/* Gather parametres for a random graph */
		char *graph_size = getCmdOption( argv, argv + argc, "-n", true );
		char *occupancy = getCmdOption( argv, argv + argc, "-occ", true );
		char *alphabet_size = getCmdOption( argv, argv + argc, "-l", true );
		if( graph_size > 0 && occupancy > 0 && alphabet_size > 0 ) {
			g = new Graph( atoi( graph_size ), atoi( alphabet_size ) );
			g->evenly_distribute_labels();
			const uint32_t num_edges = atof( occupancy ) * atoi( graph_size ) *
					( atoi( graph_size ) - 1 ) / 2; /* 1/2 b/c undirected */
			g->populate_uniformly( num_edges );
		}
		else {
			//print_usage_instructions( *argv );
			std::cerr << std::endl
					<< "\tYou must specify all values for the random graph "
					<< "or provide an input file (e.g., -n 100 =occ .01 -l 2)"
					<< std::endl;
			return 0;
		}
	}
	const float original_occupancy = g->get_occupancy();

	/* Run unit tests first. */
	if( !test_distance() ) {
		std::cerr << "Failed unit test of LabelDistribution" <<
				" distance function! Aborting." << std::endl;
		exit(0);
	}

	/* Execute algorithm. */
	g->greedy( atof( alpha ) );
	if( !g->is_alpha_proximal( atof( alpha ) ) ) {
		std::cerr << "This instance was not evidently not solved. ";
		std::cerr << "The software must have a bug?";
		exit(0);
	}

	/* Echo output (final occupancy, and % occupancy change). Note that
	 * the paper @cite asonam uses the relative difference in occupancy
	 * as the evaluation measure, not the absolute difference and
	 * the reported results here are as in the paper.
	 */
	const float new_occupancy = g->get_occupancy();
	const float occupancy_change = ( new_occupancy - original_occupancy ) / original_occupancy;
	std::cout << original_occupancy << " " << new_occupancy
			<< " " << occupancy_change << std::endl;


	/* If requested in command line args, write output Graph to file. */
	char *output_filename = getCmdOption( argv, argv + argc, "-o", true );
	if( output_filename > 0 ) {
		std::ofstream outfile;
		outfile.open( output_filename );
		g->print( &outfile );
		outfile.close();
	}

	delete g;
	return 0;
}
