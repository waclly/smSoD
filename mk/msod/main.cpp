#include "parameter.h"
#include "ecga.h"
#include "random.hpp"      // random number generator

#include <random>

using namespace std;
randomG RANDOM;

double *OShift,*M,*y,*z,*x_bound;
int ini_flag=0,n_flag,func_flag,*SS;

int main(int argc, char* argv[]) 
{    
    Parameter::parseArguments(argc, argv);
    Parameter::update();

    random_device rd;  
    Parameter::seed = static_cast<long>(rd());  
    RANDOM.randomize(Parameter::seed);
    
    Ecga ecga;
    ecga.run();
    return 0;
}
