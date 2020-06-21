/*
*
* AUTHOR: Milan De Cauwer
* 
*/

#include <boost/math/distributions/weibull.hpp>
#include <boost/program_options.hpp> // Reading from the CLI
#include <boost/random.hpp> // Aiming for the weibull dist in particular
#include <boost/random/random_device.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/math/special_functions/round.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <functional>
#include <sstream>
#include <time.h>
#include <list>
#include <unordered_map>
#include <any>
#include <vector>

// Defining Strings for the id naming convention
#define ID_PRB "1BPP"
#define ID_SEP "_"
#define ID_EXT ".dat"

#define MSG_ALPHA " * Warning : The probability of having an item of size 0 is above "
#define MSG_ZERO  " * A 0 value was produced !"
#define MSG_HZERO " 0 values were produced !"

// Defining programm constants
#define ALPHA 0.0001
