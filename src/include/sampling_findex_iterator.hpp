#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "rlbwt.hpp"

namespace stool
{
namespace rlbwt
{


template <typename CHAR, typename INDEX, typename RLBWT_STR>
class SamplingFIndexIterator
{
  private:
    INDEX _rle_findex = 0;
    const RLBWT<CHAR> &_rlbwt;
    std::vector<INDEX> &fl_mapper;
    INDEX _findex = 0;

  public:
    SamplingFIndexIterator() = default;

    SamplingFIndexIterator(INDEX __rle_findex, const RLBWT_STR &__rlbwt, std::vector<INDEX> &__fl_mapper) : 
    _rle_findex(__rle_findex), _rlbwt(__rlbwt), fl_mapper(__fl_mapper) 
    {
    }

    INDEX findex() const
    {
        return this->_findex;
    }
    CHAR character() const
    {
        return this->_rlbwt.get_char_by_run_index(this->fl_mapper[this->_rle_findex]) ;
    }
    
    INDEX rle_lindex() const
    {
        return this->fl_mapper[this->_rle_findex];
    }
    
    INDEX rle_findex() const
    {
        return this->_rle_findex;
    }

    SamplingFIndexIterator &operator++()
    {
        if(this->_rle_findex > _rlbwt.rle_size()-1 ){            
            throw std::logic_error("error" + std::to_string(this->_rle_findex) + "/"  + std::to_string(_rlbwt.rle_size()));
        }


        this->_findex += this->_rlbwt.get_run(this->fl_mapper[this->_rle_findex]);
        this->_rle_findex++;

        if (this->_rle_findex > _rlbwt.rle_size()-1)
        {
            this->_rle_findex = std::numeric_limits<INDEX>::max();
            this->_findex = std::numeric_limits<INDEX>::max();
        }

        return *this;
    }
    INDEX operator*() const
    {
        return this->_findex;
    }
    bool operator!=(const SamplingFIndexIterator &rhs) const
    {
        return (_rle_findex != rhs._rle_findex);
    }
};

template <typename CHAR, typename INDEX, typename RLBWT_STR>
class SamplingFIndexGenerator
{
  private:
    const RLBWT_STR &_rlbwt;
    std::vector<INDEX> _fl_mapper;

  public:
    SamplingFIndexGenerator(const RLBWT_STR &__rlbwt): _rlbwt(__rlbwt)
    {
        this->_fl_mapper = RLBWTFunctions::construct_rle_fl_mapper<INDEX>(_rlbwt);
    }
    SamplingFIndexIterator<CHAR,INDEX, RLBWT_STR> begin()
    {
        return SamplingFIndexIterator<CHAR,INDEX, RLBWT_STR>(0, _rlbwt, _fl_mapper);
    }
    SamplingFIndexIterator<CHAR,INDEX, RLBWT_STR> end()
    {
        return SamplingFIndexIterator<CHAR,INDEX, RLBWT_STR>(std::numeric_limits<INDEX>::max(), _rlbwt, _fl_mapper);
    }
};


class RLBWTFunctions2{
    public:
    template <typename CHAR = char, typename INDEX = uint64_t, typename RLBWT_STR>
    static std::vector<uint64_t> construct_rle_lf_lorder(const RLBWT_STR &__rlbwt){
        SamplingFIndexGenerator<CHAR, INDEX, RLBWT_STR> generator(__rlbwt);
        std::vector<uint64_t> rle_lf_lorder;
        rle_lf_lorder.resize(__rlbwt.rle_size(), std::numeric_limits<uint64_t>::max());
        INDEX k=0;
        for(SamplingFIndexIterator<CHAR, INDEX, RLBWT_STR> it = generator.begin(), end = generator.end(); it != end;++it){
            INDEX findex = *it;
            while(findex >= __rlbwt.get_lpos(k) ){
                k++;
            }
            rle_lf_lorder[it.rle_lindex()] = k-1;
        }
        return rle_lf_lorder;
    }

};

} // namespace rlbwt
} // namespace stool