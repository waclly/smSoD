// ecga.h
#pragma once

#include "chromosome.h"
#include "point.h"
#include "mpm.h"
#include "codetable.h"
#include "parameter.h"
#include <vector>
#include <cstddef>
#include <cstdint>
#include <mutex>

class Ecga {
public:
    std::vector<Chromosome> population;
    std::vector<Chromosome> matingPool;

    double previousStd{};
    double previousMean{};

    Ecga() noexcept;

    void adjustMinBoundFromTopQuartile() noexcept;
    void run() noexcept;
    void evaluate() noexcept;
    std::size_t tournamentWinner(const std::vector<std::size_t>& order,
                                  std::size_t start,
                                  std::size_t tournSize) const noexcept;
    void tournamentSelection() noexcept;

    void preEncode(std::vector<std::vector<Point>>& out) noexcept;
    void encode(std::vector<std::vector<Point>>& points,
                std::vector<CodeTable>& codeTables) noexcept;
    void generatePopulation(const std::vector<Group>& groups,
                            const std::vector<CodeTable>& codeTables,
                            const std::vector<std::vector<Point>>& points) noexcept;

    void writeCSV(const Chromosome& best) const noexcept;

private:
    template <typename T>
    static void transpose(const std::vector<std::vector<T>>& in,
                          std::vector<std::vector<T>>& out) noexcept;

    static std::mutex fileMutex;
};