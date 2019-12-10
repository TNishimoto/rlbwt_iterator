#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "../sampling_lcp.hpp"
#include "./succinct_interval_tree.hpp"

namespace stool
{
namespace rlbwt
{


class RLBWTLeftIntervals{
    public:
    std::vector<uint64_t> *previous_c_index_vector;
    RLBWTLeftIntervals(std::vector<uint64_t> *_previous_c_index_vector) : previous_c_index_vector(_previous_c_index_vector) {

    }
    uint64_t operator[](uint64_t i) const
    {        
        uint64_t x = (*previous_c_index_vector)[i];
        return ((x+1) * 2);
    }
};

class RLBWTRightIntervals{
    public:
    RLBWTRightIntervals() {

    }
    uint64_t operator[](uint64_t i) const
    {
        return i * 2;
    }
};

template <typename RLBWT_STR>
class SamplingLCP2
{
private:
public:
    using CHAR = typename RLBWT_STR::char_type;
    const RLBWT_STR &_rlbwt;
    static std::vector<uint64_t> compute_slcp_array_on_L(const RLBWT_STR &__rlbwt)
    {
        std::vector<uint64_t> previous_c_index_vector = construct_initial_data(__rlbwt);
        std::vector<uint64_t> __sampling_lcp_array_on_L = loop(__rlbwt, previous_c_index_vector);
        return __sampling_lcp_array_on_L;
    }
    static uint64_t getSpecialDistance(uint64_t _rle_lindex, uint64_t _diff)
    {
        return (_rle_lindex * 2) + (_diff == 0 ? 0 : 1);
    }
    static std::vector<uint64_t> loop(const RLBWT_STR &_rlbwt, std::vector<uint64_t> &previous_c_index_vector)
    {
        RLBWTLeftIntervals left_intervals(&previous_c_index_vector);
        RLBWTRightIntervals right_intervals;
        std::vector<bool> interval_flag_vec;
        interval_flag_vec.resize(previous_c_index_vector.size(), false);
        SuccinctIntervalTree<uint64_t, uint8_t, RLBWTLeftIntervals, RLBWTRightIntervals> intervalTree;
        intervalTree.initialize(getSpecialDistance(previous_c_index_vector.size(), 1));

        std::vector<bool> _checker;
        _checker.resize(_rlbwt.rle_size() * 2, false);

        //std::vector<uint64_t> lf = RLBWTFunctions::construct_rle_lf_mapper(_rlbwt);
        auto _findexes_lorder = RLBWTFunctions::construct_fpos_array(_rlbwt);
        std::vector<uint64_t> zero_lcp_findexes;

        std::vector<uint64_t> __sampling_lcp_array_on_L;
        __sampling_lcp_array_on_L.resize(_rlbwt.rle_size(), UINT64_MAX);

        NaiveIntervalTree nit;
        //std::cout << "PCI:" << std::flush;
        //Printer::print(previous_c_index_vector);
        uint64_t interval_count = 0;
        for (uint64_t i = 0; i < previous_c_index_vector.size(); i++)
        {
            if (previous_c_index_vector[i] == UINT64_MAX)
            {
                zero_lcp_findexes.push_back(_findexes_lorder[i]);
                __sampling_lcp_array_on_L[i] = 0;
            }
            else
            {
                uint64_t prev_index = previous_c_index_vector[i] + 1;
                interval_flag_vec[i] = true;
                interval_count++;
                uint64_t _i = getSpecialDistance(prev_index, 0);
                uint64_t _j = getSpecialDistance(i, 0);
                std::cout << "add:" << i << "@"<< _i << ", " << _j << std::endl;
                nit.add(_i, _j);
            }
        }
        intervalTree.construct(&left_intervals, &right_intervals, interval_count, interval_flag_vec);

        //Printer::print(zero_lcp_findexes);

        MultipleTextPositionIterator<RLBWT_STR> findex_iterator(zero_lcp_findexes, _rlbwt, _findexes_lorder);

        uint64_t nokori_counter = interval_count;
        while (nokori_counter > 0)
        {
            std::vector<uint64_t> reportedIndexes;
            //std::cout << "nit size:" << nit.items.size() << std::endl;
            while (!findex_iterator.isEnd())
            {
                auto current_rle_findex = *findex_iterator;
                uint64_t pos = getSpecialDistance(current_rle_findex.first, current_rle_findex.second);
                if (!_checker[pos])
                {
                    //auto r = nit.report_and_remove(pos);
                    //std::cout << "report by " << pos << ", result is " << r.size() << std::endl;
                    
                    auto r = intervalTree.report_and_remove(pos);
                    //std::cout << "report by " << pos << ", : " << std::flush;
                    for (auto rep : r)
                    {
                        nokori_counter--;
                        //std::cout << rep <<  "[" << left_intervals[rep] << "," << right_intervals[rep] << "]"<< ", "<< std::flush;
                        reportedIndexes.push_back(rep);
                    }
                    //std::cout  << std::endl;
                    

                    auto r2 = nit.report_and_remove(pos);
                    assert(r.size() == r2.size() );

                    std::cout << "report : " << std::flush;
                    for (auto rep : r2)
                    {
                        uint64_t index = rep.second / 2;
                        std::cout << index << ", "<< std::flush;
                        //reportedIndexes.push_back(index);
                    }
                    std::cout  << std::endl;
                    
                    

                    _checker[pos] = true;
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

    static std::vector<uint64_t> construct_initial_data(const RLBWT_STR &_rlbwt)
    {
        RLEFArray<RLBWT_STR> generator(_rlbwt);

        std::vector<uint64_t> previous_c_index_vector;
        previous_c_index_vector.resize(_rlbwt.rle_size(), UINT64_MAX);

        //output_previous_rle_lindex_mapper_on_F.resize(_rlbwt.rle_size());

        uint64_t prev_c = std::numeric_limits<uint64_t>::max();
        uint64_t prev_rle_lindex_on_F = std::numeric_limits<uint64_t>::max();

        for (typename RLEFArray<RLBWT_STR>::iterator it = generator.begin(); it != generator.end(); ++it)
        {

            uint64_t rle_findex = it.rle_findex();
            uint64_t rle_lindex = it.rle_lindex();
            uint64_t c = it.character();

            //std::cout << (char)c << "/" << rle_findex << "/" << rle_lindex << std::endl;

            if (rle_findex == 0 || prev_c != c)
            {
                previous_c_index_vector[rle_lindex] = UINT64_MAX;
                //output_sampling_lcp_array_on_RLEL[rle_lindex] = 0;
                uint64_t findex = it.findex();
                //output_zero_lcp_findexes.push_back(findex);
                //output_previous_rle_lindex_mapper_on_F[rle_lindex] = std::numeric_limits<uint64_t>::max();
            }
            else
            {
                previous_c_index_vector[rle_lindex] = prev_rle_lindex_on_F;
                //output_non_zero_lcp_rle_lindexes.push_back(rle_lindex);
                //output_previous_rle_lindex_mapper_on_F[rle_lindex] = prev_rle_lindex_on_F;
            }
            prev_c = c;
            prev_rle_lindex_on_F = rle_lindex;
        }
        return previous_c_index_vector;
    }
    static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT_STR &__rlbwt)
    {
        //SamplingLCP2 slcp(__rlbwt);
        return SamplingLCP2::compute_slcp_array_on_L(__rlbwt);
    }
};

} // namespace rlbwt
} // namespace stool