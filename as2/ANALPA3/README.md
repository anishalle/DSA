All you have to do is run 

	clang++ --std=c++23 -O3 MapAVL.cpp -o avl && ./avl


or gcc or whatever your system is.

The requirements in the document say you must have the main file as separate. but no HPP file is 
described in the spec. An hpp file is required for the main.cpp to compile.

I combined everything into one file to avoid linker errors and hopefully make it easier on the grader.
Unfortunately, I didn't have enough time to ask Professor Nemec about what to do here.

You'll find that the main functions are exactly the same between the two assignments
