#pragma once

#include "bound.h"
#include "random.hpp"
#include <vector>

extern randomG RANDOM;

class CodeTable {
public:
    std::vector< std::vector<Bound> > Table;
    
    CodeTable(){
        Table.reserve(100);
    }

    void add(std::vector<Bound>&& bounds) {
        Table.emplace_back(std::move(bounds));
    }

    std::vector<double> get(int index) const {
        const auto& bounds = Table[index];
        std::vector<double> tmp;
        tmp.reserve(bounds.size());
        for (const auto& b : bounds) {
            tmp.emplace_back(RANDOM.uniform(b.lower, b.upper));
        }        
        return tmp;
    }
};
