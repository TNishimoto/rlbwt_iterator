#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "../rlbwt.hpp"
//#include "forward_sa.hpp"
#include "../rle_farray.hpp"
#include "../rlbwt_functions.hpp"

namespace stool
{
namespace rlbwt
{

template <typename RLBWT_STR, typename FINDEXES_VEC>
class MultipleTextPositionIterator
{
  private:
    using CHAR = typename RLBWT_STR::char_type;
    uint64_t _index = 0;
    uint64_t _distance = 0;

    const RLBWT_STR &_rlbwt;
    FINDEXES_VEC &_findexes_lorder;
    std::vector<uint64_t> _findexes;
    //std::vector<uint64_t> _xvec;

  public:
    MultipleTextPositionIterator() = default;
    MultipleTextPositionIterator(std::vector<uint64_t> &indexes, const RLBWT_STR &__rlbwt, FINDEXES_VEC &__findexes_lorder) : _rlbwt(__rlbwt), _findexes_lorder(__findexes_lorder)
    {
        for (auto p : indexes)
        {
            this->_findexes.push_back(p);
        }
    }
    
    void process()
    {
        for (uint64_t i = 0; i < this->_findexes.size(); i++)
        {
            uint64_t value = this->succ(i);

            this->_findexes[i] = value;
        }
        uint64_t tmp_i = 0;
        for (uint64_t i = 0; i < this->_findexes.size(); i++)
        {
            if (this->_findexes[i] != std::numeric_limits<uint64_t>::max())
            {
                this->_findexes[tmp_i] = this->_findexes[i];
                tmp_i++;
            }
        }
        this->_findexes.resize(tmp_i);
        this->_findexes.shrink_to_fit();
        this->_distance++;
        this->_index = 0;
    }
    
    void add(uint64_t pos)
    {
        this->_findexes.push_back(pos);
    }

    std::pair<uint64_t, uint64_t> operator*()
    {
        std::pair<uint64_t, uint64_t> lindex_pair = _rlbwt.to_rle_lindex(_findexes[this->_index]);
        return lindex_pair;
        //return _findexes[this->_index];
    }
    MultipleTextPositionIterator &operator++()
    {
        this->_index++;
        if (this->_index == this->_findexes.size())
        {
            this->_index = std::numeric_limits<uint64_t>::max();
        }
        return *this;
    }
    uint64_t distance(){
        return this->_distance;
    }
    bool isEnd()
    {
        return this->_index == std::numeric_limits<uint64_t>::max();
    }
    uint64_t size(){
        return this->_findexes.size();
    }

  private:
    uint64_t succ(uint64_t i)
    {
        std::pair<uint64_t, uint64_t> lindex_pair = _rlbwt.to_rle_lindex(_findexes[i]);
        if (lindex_pair.second == 0)
        {
            return std::numeric_limits<uint64_t>::max();
        }
        else
        {
            uint64_t new_findex = _findexes_lorder[lindex_pair.first] + lindex_pair.second;
            return new_findex;
        }
    }
};
} // namespace rlbwt
} // namespace stool