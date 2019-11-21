#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sdsl/bit_vectors.hpp>

namespace stool
{
namespace rlbwt
{

template <typename T>
void constructSDVector(std::vector<T> &sortedItems,sdsl::sd_vector<> &output, sdsl::sd_vector<>::rank_1_type *ranker, sdsl::sd_vector<>::select_1_type *selecter)
{
    uint64_t max = 0;
    if (sortedItems.size() > 0)
        max = sortedItems[sortedItems.size() - 1] + 1;

    sdsl::sd_vector_builder builder(max, sortedItems.size());

    for (auto p : sortedItems)
    {
        builder.set(p);
    }
    sdsl::sd_vector<> sd(builder);
    output.swap(sd);

    if (ranker != NULL)
    {
        sdsl::sd_vector<>::rank_1_type _ranker(&output);
        ranker->set_vector(&output);
        ranker->swap(_ranker);
    }
    if (selecter != NULL)
    {
        sdsl::sd_vector<>::select_1_type _selecter(&output);
        selecter->set_vector(&output);
        selecter->swap(_selecter);
    }
}

} // namespace rlbwt
} // namespace stool