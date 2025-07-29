// region.cpp
#include "region.h"
#include "parameter.h"
#include "random.hpp"

#include <cassert>
#include <stack>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

extern randomG RANDOM;

using MaskType = __uint128_t;

// Custom hash for 128-bit mask: xor upper and lower 64 bits
struct MaskHash {
    std::size_t operator()(MaskType m) const noexcept {
        std::uint64_t lo = (std::uint64_t)m;
        std::uint64_t hi = (std::uint64_t)(m >> 64);
        return static_cast<std::size_t>(lo ^ hi);
    }
};

// Static definitions
std::uint32_t Region::codeIncrement = 0;
std::size_t Region::maxPoints = 0;

Region::Region(std::size_t dims)
    : dimensions(dims), bounds(dims)
{}

void Region::insertPoints(std::vector<Point>& newPoints) {
    points.reserve(static_cast<std::size_t>(Parameter::pop_size));
    for (auto& p : newPoints) {
        points.push_back(&p);
    }
}

void Region::split(CodeTable& CT) {
    std::stack<Region> stk;
    stk.push(*this);

    while (!stk.empty()) {
        Region curr = std::move(stk.top());
        stk.pop();

        // 1. Choose split point
        Point bestPt = curr.getRandomPoint();

        std::unordered_map<MaskType, std::vector<Point*>, MaskHash> buckets;
        buckets.reserve(curr.points.size());

        // 2. Assign points to buckets by integer mask
        for (Point* p : curr.points) {
            MaskType mask = 0;
            for (std::size_t d = 0; d < curr.dimensions; ++d) {
                mask |= (MaskType)(p->components[d] > bestPt.components[d]) << d;
            }
            buckets[mask].push_back(p);
        }

        // 3. Create children or finalize
        for (auto& kv : buckets) {
            MaskType mask = kv.first;
            auto& pts = kv.second;

            Region child(curr.dimensions);
            for (std::size_t d = 0; d < curr.dimensions; ++d) {
                bool bit = (mask >> d) & 1;
                if (bit) {
                    child.bounds[d].lower = bestPt.components[d];
                    child.bounds[d].upper = curr.bounds[d].upper;
                } else {
                    child.bounds[d].lower = curr.bounds[d].lower;
                    child.bounds[d].upper = bestPt.components[d];
                }
            }
            child.points = std::move(pts);

            bool canSplit = child.points.size() >= maxPoints
                          && std::all_of(child.bounds.begin(), child.bounds.end(),
                                         [](Bound const& b){ return (b.upper - b.lower) >= Parameter::min_bound; });

            if (canSplit) {
                stk.push(std::move(child));
            } else {
                for (auto* p : child.points) {
                    p->setCode(codeIncrement);
                }
                CT.add(std::move(child.bounds));
                ++codeIncrement;
            }
        }
    }
}

Point Region::findBestSplitPoint() const {
    Point bestPt;
    bestPt.components.resize(dimensions);
    std::vector<double> tmpComps(dimensions);
    int bestMax = -1;

    for (int t = 0; t < 5; ++t) {
        for (std::size_t d = 0; d < dimensions; ++d) {
            tmpComps[d] = RANDOM.uniform(bounds[d].lower, bounds[d].upper);
        }
        std::unordered_map<MaskType, int, MaskHash> counts;
        counts.reserve(points.size());

        int curMax = 0;
        for (Point* p : points) {
            MaskType mask = 0;
            for (std::size_t d = 0; d < dimensions; ++d) {
                mask |= (MaskType)(p->components[d] > tmpComps[d]) << d;
            }
            int c = ++counts[mask];
            if (c > curMax) curMax = c;
        }
        if (curMax > bestMax) {
            bestMax = curMax;
            bestPt.components.swap(tmpComps);
        }
    }
    return bestPt;
}

Point Region::getRandomPoint() const {
    std::vector<double> comps(dimensions);
    for (std::size_t d = 0; d < dimensions; ++d) {
        comps[d] = RANDOM.uniform(bounds[d].lower, bounds[d].upper);
    }
    return Point(std::move(comps));
}
