#pragma once

#include "parameter.h"
#include <vector>

#define MAX_FITNESS 1e9

using namespace std;

class Chromosome 
{
public:
    double fitness = MAX_FITNESS;
    vector<double> genes;

    Chromosome(int len_chromo = Parameter::len_chromo); // ctor for chromosome
    void random();  // for init the genes
    void evaluate(); // evaluate the goodness of chromosome

    friend ostream& operator<<(ostream& os, const Chromosome& chrom);
    friend bool compareChromosoe(const Chromosome & first, const Chromosome & second); 
};

inline bool compareChromosoe(const Chromosome & first, const Chromosome & second) 
{
    return first.fitness < second.fitness;
}

