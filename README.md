
|     GraphAnon, version 2.0       |
|:--------------------------------:|
|       © 2015 Sean Chester        |
|   (sean.chester@idi.ntnu.no)     |

------------------------------------


### Table of Contents 

  * [Introduction](#introduction)
  * [Requirements](#requirements)
  * [Installation](#installation)
  * [Input data format](#data-format)
  * [Documentation](#documentation)
  * [License](#license)
  * [Contact](#contact)
  

------------------------------------
### Introduction
<a name="introduction" ></a>

This is the GraphAnon software suite, version 2.0, with software for 
transforming a graph into supergraphs that are 
resistance to identity and attribute disclosure attacks. 
For more details about _attribute disclosure attacks_, you are
encouraged to read the short article, which presents the material 
implemented in the _attribute mode_ of this software:

> S. Chester and G. Srivastava. 2011. "Social network privacy for
>   attribute disclosure attacks." In: Proceedings of the 2011
>   International Conference on Advances in Social Networks Analysis
>   and Mining (ASONAM), pp. 445-449. 
>		doi: [10.1109/ASONAM.2011.105](https://dx.doi.org/10.1109/ASONAM.2011.105)

For more details about _identity disclosure attacks_, you are 
encouraged to read either of the following articles (the 
conference version or the expanded journal version), which presents 
the material implemented in the _identity mode_ of this software:

> S. Chester et al. 2013. "Why Waldo befriended the dummy? 
>   k-Anonymization of social networks with pseudo-nodes." 
>   Social Network Analysis and Mining 3(3): 381-399. Springer Vienna. 
>   doi: [10.1007/s13278-012-0084-6](https://dx.doi.org/10.1007/s13278-012-0084-6)

The earlier conference version:

> S. Chester et al. 2011. "k-Anonymization of social networks 
>   by vertex addition." In: Proceedings II of the 15th 
>   East-European Conference on Advances in Databases and 
>		Information Systems (ADBIS), pp. 107--116.
>   url: http://ceur-ws.org/Vol-789/paper11.pdf


------------------------------------
### Requirements
<a name="requirements" ></a>

GraphAnon relies on the following packages/libraries:

 * OpenMP for to parallelise the calculation of graph statistics
 * C++ 11 for newer STL containers such as unordered_set
 * CMake (version 3.4 or higher) for auto-generating a makefile


------------------------------------
### Installation
<a name="installation" ></a>

GraphAnon is built using `CMake`. Create an out-of-source directory 
(e.g., `bin/` or `debug/`) and then run `cmake ../src` to generate a makefile. 
Then run `make` to build the executable. You can optionally run `cmake` 
with the `-DCMAKE_BUILD_TYPE=Debug` or `-DCMAKE_BUILD_TYPE=Release` options to 
compile explicitly in _debug_ (for development) or _release_ (for performance) 
modes, respectively.

To clean the project, you can simply delete the out-of-source directory that 
you created; e.g., `rm -rf bin`.


------------------------------------
### Input data format
<a name="data-format" ></a>

The default input format is an _adjacency list_ format. 
The first line gives the number of vertices in the file.
Each subsequent line _i_ includes a space-separated list of all nodes _j_ for which the edge _(i,j)_ exists, 
beginning with _i=0_.
For example, the (undirected) [Diamond Graph](https://en.wikipedia.org/wiki/Diamond_graph), with nodes numbered 
anti-clockwise starting from the bottom-most node, would be represented by the following adjacency list:

```
4
1 2 3
0 2
0 1 3
1 3
```

More examples can be found in the `workloads/` directory.



------------------------------------
### Documentation
<a name="documentation" ></a>

The code has been documented for `doxygen`. If the `doc/html/` 
directory is empty or stale, you can regenerate the documentation 
by running `doxygen Doxyfile` from within the `doc/` subdirectory.
The `doxygen` settings are included in `doc/Doxyfile` and can be 
freely modified to suit your preferences.


------------------------------------
### License
<a name="license" ></a>

Copyright (c) 2015 Sean Chester

GraphAnon, version 2.0, is distributed freely under the *MIT License*:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


------------------------------------
### Contact
<a name="contact"></a>

This software suite may be updated and so you are encouraged to check  
[GraphAnon on GitHub](https://github.com/sean-chester/graphAnon) to ensure 
this is the latest version. Do not hesitate to contact the author 
if you have comments, questions, or bugs to report, but please first 
consult the documentation.

------------------------------------
