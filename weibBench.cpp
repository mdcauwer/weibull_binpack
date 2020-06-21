/*
*
* AUTHOR: Milan De Cauwer
*
* This is a simple implementation of the weibull based bin packing generator.
* Requires the Boost library 1.51.0 or above.
* Compilation line :
* clear; g++ -Wall -pedantic weibBench.cpp -o weibBench -L/usr/local/lib/ -lboost_program_options -lboost_random 
* 
* TODO: Implementation the xml output format.
*
*/
#include "weibBench.hpp"

using namespace std;
using namespace boost::program_options;
using namespace boost::random;

typedef std::unordered_map<std::string, std::any> t_map;

/**
 * A to string template
 */

template<typename T>
std::string to_string(const T &Value){
    std::ostringstream oss;
    oss << Value;
    return oss.str();
}

/**
 * A generator template
 */

template<class T>
double rItemSize(T &generator)
{
  return generator();
}

/**
 * Finds max value in a list of items
 *
 * @param items List of item sizes (ints)
 * @return Size (int) of largest item.
 */
const uint findMax(const list<uint> items) {
    // TODO: Should probably deal with the empty list case.
    list<uint>::const_iterator cit;
    uint max = *items.begin();
    for(cit=items.begin(); cit!=items.end(); cit++){
        if(max < *cit){
            max = *cit;
        }
    }
    return max;
}

/**
 * Writing instance to disk
 *
 * @param name (str)
 * @param items (int) is list of ints representing item sizes
 * @param params (map<str, double>) 
 * @param binSize (int) the bin size
 */
void writeInstance(const std::string name, const list<uint> items, uint binSize){
    
    try {
        ofstream fd(name.c_str(), ios::out | ios::trunc);
        fd << items.size() << " " << binSize << endl;
        list<uint>::const_iterator cit;
        for(cit=items.begin(); cit!=items.end(); cit++) {
            fd << *cit << endl;
        }
        fd.close();
    }catch (const ofstream::failure& e) {
        cerr << "Could not create " << name << "file" << endl;
  }
}

/**
 * Helper function to build file names
 *
 * @param items List of item sizes (ints)
 * @return ret (str) file name
 */
const std::string buildName(const uint numItem, const uint binSize, const uint seed){
    std::string ret(""), a(ID_PRB), sep(ID_SEP), dat(ID_EXT);
    ret += a; ret += sep;
    ret += to_string(numItem);ret += sep;
    ret += to_string(binSize);ret += sep;
    ret += to_string(seed);  ret += dat;
    return ret;
}

/**
 * Finds max value in a list of items
 *
 * @param shape weibull's shape param
 * @param scale weibull's scale param
 * @return 0 if not above threshold, 1 otherwise
 */

const uint isAboveThreshold(const double shape, const double scale){
    uint ret = 0;
    double p;
    boost::math::weibull_distribution<> wd(shape, scale);
    p = boost::math::cdf(wd, 0.5); 
    if(p > ALPHA){
        ret = 1;
    }
    cout << "P : " << p << endl;
    return ret;
}


/**
 * Generates the list of item sizes
 *
 * @param numItem 
 * @param params parameter map
 * @return 0 if not above threshold, 1 otherwise
 */

const list<uint> genItemSizes(const uint numItem, t_map params){

    const double shape = std::any_cast<double>(params["wShape"]);
    const double scale = std::any_cast<double>(params["wScale"]);
    const uint seed = std::any_cast<uint>(params["rSeed"]);
    
    // A mt19937 random number generator - the best one according to the Boost Documentation
    typedef boost::mt19937 RNGType;
    
    // An instance of a Weibull distribution with the specified shape and scale
    weibull_distribution<> wd(shape, scale);
    
    // A variate generator
    boost::variate_generator< RNGType, boost::random::weibull_distribution<> > gen(boost::mt19937(seed), boost::random::weibull_distribution<>(shape, scale));

    list<uint> ret; uint zeros = 0;
        for(uint i = 0; i < numItem; i++){
        ret.push_back(boost::math::iround(rItemSize(gen)));
        if(ret.back() == 0){
            zeros++;
        }
    }
    if(zeros) cerr << zeros << MSG_HZERO << endl;
    return ret;
}

const uint setSeed(){
    boost::random_device rd;
    uint ret = rd();
    return ret;
}

int main(int ac, char* av[]) {
    
    unsigned int numItem, numInst, rSeed;
    double wShape, wScale, bFactor;
    
    std::string srt; variables_map vm;
    std::string s("none");
    
    try {
        // Parsing params
        options_description desc("Allowed Options");
        desc.add_options()
        ("help", "produce help message")
        ("ninstances,N" , value<unsigned int>(&numInst)->default_value(1), "Number of instances to generate")
        ("nitems,n" , value<unsigned int>(&numItem)->default_value(100)   , "Sets the number of items in each instance")
        ("factor,f", value<double>(&bFactor)->default_value(2.0) , "Sets the bin factor")
        ("shape,a", value<double>(&wShape)->default_value(1.0)  , "Sets Weibull shape")
        ("scale,b", value<double>(&wScale)->default_value(1000) , "Sets Weibull scale")
        ("seed,s" , value<unsigned int>(&rSeed)                 , "Sets a seed for random generator - ")
        ("sort,t" , value<std::string>(&srt)->default_value(s),   "Sort items by [inc|dec] size")      
        ;
    
        // Storing values read on the command line
        store(parse_command_line(ac, av, desc), vm);
        notify(vm);
                
        // If user needs help -> print it and leave
        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }
        
        // We should check if values are in the correct range
        int ret = 0;
        if (vm.count("num-items") && numItem < 1) {
            cerr << "* Number of items to generate must be a positive integer" << endl; ret = 1;
        }
        if (vm.count("bin-factor") && bFactor < 1.0) {
            cerr << "* Bin factor must be bigger than 1.0" << endl; ret = 1;
        }
        if (vm.count("weib-shape") && wShape <= 0.0 ) {
            cerr << "* Shape must be bigger than 0.0" << endl; ret = 1;
        }
        if (vm.count("weib-scale") && wScale <= 0.0) {
            cerr << "* Scale must be bigger than 0.0" << endl; ret = 1;
        }
        if (vm.count("rand-seed") && rSeed < 0) {
            cerr << "* Seed must be bigger than 0" << endl; ret = 1;
        }
        if (vm.count("num-instances") && numInst < 0) {
            cerr << "* Number of generated instances should be at least 1" << endl; ret = 1;
        }
        if (vm.count("num-instances") && numInst > 1 && vm.count("rand-seed")) {
            cerr << "* Cannot specify a unique seed while generating several instances" << endl; ret = 1;
        }
        if (vm.count("sort") && (srt != "dec" && srt != "inc" && srt != "none") ) {
            cerr << "* Wrong sort parameter. Should be [inc|dec]" << endl; ret = 1;
        }
        if (ret) return ret;
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << endl;
        return 1;
    }

    uint binSize, nb;
    std::string fileName;
    
    t_map params;    
         
    params["bFactor"] = bFactor;
    params["wShape"] = wShape;
    params["wScale"] = wScale;
    params["rSeed"] = rSeed;
    
    
    // TODO: Only a warning message for now
    if(isAboveThreshold(wShape, wScale)) cerr << MSG_ALPHA << ALPHA << endl; 
    
    for(nb = 0; nb < numInst; nb++){
        
        if(!vm.count("rand-seed")) rSeed = setSeed();
        
        list<uint> inst = genItemSizes(numItem, params);
        
        // Sorting the instance
        if(srt == "inc"){
            inst.sort();
        }else{
            if (srt == "dec"){
                inst.sort();
                inst.reverse();
            }
        }        
        
        binSize = boost::math::iround(findMax(inst) * bFactor);
        fileName = buildName(numItem, binSize, rSeed);

        cout << "Generating : " << fileName << endl;
        writeInstance(fileName, inst, binSize);
    }
    return 0;
}
