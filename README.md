

                   AlphaProximity suite, version 1.0

                         © 2015, Sean Chester
                          (schester@cs.au.dk)
                          All rights reserved. 

This is the AlphaProximity suite, version 1.0, with software for 
transforming a vertex-labelled graph into a supergraph that is 
resistance to neighbourhood attribute disclosure (NAD) attacks. 
For more details about attribute disclosure attacks, you are
encouraged to read the short article:

 S. Chester and G. Srivastava. 2011. "Social network privacy for
   attribute disclosure attacks." In Proceedings of the 2011
   International Conference on Advances in Social Networks Analysis
   and Mining (ASONAM), pp. 445-449. doi: 10.1109/ASONAM.2011.105


Licensing
---------

You are free to use and redistribute this software as you see fit. 
Consult the attached LICENSE for more details.



Documentation
-------------

The code has been documented for doxygen. If the doc/html/ 
directory is empty or stale, you can regenerate the documentation 
by running the doxygen command without arguments from the 
doc/ directory of this package. The doxygen settings are included 
here in doc/Doxyfile.


Installation
------------

To generate an executable, simply type "make all" from the root directory
of this package (the same directory in which you found this README file).
The makefile will generate the executable bin/alpha_proximity. If you 
encounter difficulties, try first typing "make deepclean" and ensure that
the bin/ directory exists. You can run the executable from a console with 
no command line arguments to get usage instructions. 


Contact
-------

This software suite may be updated and so you are encouraged to contact 
the author (schester@cs.au.dk) to enquire as to whether this is the 
latest version. Also, please do not hesitate to contact the author if 
you have comments, questions, or bugs to report. 

