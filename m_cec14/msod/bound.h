#pragma once

#include "parameter.h"
#include <iostream>

class Bound {
public:
    double lower;  
    double upper;  

    Bound(double lowerBound = Parameter::lower, double upperBound = Parameter::upper)
        : lower(lowerBound), upper(upperBound)
    {}

    friend std::ostream& operator<<(std::ostream& os, const Bound& bound) {
        os << "{lower: " << bound.lower << ", upper: " << bound.upper << "}";
        return os;
    }
};
