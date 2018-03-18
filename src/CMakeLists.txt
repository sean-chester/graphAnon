cmake_minimum_required( VERSION 3.4 )
project( graphAnon )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp -march=native -mavx -Wall -Wextra -Werror" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og" )
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3" )
set( basepath "${CMAKE_SOURCE_DIR}/.." )

include_directories( "${basepath}/include" )

add_subdirectory( labelled_graph )
add_subdirectory( unlabelled_graph )

add_executable( graphAnon main.cpp )
target_link_libraries( graphAnon labelled_graph unlabelled_graph )
