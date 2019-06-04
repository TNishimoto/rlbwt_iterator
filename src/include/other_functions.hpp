#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>

namespace stool
{
namespace rlbwt
{
/*
template <typename T>
std::vector<T> permutate(std::vector<T> &&items, std::vector<uint64_t> &order)
{
    std::vector<T> tmp;
    tmp.resize(items.size(), 0);

    for (uint64_t i = 0; i < items.size(); i++)
    {
        tmp[order[i]] = items[i];
    }

    items.resize(0);
    items.shrink_to_fit();
    return tmp;
    //std::swap(items, tmp);
}
*/
template <typename T>
std::vector<uint64_t> permutate(std::vector<T> &&items, std::vector<uint64_t> &order)
{
    std::vector<bool> checker;
    uint64_t size = items.size();
    checker.resize(size, false);

    uint64_t current_i = std::numeric_limits<uint64_t>::max();
    T current_value;
    uint64_t i = 0;
    while (i < size)
    {
        if (current_i == std::numeric_limits<uint64_t>::max())
        {
            if (checker[i])
            {
                ++i;
            }else{
                current_i = i;
                current_value = items[i];
            }
        }
        else
        {
            uint64_t tmp_i = order[current_i];
            T tmp_value = items[order[current_i]]; 
            items[order[current_i]] = current_value;
            checker[current_i] = true;
            if(checker[tmp_i]){
                current_i = std::numeric_limits<uint64_t>::max();
            }else{
                current_i = tmp_i;
                current_value = tmp_value;
            }
        }
    }

    return std::move(items);
}

template <typename T>
std::vector<T> rotate(std::vector<T> &&items)
{
    uint64_t size = items.size();
    T fst = items[0];
    for (uint64_t i = 0; i < size; i++)
    {
        items[i] = i + 1 == size ? fst : items[i + 1];
    }
    return std::move(items);
}

template <typename T>
std::vector<uint64_t> get_sorted_positions(std::vector<T> &items)
{
    std::vector<uint64_t> r;
    uint64_t size = items.size();
    r.resize(size);

    for (uint64_t i = 0; i < size; i++)
    {
        r[i] = i;
    }
    sort(r.begin(), r.end(),
         [&](const uint64_t &x, const uint64_t &y) {
             return items[x] < items[y];
         });

    return r;
}
/*
std::vector<uint64_t> change_inv(std::vector<uint64_t> &&items)
{
    std::vector<uint64_t> tmp;
    uint64_t size = items.size();
    tmp.resize(size);

    for (uint64_t i = 0; i < size; i++)
    {
        tmp[items[i]] = i;
    }
    items.resize(0);
    items.shrink_to_fit();

    return tmp;
}
*/
std::vector<uint64_t> change_inv(std::vector<uint64_t> &&items)
{
    std::vector<bool> checker;
    uint64_t size = items.size();
    checker.resize(size, false);

    uint64_t current_i = std::numeric_limits<uint64_t>::max();
    uint64_t current_value = std::numeric_limits<uint64_t>::max();
    uint64_t i = 0;
    while (i < size)
    {
        if (current_i == std::numeric_limits<uint64_t>::max())
        {
            if (checker[i])
            {
                ++i;
            }else{
                current_i = i;
                current_value = items[i];
            }
        }
        else
        {
            uint64_t tmp_i = current_value;
            uint64_t tmp_value = items[current_value]; 
            items[current_value] = current_i;
            checker[current_i] = true;
            if(checker[tmp_i]){
                current_i = std::numeric_limits<uint64_t>::max();
            }else{
                current_i = tmp_i;
                current_value = tmp_value;
            }
        }
    }

    return std::move(items);
}

uint64_t total_cache_miss_counter1 = 0;
uint64_t total_cache_miss_counter2 = 0;
template <typename VecType>
class SortedVec
{
  public:
    static int64_t pred_by_linear_search(const VecType &items, uint64_t value, uint64_t start_pos)
    {
        for (uint64_t i = start_pos; i < items.size(); i++)
        {
            if (items[i] > value)
            {
                return i - 1;
            }
#ifdef DEBUG
            ++total_cache_miss_counter1;
#endif
        }
        return -1;
    }
    static int64_t succ_by_back_linear_search(const VecType &items, uint64_t value, uint64_t end_pos)
    {
        for (int64_t i = end_pos; i >= 0; --i)
        {
            if (items[i] < value)
            {
                return i + 1;
            }
#ifdef DEBUG
            ++total_cache_miss_counter2;
#endif
        }
        return 0;
    }

    static int64_t pred(VecType &items, uint64_t value)
    {
        auto p = std::lower_bound(items.begin(), items.end(), value);
        uint64_t pos = distance(items.begin(), p);
        if (pos == 0)
        {
            return -1;
        }
        else
        {
            return pos - 1;
        }
    }
    static int64_t pred(VecType &items, uint64_t value, uint64_t startIndex, uint64_t endIndex)
    {
        int64_t _startIndex = startIndex;
        int64_t _endIndex = endIndex;
        while (_endIndex - _startIndex > 3)
        {
            //Counter++;
            int64_t _center = (_startIndex + _endIndex) / 2;
            if (value > items[_center])
            {
                _startIndex = _center;
            }
            else
            {
                _endIndex = _center;
            }
        }
        int64_t ret = _startIndex;
        for (int64_t i = _startIndex; i <= _endIndex; i++)
        {
            //Counter++;

            if (items[i] >= value)
            {
                ret = i - 1;
                break;
            }
        }

        /*
        if(pred(items, value) != ret){
            std::cout << "error!" << pred(items, value) << "/" << ret << "/" << _startIndex << "/" << items[startIndex] << "/" << value << std::endl;
            std::cout <<  items[startIndex-1]  << std::endl;
            throw -1;
        }
        */

        return ret;
    }
};
/*
int64_t pred(sd_std::vector<>::rank_1_type &ranker, uint64_t value)
{
    //auto start = std::chrono::system_clock::now();
    uint64_t rank = ranker(value);
    //auto end = std::chrono::system_clock::now();
    //double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    //Counter++;
    //SumTimer+=elapsed;
    
    if (rank == 0)
    {
        return -1;
    }
    else
    {
        return rank - 1;
    }
}
void constructSDVector(std::vector<uint64_t> &sortedItems, sd_std::vector<> &output, sd_std::vector<>::rank_1_type *ranker, sd_std::vector<>::select_1_type *selecter)
{
    uint64_t max = 0;
    if (sortedItems.size() > 0)
        max = sortedItems[sortedItems.size() - 1] + 1;

    sd_vector_builder builder(max, sortedItems.size());

    for (auto p : sortedItems)
    {
        builder.set(p);
    }
    sd_std::vector<> sd(builder);
    output.swap(sd);

    if (ranker != NULL)
    {
        sd_std::vector<>::rank_1_type _ranker(&output);
        ranker->set_vector(&output);
        ranker->swap(_ranker);
    }
    if (selecter != NULL)
    {
        sd_std::vector<>::select_1_type _selecter(&output);
        selecter->set_vector(&output);
        selecter->swap(_selecter);
    }
}
*/
} // namespace rlbwt
} // namespace stool