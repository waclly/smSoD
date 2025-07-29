#pragma once

#include <vector>
#include <string>

using namespace std;

class Parameter 
{
public:
    static long seed;                                  
    
    // ecga parameter
    static int pop_size;
    static int tour_size;
    static double cross_prob;
    static int max_gen;
    static int len_chromo;

    // msod parameter
    static vector<vector<int>> dim_subproblems;

    // discretization parameter
    static double split_rate;
    static double decrease_rate;
    static double min_bound;
    static double lower;
    static double upper;

    // problem parameter
    static int nfe;
    static int k;
    static int m;
    static int funNum;

    // result output parameter
    static string output_file;

    
    // update static members varaible accroding to arguments
    static void parseArguments(int argc, char* argv[]);    
    static void update(); 
};
