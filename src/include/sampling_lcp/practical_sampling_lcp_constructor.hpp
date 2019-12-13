#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "../rlbwt.hpp"
#include "../backward_text.hpp"
//#include "forward_sa.hpp"
#include "../rle_farray.hpp"
#include "../rlbwt_functions.hpp"
#include "./multiple_text_position_iterator.hpp"

namespace stool
{
namespace rlbwt
{


template <typename RLBWT_STR>
class PracticalSamplingLCPConstructor
{
  private:
  public:
    using CHAR = typename RLBWT_STR::char_type;
    const RLBWT_STR &_rlbwt;
    std::vector<uint64_t> _findexes_lorder;
    std::vector<uint64_t> _undetermined_rle_lindexes_of_LCP;
    std::vector<uint64_t> _previous_lindex_mapper_on_F;

    std::vector<uint64_t> _sampling_lcp_array_on_L;
    std::vector<bool> _checker;
    uint64_t current_lcp = 0;

    PracticalSamplingLCPConstructor(const RLBWT_STR &__rlbwt) : _rlbwt(__rlbwt)
    {

        this->_findexes_lorder = RLBWTFunctions::construct_fpos_array(__rlbwt);
        _sampling_lcp_array_on_L.resize(_rlbwt.rle_size(), std::numeric_limits<uint64_t>::max());
        std::vector<uint64_t> zero_lcp_findexes; 

        //std::vector<uint64_t> indexes;        
        //InitialDataStructureForSamplingLCP ids();
        //ids._sampling_lcp.resize(_rlbwt.rle_size(), std::numeric_limits<uint64_t>::max());
        //ids.construct_initial_data<CHAR>(__rlbwt);

        construct_initial_data(__rlbwt, zero_lcp_findexes, this->_undetermined_rle_lindexes_of_LCP, this->_previous_lindex_mapper_on_F, this->_sampling_lcp_array_on_L);
        _checker.resize(_rlbwt.rle_size() * 2, false);
        loop(zero_lcp_findexes);
    }
    static uint64_t getSpecialDistance(uint64_t _rle_lindex, uint64_t _diff)
    {
        return (_rle_lindex * 2) + (_diff == 0 ? 0 : 1);
    }
    void loop(std::vector<uint64_t> &zero_lcp_findexes)
    {
        MultipleTextPositionIterator<RLBWT_STR, std::vector<uint64_t>> findex_iterator(zero_lcp_findexes, _rlbwt, _findexes_lorder);

        while (this->_undetermined_rle_lindexes_of_LCP.size() > 0)
        {
            std::vector<uint64_t> result;
            std::vector<uint64_t> lcp_interval_special_positions;
            while (!findex_iterator.isEnd())
            {
                //_d_lcp_vec[*it] = it._distance;
                auto current_rle_findex = *findex_iterator;
                uint64_t lcp_interval_special_position = PracticalSamplingLCPConstructor<RLBWT_STR>::getSpecialDistance(current_rle_findex.first, current_rle_findex.second);
                if (!this->_checker[lcp_interval_special_position])
                {
                    lcp_interval_special_positions.push_back(lcp_interval_special_position);
                    this->_checker[lcp_interval_special_position] = true;
                }

                //search(*it, result, it._distance);
                //assert(_d_lcp_vec[*it] == result_lcp_vec[*it]);
                ++findex_iterator;
            }
            search(lcp_interval_special_positions, result, findex_iterator.distance() );
            findex_iterator.process();
            for (auto p : result)
            {
                findex_iterator.add(p);
            }
            //std::cout << it.size() << std::endl;
            //it.succ_process(result);
        }
    }
    bool search(std::vector<uint64_t> &x_lcp_interval_special_positions, std::vector<uint64_t> &result, uint64_t distance)
    {
        /*
        if(this->_undetermined_rle_lindexes_of_LCP.size() > 30 && x_lcp_interval_special_positions.size() > 30 ){
        std::cout << "undetermined lcps: " << this->_undetermined_rle_lindexes_of_LCP.size() << ", current lcp intervals: " << x_lcp_interval_special_positions.size() << std::endl;
        }else{
            std::cout << "+";
        }
        */
        std::sort(x_lcp_interval_special_positions.begin(), x_lcp_interval_special_positions.end());
        std::vector<uint64_t> nextIntervals;
 
        for (auto &rle_lindex_it : this->_undetermined_rle_lindexes_of_LCP)
        {
            uint64_t left = PracticalSamplingLCPConstructor<RLBWT_STR>::getSpecialDistance(_previous_lindex_mapper_on_F[rle_lindex_it], 1);
            uint64_t right = PracticalSamplingLCPConstructor<RLBWT_STR>::getSpecialDistance(rle_lindex_it, 0);


            auto pointer = std::upper_bound(x_lcp_interval_special_positions.begin(), x_lcp_interval_special_positions.end(), left);
            uint64_t next = pointer != x_lcp_interval_special_positions.end() ? *pointer : std::numeric_limits<uint64_t>::max();
            if (next <= right)
            {
                result.push_back(_findexes_lorder[rle_lindex_it]);
                _sampling_lcp_array_on_L[rle_lindex_it] = distance + 1;
            }
            else
            {
                nextIntervals.push_back(rle_lindex_it);
            }
        }
        bool b = this->_undetermined_rle_lindexes_of_LCP.size() != nextIntervals.size();

        std::swap(this->_undetermined_rle_lindexes_of_LCP, nextIntervals);



        return b;
    }

    static void construct_initial_data(const RLBWT_STR &_rlbwt, std::vector<uint64_t> &output_zero_lcp_findexes, std::vector<uint64_t> &output_non_zero_lcp_rle_lindexes, std::vector<uint64_t> &output_previous_rle_lindex_mapper_on_F, std::vector<uint64_t> &output_sampling_lcp_array_on_RLEL)
    {
        RLEFArray<RLBWT_STR > generator(_rlbwt);
        
        output_previous_rle_lindex_mapper_on_F.resize(_rlbwt.rle_size());

        uint64_t prev_c = std::numeric_limits<uint64_t>::max();
        uint64_t prev_rle_lindex_on_F = std::numeric_limits<uint64_t>::max();

        for (typename RLEFArray<RLBWT_STR >::iterator it = generator.begin(); it != generator.end(); ++it)
        {

            uint64_t rle_findex = it.rle_findex();
            uint64_t rle_lindex = it.rle_lindex();
            uint64_t c = it.character();

            if (rle_findex == 0 || prev_c != c)
            {
                output_sampling_lcp_array_on_RLEL[rle_lindex] = 0;
                uint64_t findex = it.findex();
                output_zero_lcp_findexes.push_back(findex);
                output_previous_rle_lindex_mapper_on_F[rle_lindex] = std::numeric_limits<uint64_t>::max();
            }
            else
            {
                output_non_zero_lcp_rle_lindexes.push_back(rle_lindex);
                output_previous_rle_lindex_mapper_on_F[rle_lindex] = prev_rle_lindex_on_F;
            }
            prev_c = c;
            prev_rle_lindex_on_F = rle_lindex;
        }

    }
    static std::vector<uint64_t> construct_sampling_lcp_array_lorder(const RLBWT_STR &__rlbwt)
    {
        PracticalSamplingLCPConstructor slcp(__rlbwt);
        return slcp._sampling_lcp_array_on_L;
    }

    static std::vector<uint64_t> construct_sampling_lcp_array(const RLBWT_STR &__rlbwt, std::vector<uint64_t> &__sampling_end_sa)
    {
        std::vector<uint64_t> r = construct_sampling_lcp_array_lorder(__rlbwt);
        return to_succ_sampling_lcp_array_yorder(std::move(r), __rlbwt, __sampling_end_sa);
    }
    static std::vector<uint64_t> to_succ_sampling_lcp_array_yorder(std::vector<uint64_t> &&slcp_lorder, const RLBWT_STR &__rlbwt, std::vector<uint64_t> &__sampling_end_sa)
    {
        std::vector<uint64_t> lf = RLBWTFunctions::construct_rle_lf_mapper(__rlbwt);
        std::vector<uint64_t> slcp_forder = stool::rlbwt::permutate(std::move(slcp_lorder), lf);
        lf.resize(0);
        lf.shrink_to_fit();

        //std::vector<uint64_t> yf = construct_yf_mapper(__rlbwt,__sampling_end_sa);
        std::vector<uint64_t> yf = stool::rlbwt::get_sorted_positions(__sampling_end_sa);
        std::vector<uint64_t> succ_slcp = stool::rlbwt::rotate(std::move(slcp_forder));
        std::vector<uint64_t> fy = stool::rlbwt::change_inv(std::move(yf));
        std::vector<uint64_t> succ_slcp_yorder = stool::rlbwt::permutate(std::move(succ_slcp), fy);
        return succ_slcp_yorder;
        //to_ylcp(slcp_lorder, yf);
    }

    /*
    static void to_ylcp(std::vector<uint64_t> &slcp, std::vector<uint64_t> &yf)
    {
        uint64_t rle_size = yf.size();
        std::vector<uint64_t> tmp;
        tmp.resize(rle_size, 0);

        for (uint64_t i = 0; i < rle_size; i++)
        {
            tmp[i] = yf[i] + 1 == rle_size ? std::numeric_limits<uint64_t>::max() : slcp[yf[i] + 1];
        }
        std::swap(tmp, slcp);
    }
    */
};
} // namespace rlbwt
} // namespace stool