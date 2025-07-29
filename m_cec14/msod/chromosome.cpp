#include "chromosome.h"
#include "random.hpp"
#include "objfun.h"
#include <iostream>
#include <iomanip>


extern randomG RANDOM;
Chromosome::Chromosome(int len_chromo)
    : genes(len_chromo)
{}


void Chromosome::random() {
    for (auto& gene : genes) {
        gene = RANDOM.uniform(Parameter::lower, Parameter::upper);
    }
}


void Chromosome::evaluate() {   
    Parameter::nfe++;
    this->fitness = objFun(this->genes);    
}

std::ostream& operator<<(std::ostream& os, const Chromosome& chrom) {
    os << std::scientific << std::setprecision(8);    
    os << "Chromosome { fitness: " << chrom.fitness << ", genes: [";

    if (!chrom.genes.empty()) {
        os << chrom.genes[0];
        for (size_t i = 1; i < chrom.genes.size(); ++i) {
            os << ", " << chrom.genes[i];
        }
    }
    os << "] }" << std::endl;
    return os;
}
