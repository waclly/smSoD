// region.h
#pragma once

#include <vector>
#include <cstdint>
#include "codetable.h"
#include "point.h"
#include "parameter.h"
#include "bound.h"

class Region {
public:
    std::vector<Point*> points;
    std::size_t dimensions;
    std::vector<Bound> bounds;
    
    static std::uint32_t codeIncrement;
    static std::size_t maxPoints;

    Region(std::size_t dims);

    void insertPoints(std::vector<Point>& newPoints);
    void split(CodeTable& CT);
    Point getRandomPoint() const;
    std::vector<Region> splitRegion(const Point& splitPoint) const;
    Point findBestSplitPoint() const;
};