CIS3110 Assignment 2
date: Sunday, March 13th, 2016
course: CIS*3110 Operating Systems I
__________________________________________________________________

For documentation on any of the function's functionality,
view function headers in *.h or *.c.
__________________________________________________________________

**Assumptions, Limitations, and Testing**
__________________________________________________________________
Assumes input file is redirected to stdin.
Assumes input file contains only integers, spaces or tabs
Assumes input file is in proper format
    If any of the above assumptions are violated, program
    receives a short message to stderr before terminating

Assumes command line arguments (if any exist) are among:
    -v, -d, (-r x) in any order. -r must be followed directly
    by some integer value (x). Any unrecognized flags will cause
    the program to terminate with a message on stderr.
 
Assumes system has sufficient memory in order to run the program.
    Memory allocations are checked with assert. Program will
    terminate if an assertion fails and NDEBUG isn't defined.
