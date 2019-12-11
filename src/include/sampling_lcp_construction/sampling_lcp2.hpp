#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "../sampling_lcp.hpp"
#include "./succinct_interval_tree.hpp"
#include "stool/src/debug.hpp"

namespace stool
{
namespace rlbwt
{

template <typename RLE_SIZE_TYPE>
class SelectOnRLBWT{
    public:
    //const RLBWT_STR *_rlbwt;
    std::vector<RLE_SIZE_TYPE> next_c_index_vector;
    std::vector<bool> first_c_flag_vec;

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
    /*
    uint64_t get_next_count(){
        uint64_t count;
        for(uint64_t i=0;i<next_c_index_vector.size();i++){
            if(next_c_index_vector[i] != UINT64_MAX) count++;
        }
    }
    */
    
};

class RLBWTLeftIntervals{
    public:
    RLBWTLeftIntervals() {

    }
    uint64_t operator[](uint64_t i) const
    {        
        return ((i+1) * 2);
    }
};

template <typename RLE_SIZE_TYPE>
class RLBWTRightIntervals{
    public:
    std::vector<RLE_SIZE_TYPE> *next_c_index_vector;
    RLBWTRightIntervals(std::vector<RLE_SIZE_TYPE> *_next_c_index_vector) : next_c_index_vector(_next_c_index_vector) {

    }
    uint64_t operator[](uint64_t i) const
    {
        uint64_t x = (*next_c_index_vector)[i];
        return x * 2;
    }
};

template <typename RLBWT_STR>
class SamplingLCP2
{
private:
public:
    using CHAR = typename RLBWT_STR::char_type;
    /*
    const RLBWT_STR &_rlbwt;
    static std::vector<uint64_t> compute_slcp_array_on_L(const RLBWT_STR &__rlbwt)
    {
        //std::vector<uint64_t> previous_c_index_vector = construct_initial_data(__rlbwt);
        std::vector<uint64_t> __sampling_lcp_array_on_L = loop(__rlbwt);
        return __sampling_lcp_array_on_L;
    }
    */
    static uint64_t getSpecialDistance(uint64_t _rle_lindex, uint64_t _diff)
    {
        return (_rle_lindex * 2) + (_diff == 0 ? 0 : 1);
    }
    template <typename RLE_SIZE_TYPE>
    static std::vector<uint64_t> construct_zero_lcp_findexes(SelectOnRLBWT<RLE_SIZE_TYPE> &selecter, std::vector<uint64_t> &_findexes_lorder){
        std::vector<uint64_t> zero_lcp_findexes;
        for (uint64_t i = 0; i < selecter.first_c_flag_vec.size(); i++)
        {
            if(selecter.first_c_flag_vec[i]){
                zero_lcp_findexes.push_back(_findexes_lorder[i]);
            }
        }
        return zero_lcp_findexes;

    }

    template <typename RLE_SIZE_TYPE>
    static std::vector<bool> construct_interval_flag_vec(SelectOnRLBWT<RLE_SIZE_TYPE> &selecter){
        std::vector<bool> interval_flag_vec;
        interval_flag_vec.resize(selecter.next_c_index_vector.size(), false);
        for(uint64_t i=0;i<selecter.next_c_index_vector.size();i++){
            interval_flag_vec[i] = selecter.next_c_index_vector[i] != std::numeric_limits<RLE_SIZE_TYPE>::max();
        }

        return interval_flag_vec;
        
    }

    template <typename RLE_SIZE_TYPE>
    static std::vector<uint64_t> compute_slcp_array_on_L(const RLBWT_STR &_rlbwt)
    {
        uint64_t run_size = _rlbwt.rle_size();
        SelectOnRLBWT<RLE_SIZE_TYPE> selecter;
        selecter.build(_rlbwt);

        std::vector<bool> _access_checker;
        _access_checker.resize(run_size * 2, false);

        auto _findexes_lorder = RLBWTFunctions::construct_fpos_array(_rlbwt);
        

        std::vector<bool> interval_flag_vec = construct_interval_flag_vec(selecter);
        std::vector<uint64_t> zero_lcp_findexes = construct_zero_lcp_findexes(selecter, _findexes_lorder);


        RLBWTLeftIntervals left_intervals;
        RLBWTRightIntervals<RLE_SIZE_TYPE> right_intervals(&selecter.next_c_index_vector);
        SuccinctIntervalTree<RLE_SIZE_TYPE, uint8_t, RLBWTLeftIntervals, RLBWTRightIntervals<RLE_SIZE_TYPE>> intervalTree;
        intervalTree.initialize(getSpecialDistance(run_size, 1));
        intervalTree.construct(&left_intervals, &right_intervals, interval_flag_vec);

        //Printer::print(zero_lcp_findexes);

        MultipleTextPositionIterator<RLBWT_STR> findex_iterator(zero_lcp_findexes, _rlbwt, _findexes_lorder);

        uint64_t interval_count = run_size - zero_lcp_findexes.size();
        uint64_t nokori_counter = interval_count;

        std::vector<uint64_t> __sampling_lcp_array_on_L;
        __sampling_lcp_array_on_L.resize(_rlbwt.rle_size(), 0);
        while (nokori_counter > 0)
        {
            std::vector<uint64_t> reportedIndexes;
            //std::cout << "nit size:" << nit.items.size() << std::endl;
            while (!findex_iterator.isEnd())
            {
                auto current_rle_findex = *findex_iterator;
                uint64_t pos = getSpecialDistance(current_rle_findex.first, current_rle_findex.second);
                if (!_access_checker[pos])
                {
                    //auto r = nit.report_and_remove(pos);
                    //std::cout << "report by " << pos << ", result is " << r.size() << std::endl;
                    
                    auto r = intervalTree.report_and_remove(pos);
                    //std::cout << "report by " << pos << ", : " << std::flush;
                    for (auto rep : r)
                    {
                        nokori_counter--;
                        //std::cout << rep <<  "[" << left_intervals[rep] << "," << right_intervals[rep] << "]"<< ", "<< std::flush;
                        reportedIndexes.push_back(selecter.next_c_index_vector[rep]);
                    }
                    //std::cout  << std::endl;
                    
                    /*
                    auto r2 = nit.report_and_remove(pos);
                    std::vector<uint64_t> r3;
                    //assert(r.size() == r2.size());

                    //std::cout << "report : " << std::flush;
                    for (auto rep : r2)
                    {
                        uint64_t index = rep.second / 2;
                        //std::cout << index << ", "<< std::flush;
                        r3.push_back(index);
                        //reportedIndexes.push_back(index);
                    }
                    */
                    //std::cout  << "+"<< std::flush;
                    //std::sort(r.begin(), r.end());
                    //std::sort(r3.begin(), r3.end());
                    //stool::Printer::print(r);
                    //stool::Printer::print(r3);

                    //bool b2 = stool::equal_check<uint64_t>(r3, r);
                    
                    

                    _access_checker[pos] = true;
                }

                //std::cout << current_rle_findex.first << "/" << current_rle_findex.second << std::endl;
                ++findex_iterator;
            }
            uint64_t distance = findex_iterator.distance();
            findex_iterator.process();
            for (auto p : reportedIndexes)
            {
                __sampling_lcp_array_on_L[p] = distance + 1;
                //std::cout << "SLCP[" << lf[p] <<  "] = " << (distance + 1) << std::endl;
                findex_iterator.add(_findexes_lorder[p]);
            }
        }
        return __sampling_lcp_array_on_L;
    }
    static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT_STR &__rlbwt)
    {
        //SamplingLCP2 slcp(__rlbwt);
        return SamplingLCP2::compute_slcp_array_on_L<uint64_t>(__rlbwt);
    }
};

} // namespace rlbwt
} // namespace stool