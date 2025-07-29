#pragma once
#include "parameter.h"
#include <vector>

void cec14_test_func(double *, double *,int,int,int);


double objFun(std::vector<double>& genes) {
    const int M = Parameter::m;
    const int K = Parameter::k;
    std::vector<double> tmps(M);
    
    cec14_test_func(genes.data(), tmps.data(), K, M, Parameter::funNum);
    
    double fitness = 0;
    for (const auto& tmp : tmps){
        fitness += tmp;
    }
    
    return fitness;
}
