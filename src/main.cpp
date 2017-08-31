/**
 * @file
 * @brief The main driver for the GraphAnon suite.
 * It parses user input to generate and anonymise a Graph.
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

#include <iostream>		/* For std::cout, std::endl */
#include <algorithm>	/* For std::find */
#include <string.h>		/* For strcmp() */

#include "labelled_graph/labelled_graph.h"
#include "unlabelled_graph/unlabelled_graph.h"
#include "labelled_graph/label_distribution.test.h"

/* STL containers in use */
#include <map>

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
	std::cout << "\t\t[-mode {identity,attribute} [type of anonymization to conduct]]" << std::endl;
	std::cout << "\t\t[-f [path to input file]]" << std::endl;
	std::cout << "\t\t[-format {adjList, edgeList, adjListVL} [format to read/write "
		<< "input/output files (adjList by default)]]" << std::endl;
	std::cout << "\t\t[-o [path to output file]]" << std::endl;
	std::cout << "\t\t[-k [identity privacy threshold]]" << std::endl;
	std::cout << "\t\t[-alpha [attribute privacy threshold]]" << std::endl;
	std::cout << "\t\t[-n [number of vertices in random graph]]" << std::endl;
	std::cout << "\t\t[-occ [occupancy rate in random graph (i.e., percentage of possible edges)]]" << std::endl;
	std::cout << "\t\t[-l [label set size in random graph]]" << std::endl;
	std::cout << "\t\t[-stats [enables printing of graph properties to stdout]]" << std::endl;
	std::cout << "\t\t[-hide-additional [enables the anonymisation of newly added vertices]]" << std::endl << std::endl;
	std::cout << "\tNote that if an input file is specified, all random graph parametres are ignored. " << std::endl
			<< "\tIf no input file is specified, -n, -occ, and -l are mandatory. " << std::endl
			<< "\t-alpha, the privacy threshold, is always mandatory." << std::endl << std::endl;
	std::cout << "\tExample usage:" << std::endl;
	std::cout << "\t\t" << bin_path << " -mode attribute -alpha 0.10001 -f ./workloads/asonam11_example.adjList -o private_graph.adjList" << std::endl;
	std::cout << "\t\t" << bin_path << " -mode attribute -alpha 0.05 -n 100 -occ .01 -l 2" << std::endl << std::endl;
	std::cout << "\t\t" << bin_path << " -mode identity -k 3 -f ./workloads/snam_example1.adjList -o anon_graph.adjList -stats" << std::endl << std::endl;
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
 * Echoes to stdout statistics (namely clustering coefficient, 
 * hop plot, and average path length) for a graph.
 * @param g The graph for which to print out statistics.
 */
void inline print_stats( UnlabelledGraph *g ) {
	std::cout << "|V|: " << g->num_vertices() << std::endl;
	std::cout << "|E|: " << g->num_edges() << std::endl;
	std::cout << "Occ: " << g->get_occupancy() << std::endl;
	std::cout << " CC: " << g->clustering_coefficient() << std::endl;
	std::cout << " SC: " << g->subgraph_centrality( 120 ) << std::endl;
	HopPlot hop_plot;
	g->hop_plot( &hop_plot );
	std::cout << " HP: ";
	for( auto it = hop_plot.begin(); it != hop_plot.end(); ++it ) { std::cout << it->first << ":" << it->second << " "; }
	std::cout << std::endl;
	std::cout << "APL: " << g->average_path_length< true >( &hop_plot ) << std::endl;
	std::cout << " HM: " << g->harmonic_mean( &hop_plot ) << std::endl;
}


/**
 * Runs the software to create a alpha-proximal graph, 
 * according to command-line specifications.
 * @param argc The number of command line arguments provided by the user
 * @param argv An array of strings, each string containing a command
 * line argument.
 * @returns <ul><li>0 on successful computation</li>
 * <li>1 if there is an error in the user input</li>
 * <li>2 if there is a software error (either a unit test 
 * fails or the algorithm fails to produce an alpha-proximal graph)</li></ul>
 */
uint32_t run_attribute_mode( int argc, char** argv ) {

	LabelledGraph *g;

	char *filename = getCmdOption( argv, argv + argc, "-f", true );
	char *alpha = getCmdOption( argv, argv + argc, "-alpha", true );
	if( alpha == 0 ) {

		//print_usage_instructions( *argv );
		std::cerr << std::endl
				<< "\tYou must specify a value for alpha (e.g., -alpha 0.1)"
				<< std::endl;
		return 1;
	}
	if( filename != 0 ) {
		g = new LabelledGraph( filename );
		assert( g != NULL );
	}
	else {
		/* Gather parametres for a random graph */
		char *graph_size = getCmdOption( argv, argv + argc, "-n", true );
		char *occupancy = getCmdOption( argv, argv + argc, "-occ", true );
		char *alphabet_size = getCmdOption( argv, argv + argc, "-l", true );
		
		size_t const n = atoi( graph_size );
		size_t const l = atoi( alphabet_size );
		float const occ = atof( occupancy );
		
		if( n > 0 && occ > 0 && l > 0 ) {
			g = new LabelledGraph( n, l );
			g->evenly_distribute_labels();
			const uint32_t num_edges = occ * n * ( n - 1 ) / 2; /* 1/2 b/c undirected */
			g->populate_uniformly( num_edges );
		}
		else {
			//print_usage_instructions( *argv );
			std::cerr << std::endl
					<< "\tYou must specify all values for the random graph "
					<< "or provide an input file (e.g., -n 100 =occ .01 -l 2)"
					<< std::endl;
					
			return 1;
		}
	}

	/* Run unit tests first. */
	if( !test_distance() ) {
		std::cerr << "Failed unit test of LabelDistribution" <<
				" distance function! Aborting." << std::endl;
				
		delete g;
		return 2;
	}
	

	/* Execute algorithm. */
	g->greedy( atof( alpha ) );
	if( !g->is_alpha_proximal( atof( alpha ) ) ) {
		std::cerr << "This instance was evidently not solved. ";
		std::cerr << "The software must have a bug? ";
		std::cerr << "You should contact the developer.";
		
		delete g;
		return 2;
	}

	/* If requested in command line args, echo to stdout the orig graph stats. */
	char *stats = getCmdOption( argv, argv + argc, "-stats", false );
	if( stats != NULL ) { print_stats( g ); }


	/* If requested in command line args, write output Graph to file. */
	char *output_filename = getCmdOption( argv, argv + argc, "-o", true );
	if( output_filename != NULL ) {
		std::ofstream outfile;
		outfile.open( output_filename );
		outfile << *g;
		outfile.close();
	}
	
	/* clean up. */
	delete g;
	return 0;
}


/**
 * Runs the software to create a k-degree-anonymous graph, 
 * according to command-line specifications.
 * @param argc The number of command line arguments provided by the user
 * @param argv An array of strings, each string containing a command
 * line argument.
 * @returns <ul><li>0 on successful computation</li>
 * <li>1 if there is an error in the user input</li>
 * <li>2 if there is a software error (either a unit test 
 * fails or the algorithm fails to produce an alpha-proximal graph)</li></ul>
 */
uint32_t run_identity_mode( int argc, char** argv ) {

	UnlabelledGraph *g;

	char *filename = getCmdOption( argv, argv + argc, "-f", true );
	char *k = getCmdOption( argv, argv + argc, "-k", true );
	
	
	if( k == 0 ) {

		std::cerr << std::endl
				<< "\tYou must specify a privacy threshold, k (e.g., -k 5)"
				<< std::endl;
		return 1;
	}
	
	if( filename != 0 ) {
		char *format = getCmdOption( argv, argv + argc, "-format", true );
		if( format == 0 || strcmp( format, "adjList" ) == 0 ) {
			g = new UnlabelledGraph( filename, graphAnon::FileFormat::adjacencyList );
		}
		else if( strcmp( format, "edgeList" ) == 0 ) {
			g = new UnlabelledGraph( filename, graphAnon::FileFormat::edgeList );
		}
		else if( strcmp( format, "adjListVL" ) == 0 ) {
			g = new UnlabelledGraph( filename, graphAnon::FileFormat::adjacencyListVertexLabelled );
		}
		else {
			std::cerr << std::endl
				<< "\tFormat \"" << format << "\" not supported."
				<< std::endl;
			
			return 1;
		}
		assert( g != NULL );
	}
	else {
		/* Gather parametres for a random graph */
		char *graph_size = getCmdOption( argv, argv + argc, "-n", true );
		char *occupancy = getCmdOption( argv, argv + argc, "-occ", true );
		
		size_t const n = atoi( graph_size );
		float const occ = atof( occupancy );
		
		if( n > 0 && occ > 0 ) {
			g = new UnlabelledGraph( n );
			const uint32_t num_edges = occ * n * ( n - 1 ) / 2; /* 1/2 b/c undirected */
			g->populate_uniformly( num_edges );
		}
		else {
			std::cerr << std::endl
					<< "\tYou must specify all values for the random graph "
					<< "or provide an input file (e.g., -n 100 =occ .01 -l 2)"
					<< std::endl;
					
			return 1;
		}
		assert( g != NULL );
	}
	
	/* Determine whether or not all vertices should be hidden. */
	char *hide_all = getCmdOption( argv, argv + argc, "-hide-additional", false );
	
	/* Execute algorithm. */
	if( hide_all != NULL ) {
		g->hide_waldo< true >( atoi( k ) );
		if( !g->is_anonymous( atoi( k ) ) ) {
			std::cerr << "This instance was evidently not solved. ";
			std::cerr << "Did you ensure k <= n?" << std::endl;
		
			delete g;
			return 2;
		}
	}
	else { g->hide_waldo< false >( atoi( k ) ); }

	/* If requested in command line args, echo to stdout the anon graph stats. */
	char *stats = getCmdOption( argv, argv + argc, "-stats", false );
	if( stats != NULL ) { print_stats( g ); }
	
	/* If requested in command line args, write output Graph to file. */
	char *output_filename = getCmdOption( argv, argv + argc, "-o", true );
	if( output_filename != NULL ) {
		std::ofstream outfile;
		outfile.open( output_filename );
		outfile << *g;
		outfile.close();
	}
	
	/* clean up. */
	delete g;
	return 0;
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

	/* Parse input parametres to create a graph. */
	if( argc == 1 || getCmdOption( argv, argv + argc, "-h", false ) != NULL
			|| getCmdOption( argv, argv + argc, "--help", false ) != NULL ) {

		print_usage_instructions( *argv );
		return 0;
	}

	char *mode = getCmdOption( argv, argv + argc, "-mode", true );
	if( mode == NULL ) {
			//print_usage_instructions( *argv );
			std::cerr << std::endl
					<< "\tYou must specify an operation mode (e.g., -mode attribute)"
					<< std::endl;
			return 0;
	}
	
	if( strcmp( mode, "attribute" ) == 0 ) {
		run_attribute_mode( argc, argv );
	}
	else if( strcmp( mode, "identity" ) == 0) {
		run_identity_mode( argc, argv );
	}
	else {
		std::cerr << "Mode \"" << mode << "\" not supported. Please try either ";
		std::cerr << "\"identity\" or \"attribute\" instead." << std::endl;
	}

	return 0;
}
