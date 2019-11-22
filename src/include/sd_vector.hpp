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
void constructSDVector(std::vector<T> &sortedItems, sdsl::sd_vector<> &output, sdsl::sd_vector<>::rank_1_type *ranker, sdsl::sd_vector<>::select_1_type *selecter)
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

class SDVectorSeq
{
public:
    class iterator
    {
    private:
        uint64_t index = 0;
        const sdsl::sd_vector<>::select_1_type *selecter;

    public:

        using difference_type = uint64_t;
        using value_type =uint64_t;
        using pointer = uint64_t*;
        using reference = uint64_t&;
        using iterator_category = std::random_access_iterator_tag;
        //using value_type = uint64_t;

        iterator(const sdsl::sd_vector<>::select_1_type *_selecter, uint64_t _index) : index(_index), selecter(_selecter)
        {
        }

        iterator &operator++()
        {
            this->index++;
            return *this;
        }
        uint64_t operator*()
        {
            return (*selecter)(this->index + 1);
        }
        bool operator!=(const iterator &rhs)
        {
            return (index != rhs.index);
        }
        bool operator==(const iterator &rhs)
        {
            return (index == rhs.index);
        }

        iterator &operator+=(int64_t p)
        {
            this->index += p;
            return *this;
        }


        bool operator<(const iterator &rhs)
        {
            return (index < rhs.index);
        }
        bool operator>(const iterator &rhs)
        {
            return (index > rhs.index);
        }
        bool operator<=(const iterator &rhs)
        {
            return (index <= rhs.index);
        }
        bool operator>=(const iterator &rhs)
        {
            return (index >= rhs.index);
        }
        uint64_t operator-(const iterator &rhs)
        {
            if (this->index < rhs.index)
            {
                return rhs.index - this->index;
            }
            else
            {
                return this->index - rhs.index;
            }
        }
    };

private:
    sdsl::sd_vector<> item;
    sdsl::sd_vector<>::select_1_type selecter;
    uint64_t _size = 0;

public:
    using value_type = uint64_t;
    SDVectorSeq()
    {
    }

    SDVectorSeq(SDVectorSeq &&obj)
    {

        this->item.swap(obj.item);
        sdsl::sd_vector<>::select_1_type _selecter(&item);
        selecter.set_vector(&item);
        selecter.swap(_selecter);
        this->_size = obj._size;

    }
    void construct(std::vector<uint64_t> &sortedItems)
    {
        _size = sortedItems.size();
        constructSDVector(sortedItems, item, NULL, &selecter);
    }

    uint64_t operator[](uint64_t n) const
    {
        uint64_t p = selecter(n + 1);
        return p;
    }
    uint64_t size() const
    {
        return _size;
    }

    iterator begin() const
    {
        auto p = iterator(&this->selecter, 0);
        return p;
    }
    iterator end() const
    {
        auto p = iterator(&this->selecter, this->size());
        return p;
    }
};

} // namespace rlbwt
} // namespace stool