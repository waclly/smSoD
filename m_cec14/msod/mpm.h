#pragma once

#include "parameter.h"
#include "point.h"
#include "codetable.h"

#include <vector>
#include <ostream>
#include <cstdint>

class Group {
public:
    std::uint64_t num_counts{};
    std::vector<std::size_t> indices{};

    double modelComplexity{};
    double populationComplexity{};

    Group();
    std::size_t size() const noexcept;
    double getModelComplexity() const noexcept;
    double getPopulationComplexity() const noexcept;
    Group merge(const Group&) const;
    friend std::ostream& operator<<(std::ostream&, const Group&);
};

void count(Group&, const std::vector<std::vector<Point>>&);

std::vector<Group> buildMPM(
    const std::vector<std::vector<Point>>&,
    const std::vector<CodeTable>&);

