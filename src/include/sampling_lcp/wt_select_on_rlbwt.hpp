#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "stool/include/debug.hpp"
#include <sdsl/wavelet_trees.hpp>
#include "stool/include/elias_fano_vector.hpp"

namespace stool
{
namespace rlbwt
{

template <typename RLE_SIZE_TYPE>
class SelectOnRLBWT
{
    //const RLBWT_STR *_rlbwt;
    std::vector<RLE_SIZE_TYPE> next_c_index_vector;
    std::vector<bool> first_c_flag_vec;

public:
    template <typename RLBWT_STR>
    void build(RLBWT_STR &_rlbwt)
    {
        RLEFArray<RLBWT_STR> generator(_rlbwt);

        //std::vector<uint64_t> previous_c_index_vector;
        this->next_c_index_vector.resize(_rlbwt.rle_size(), std::numeric_limits<RLE_SIZE_TYPE>::max());
        this->first_c_flag_vec.resize(_rlbwt.rle_size(), false);

        RLE_SIZE_TYPE prev_c = std::numeric_limits<RLE_SIZE_TYPE>::max();
        RLE_SIZE_TYPE prev_rle_lindex_on_F = std::numeric_limits<RLE_SIZE_TYPE>::max();

        for (typename RLEFArray<RLBWT_STR>::iterator it = generator.begin(); it != generator.end(); ++it)
        {

            uint64_t rle_findex = it.rle_findex();
            RLE_SIZE_TYPE rle_lindex = it.rle_lindex();
            uint64_t c = it.character();

            if (rle_findex == 0 || prev_c != c)
            {
                first_c_flag_vec[rle_lindex] = true;
                //previous_c_index_vector[rle_lindex] = UINT64_MAX;
                //uint64_t findex = it.findex();
            }
            else
            {
                next_c_index_vector[prev_rle_lindex_on_F] = rle_lindex;
                //previous_c_index_vector[rle_lindex] = prev_rle_lindex_on_F;
            }
            prev_c = c;
            prev_rle_lindex_on_F = rle_lindex;
        }
    }

    bool get_first_c_flag(uint64_t x)
    {
        return first_c_flag_vec[x];
    }
    uint64_t size()
    {
        return next_c_index_vector.size();
    }
    uint64_t get_next_c_index(uint64_t x)
    {
        return next_c_index_vector[x];
    }
    uint64_t null_flag()
    {
        return std::numeric_limits<RLE_SIZE_TYPE>::max();
    }
    uint64_t get_using_memory() const {
        return (next_c_index_vector.size() * sizeof(RLE_SIZE_TYPE)) + (first_c_flag_vec.size() / 8);
    }
};

template <typename CHAR, typename WT>
class WTSelectOnRLBWT
{
public:
    //const RLBWT_STR *_rlbwt;
    uint64_t _size;
    WT *wt;
    //std::vector<bool> first_c_flag_vec;

    void set(WT *_wt)
    {
        this->wt = _wt;
        this->_size = this->wt->size();
        /*
        this->first_c_flag_vec.resize(this->_size);
        for(uint64_t i=0;i<_size;i++){
            this->first_c_flag_vec[i] = this->get_first_c_flag(i);
        }
        */
    }
    uint64_t get_next_c_index(uint64_t x)
    {

        CHAR c = (*wt)[x];
        uint64_t rank = wt->rank(x + 1, c);
        uint64_t c_num = wt->rank(_size, c);
        uint64_t next_rank = rank + 1;
        if (next_rank <= c_num)
        {
            return wt->select(next_rank, c);
        }
        else
        {
            return UINT64_MAX;
        }
    }
    bool get_first_c_flag(uint64_t x)
    {
        CHAR c = (*wt)[x];
        uint64_t rank = wt->rank(x + 1, c);
        return rank == 1;
    }
    uint64_t size()
    {
        return this->_size;
    }
    uint64_t null_flag()
    {
        return std::numeric_limits<uint64_t>::max();
    }
    uint64_t get_using_memory()  const {
        return 8;
    }

    /*
    template <typename RLBWT_STR>
    void build(RLBWT_STR &_rlbwt){
        RLEFArray<RLBWT_STR> generator(_rlbwt);

        //std::vector<uint64_t> previous_c_index_vector;
        this->next_c_index_vector.resize(_rlbwt.rle_size(), std::numeric_limits<RLE_SIZE_TYPE>::max());
        this->first_c_flag_vec.resize(_rlbwt.rle_size(), false);

        RLE_SIZE_TYPE prev_c = std::numeric_limits<RLE_SIZE_TYPE>::max();
        RLE_SIZE_TYPE prev_rle_lindex_on_F = std::numeric_limits<RLE_SIZE_TYPE>::max();

        for (typename RLEFArray<RLBWT_STR>::iterator it = generator.begin(); it != generator.end(); ++it)
        {

            uint64_t rle_findex = it.rle_findex();
            RLE_SIZE_TYPE rle_lindex = it.rle_lindex();
            uint64_t c = it.character();

            if (rle_findex == 0 || prev_c != c)
            {
                first_c_flag_vec[rle_lindex] = true;
                //previous_c_index_vector[rle_lindex] = UINT64_MAX;
                //uint64_t findex = it.findex();
            }
            else
            {
                next_c_index_vector[prev_rle_lindex_on_F] = rle_lindex;
                //previous_c_index_vector[rle_lindex] = prev_rle_lindex_on_F;
            }
            prev_c = c;
            prev_rle_lindex_on_F = rle_lindex;
        }
    }
    */
};

template <typename CHAR, typename WT>
class SuccinctFIndexesLOrder{
    uint64_t _size;
    WT *wt;
    stool::EliasFanoVector X;
    std::vector<uint64_t> C_on_rlbwt;
    public:

    template <typename RLBWT_STR>
    static std::vector<typename RLBWT_STR::CHAR> get_sorted_chars(RLBWT_STR &_rlbwt){

        std::vector<typename RLBWT_STR::CHAR> chars;
        uint64_t rle_size = _rlbwt.rle_size();
        chars.resize(rle_size, 0);
        for(uint64_t i=0;i<rle_size;i++){
            chars[i] = _rlbwt.get_char_by_run_index(i);
        }
        std::sort(chars.begin(), chars.end(), [&](const typename RLBWT_STR::CHAR &lhs, const typename RLBWT_STR::CHAR &rhs) {
                    return (uint64_t)lhs < (uint64_t)rhs;
                });

        std::vector<typename RLBWT_STR::CHAR> r;
        r.push_back(chars[0]);
        for(uint64_t i=1;i<chars.size();i++){
            if(chars[i-1] != chars[i]){
                r.push_back(chars[i]);
            }
        }
        return r;

    }
    uint64_t operator[](uint64_t i) const
    {
        CHAR c = (*wt)[i];
        uint64_t pos = C_on_rlbwt[c];
        uint64_t rank = wt->rank(i+1, c);

        return this->X[pos + rank-1];
    }
    uint64_t get_using_memory() const {
        return 1 + (C_on_rlbwt.size() * sizeof(uint64_t));

    }

    template <typename RLBWT_STR>
    void build(RLBWT_STR &_rlbwt, WT *_wt){
        this->wt = _wt;
        this->_size = this->wt->size();

        std::vector<typename RLBWT_STR::CHAR> sorted_chars = get_sorted_chars(_rlbwt);

        std::vector<uint64_t> increased_vec;
        increased_vec.resize(_size+1, 0);
        uint64_t vi = 0;
        uint64_t num = 0;

        increased_vec[vi++] = 0;
        C_on_rlbwt.resize(std::numeric_limits<CHAR>::max(), UINT64_MAX);
        
        for(uint64_t i=0;i<sorted_chars.size();i++){
            CHAR c = sorted_chars[i];
            C_on_rlbwt[c] = num;
            uint64_t c_num = wt->rank(_size, c);
            for(uint64_t x=1;x<=c_num;x++){
                uint64_t pos = wt->select(x, c);
                uint64_t run = _rlbwt.get_run(pos);
                increased_vec[vi] = increased_vec[vi-1] + run;
                vi++;
            }
            num += c_num;
        }

        this->X.construct(&increased_vec);

        /*
        std::vector<uint64_t> indexes;
        indexes.resize(_size, 0);
        for(uint64_t i=0;i<_size;i++){
            indexes[i] = i;
        }
        //using RLBWT_CHAR = typename RLBWT_STR::CHAR;
        std::sort(indexes.begin(), indexes.end(), [&](auto const &lhs, auto const &rhs) {
                    auto c1 = _rlbwt.get_char_by_run_index(lhs);
                    auto c2 = _rlbwt.get_char_by_run_index(lhs);
                    if(c1 == c2){
                        return lhs < rhs;
                    }else if(c1 < c2){
                        return true;
                    }else{
                        return false;
                    }
                });

        for(uint64_t i=1;i<indexes.size();i++){
            uint64_t prev = indexes[i-1];
            uint64_t x = indexes[i]; 
            auto c = _rlbwt.get_char_by_run_index(x);
            if(_rlbwt.get_char_by_run_index(prev) != c){
                C_on_rlbwt[(CHAR)c] = i;
            }
        }

        */


    }

};

}
}