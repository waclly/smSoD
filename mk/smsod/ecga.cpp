// ecga.cpp
#include "ecga.h"
#include "random.hpp"
#include "region.h"
#include "utility.hpp"
#include <algorithm>
#include <numeric>
#include <fstream>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <cassert>

using std::size_t;

// static member
std::mutex Ecga::fileMutex;

Ecga::Ecga() noexcept
    : population(Parameter::pop_size),
      matingPool(Parameter::pop_size)
{
    for (auto& ind : population) {
        ind.random();
    }
}

void Ecga::adjustMinBoundFromTopQuartile() noexcept {
    const size_t popSz = population.size();
    const size_t k     = popSz / 4;
    std::nth_element(population.begin(), population.begin() + k, population.end(), compareChromosoe);

    double sum = 0.0;
    for (size_t i = 0; i < k; ++i) sum += population[i].fitness;
    const double mu = (k > 0 ? sum / k : 0.0);

    double sq = 0.0;
    for (size_t i = 0; i < k; ++i) {
        const double d = population[i].fitness - mu;
        sq += d * d;
    }
    const double sigma = (k > 0 ? std::sqrt(sq / k) : 0.0);

    if (sigma < Parameter::min_bound) {
        Parameter::min_bound = std::max(Parameter::min_bound / 1.05, 1e-13);
    }
}

void Ecga::run() noexcept {
    for (size_t gen = 1; gen < Parameter::max_gen; ++gen) {
        evaluate();
        adjustMinBoundFromTopQuartile();
        tournamentSelection();

        std::vector<std::vector<Point>> buffer1;
        buffer1.reserve(population.size());
        preEncode(buffer1);

        std::vector<std::vector<Point>> buffer2;
        transpose(buffer1, buffer2);

        std::vector<CodeTable> codeTables(Parameter::dim_subproblems.size());
        encode(buffer2, codeTables);

        transpose(buffer2, buffer1);
        auto groups = buildMPM(buffer1, codeTables);
        generatePopulation(groups, codeTables, buffer1);
    }
    evaluate();
    const auto it = std::min_element(population.begin(), population.end(), compareChromosoe);
    if (it != population.end()) {
        writeCSV(*it);
    }
}

void Ecga::evaluate() noexcept {
    for (auto& ind : population) {
        ind.evaluate();
    }
}

size_t Ecga::tournamentWinner(const std::vector<size_t>& order,
                              size_t start,
                              size_t tournSize) const noexcept
{
    size_t winner = order[start];
    const size_t end = std::min(start + tournSize, order.size());
    for (size_t i = start + 1; i < end; ++i) {
        if (population[order[i]].fitness < population[winner].fitness) {
            winner = order[i];
        }
    }
    return winner;
}

void Ecga::tournamentSelection() noexcept {
    const size_t N = population.size();
    const size_t s = Parameter::tour_size;
    thread_local static std::vector<size_t> order;
    order.resize(N);
    std::iota(order.begin(), order.end(), 0);
    std::shuffle(order.begin(), order.end(), RANDOM.get_engine());

    size_t pick = 0;
    for (size_t i = 0; i < N; ++i) {
        if (pick + s > N) {
            std::shuffle(order.begin(), order.end(), RANDOM.get_engine());
            pick = 0;
        }
        size_t win = tournamentWinner(order, pick, s);
        matingPool[i] = population[win];
        pick += s;
    }
}

void Ecga::preEncode(std::vector<std::vector<Point>>& out) noexcept {
    const size_t N = population.size();
    const size_t D = Parameter::dim_subproblems.size();
    out.assign(N, std::vector<Point>(D));

    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < D; ++j) {
            Point& p = out[i][j];
            const auto& sub = Parameter::dim_subproblems[j];
            p.components.resize(sub.size());
            for (size_t k = 0; k < sub.size(); ++k) {
                p.components[k] = matingPool[i].genes[sub[k]];
            }
        }
    }
}

template <typename T>
void Ecga::transpose(const std::vector<std::vector<T>>& in,
                     std::vector<std::vector<T>>& out) noexcept
{
    const size_t R = in.size();
    if (R == 0) { out.clear(); return; }
    const size_t C = in[0].size();
    out.assign(C, std::vector<T>(R));
    for (size_t c = 0; c < C; ++c) {
        for (size_t r = 0; r < R; ++r) {
            out[c][r] = in[r][c];
        }
    }
}

void Ecga::encode(std::vector<std::vector<Point>>& points,
                  std::vector<CodeTable>& codeTables) noexcept
{
    Region::maxPoints = static_cast<size_t>(Parameter::pop_size * Parameter::split_rate);
    for (size_t i = 0; i < points.size(); ++i) {
        Region::codeIncrement = 0;
        Region subRegion(points[i].front().components.size());
        subRegion.insertPoints(points[i]);
        subRegion.split(codeTables[i]);
    }
    Parameter::split_rate *= Parameter::decrease_rate;
}

void Ecga::generatePopulation(const std::vector<Group>& groups,
                              const std::vector<CodeTable>& codeTables,
                              const std::vector<std::vector<Point>>& points) noexcept
{
    const size_t popSize = population.size();
    const size_t numCross = static_cast<size_t>(popSize * Parameter::cross_prob);
    const size_t remain   = popSize - numCross;

    const auto& dimSub = Parameter::dim_subproblems;
    auto& popData       = population;
    const auto& pool    = matingPool;

    for (size_t k = 0; k < numCross; ++k) {
        auto& child = popData[k];
        for (const auto& g : groups) {
            for (auto idx : g.indices) {
                const size_t parent = RANDOM.uniform(0, static_cast<int>(popSize - 1));
                const auto& pt     = points[parent][idx];
                const auto& vals   = codeTables[idx].get(pt.code);
                const auto& sub    = dimSub[idx];
                for (size_t t = 0; t < sub.size(); ++t) {
                    child.genes[sub[t]] = vals[t];
                }
            }
        }
    }

    auto poolCopy = pool;
    std::nth_element(poolCopy.begin(), poolCopy.begin() + remain, poolCopy.end(), compareChromosoe);
    for (size_t i = 0; i < remain; ++i) {
        popData[numCross + i] = poolCopy[i];
    }
}

void Ecga::writeCSV(const Chromosome& best) const noexcept {
    static const std::vector<std::string> header = {
        "m","k","length","mode","eva_fun","seed","error"
    };
    std::lock_guard<std::mutex> guard(fileMutex);
    std::ifstream infile(Parameter::output_file);
    const bool isEmpty = infile.peek() == std::ifstream::traits_type::eof();
    infile.close();

    std::ofstream outfile(Parameter::output_file, std::ios::app);
    if (!outfile.is_open()) return;

    if (isEmpty) {
        for (size_t i = 0; i < header.size(); ++i) {
            outfile << header[i] << (i + 1 < header.size() ? ',' : '\n');
        }
    }
    outfile << Parameter::m << ','
            << Parameter::k << ','
            << Parameter::len_chromo << ','
            << "smsod" << ','
            << Parameter::nfe << ','
            << Parameter::seed << ','
            << std::scientific << std::setprecision(16)
            << (best.fitness - (0))
            << '\n';
}