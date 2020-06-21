# A weibull-based benchmark generator for bin packing

This is an implementation of a instance generator for the bin packing problem.
Bin packing is a ubiquitous problem that arises in many practical applications. The motivation for the work presented here comes from the domain of data centre optimisation. The parameterisable benchmark generator can be used to study the behavior of various solving methodologies for bin packing under various Weibull distributions.

# Requirements

The generator relies on the implementation of the features provided by the `boost` libraries for c++. Boost can be installed on Debian related systems as follows.

> sudo apt-get install libboost-all-dev

# Compiling the generator

Compilation line:
> g++ -Wall -pedantic weibBench.cpp -o  weibBench -L/usr/local/lib/ -lboost_program_options -lboost_random -std=c++17

Note the `-std=c++17` forcing the compiler to use the 17 implementation of cpp. Essential to use the `any` type for heterogeneous maps of type `map<str, any>` bot present in previous implementations.

# Usage

The instance generator reads arguments from the command line. Here are the options available.

*  *--ninstances*, *-N* Desired number of instances. Defaults to 1.
*  *--nitems*, *-n* Number of items in each instance. Defaults to 100.
*  *--factor*, *-f* Bin factor. Defaults to 2.0.
*  *--shape*, *-a* Weibull's shape parameter. Defaults to 1.0.
*  *--scale*, *-b* Weibull's scale parameter. Defaults to 1000.
*  *--seed*, *-s* Seeds the boost random number generator.
*  *--sort*, *-t* Sorts the items by increasing (inc) or decreasing (dec) size. Defaults to unsorted.


Refer to the [paper][topaper] for a description of these parameters.


# Output

Each sincle instance will be written to a file with the following format:
* First line contains two space separated integers: `nitems binsize`
* The following `nitmes` lines contain an integer representing the size of that item.


[topaper]: https://link.springer.com/chapter/10.1007%2F978-3-642-33558-7_17
